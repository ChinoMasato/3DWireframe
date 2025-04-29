/*
 * Camera.cpp
 *
 * 元のコード (最初のバージョン) からの改善点と修正内容の説明:
 * (このファイルは、元のコードから大幅に機能拡張された後のバージョンです)
 *
 * [元のコード(最初のバージョン)の問題点 - このコードで修正された点]
 * 1. カメラの回転方法:
 *    - 最初のバージョン: オイラー角 (`rotation` 変数) で管理 → ジンバルロックの可能性。
 *    - このコード: クォータニオン (`orientation` 変数) で管理 → ジンバルロック回避、滑らかな回転合成。
 *      (Update関数内でマウス/キー入力から差分回転クォータニオンを作り合成・適用)
 *
 * 2. カメラの移動方法:
 *    - 最初のバージョン: ワールド軸基準で移動 → カメラの向きと移動方向が一致せず不自然。
 *    - このコード: カメラのローカル軸 (Forward, Right, Up) 基準で移動 → FPS視点の直感的な操作感。
 *      (Update関数内で現在の向きベクトルを取得し、それに沿って移動量を計算)
 *
 * 3. 座標変換と描画:
 *    - 最初のバージョン: 簡略化された透視計算 (`GetScreenPos`) → 不正確な遠近感・視野角。
 *    - このコード: ビュー行列・プロジェクション行列を用いた標準的な3D座標変換パイプラインを導入。
 *      (`GetViewMatrix`, `GetProjectionMatrix` で行列を計算し、`Draw` や `WorldToScreen` で使用)
 *      → 正確な遠近感と視野角の表現。
 *
 * 4. クリッピング:
 *    - 最初のバージョン: Z座標のみの単純なチェック → 画面外描画の問題。
 *    - このコード: Cohen-Sutherlandアルゴリズム (`ClipLineCohenSutherland`) による視錐台クリッピングを実装。
 *      → 画面内外の線分を正しく処理。
 *
 * [このコードに関する注意点]
 * - 回転の適用順序: `Update` 関数内の `orientation = deltaRotation * orientation;` は
 *   ワールド空間での回転適用に相当します。FPS視点としては `orientation = orientation * deltaRotation;`
 *   (ローカル空間での適用) の方が自然な操作感になることが多いです。
 * - ピッチ角制限: このコードには上下方向の回転 (ピッチ) を制限する機能が含まれていません。
 *   そのため、カメラが真上や真下を通り越して回転する可能性があります。
 * - メンバ変数の依存: このコードは、`Camera.h` で `fovY`, `aspectRatio`, `nearZ`, `farZ`,
 *   `lastMouseMoveX`, `lastMouseMoveY`, `lastYawAngle`, `lastPitchAngle`, `lastRollAngle`,
 *   `currentForward`, `currentRight`, `currentUp`, `lastMoveForward`, `lastMoveRight`, `lastMoveUp`,
 *   `lastWorldMoveOffset` などのメンバ変数が正しく宣言・初期化されていることを前提としています。
 *
 */
#define NOMINMAX
#include "Camera.h"
#include "DxLib.h"
#include "CameraMath.h" // 必要に応じてインクルード (PerspectiveFovLHなど)
#include "Common.h"     // 必要に応じてインクルード (WINDOW_WIDTHなど)
#include <cmath>        // fabsf など (<math.h> より推奨)
#include <algorithm>    // std::min, std::max (現在は未使用)
#include <stdexcept>    // 例外 (現在は未使用)
#include <string>       // std::string
#include <sstream>      // std::stringstream
#include <iomanip>      // std::setprecision
#include "Logger.h"     // LogDebug など (必要に応じてインクルード)
#include "Vector.h"     // Vector3D, Vector4D (必要に応じてインクルード)
#include "Matrix.h"     // Matrix, VEC4Transform など (必要に応じてインクルード)
#include "Quaternion.h" // Quaternion (必要に応じてインクルード)

 // --- 匿名名前空間 ---
 // このファイル内でのみ有効なヘルパー関数や定数
namespace {
    // 4次元ベクトルの線形補間
    inline Vector4D VectorLerp4D(const Vector4D& start, const Vector4D& end, float t) {
        return {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t,
            start.z + (end.z - start.z) * t,
            start.w + (end.w - start.w) * t
        };
    }

    // Cohen-Sutherland クリッピング用アウトコード定数
    const int INSIDE = 0;
    const int LEFT = 1;
    const int RIGHT = 2;
    const int BOTTOM = 4;
    const int TOP = 8;
    const int OUTCODE_NEAR = 16;
    const int OUTCODE_FAR = 32;

    // 点のアウトコードを計算する関数 (クリップ座標系)
    // 元のコードのロジックを保持
    int ComputeOutCode(const Vector4D& p) {
        int code = INSIDE;
        if (p.x < -p.w) { code |= LEFT; }
        else if (p.x > p.w) { code |= RIGHT; }
        if (p.y < -p.w) { code |= BOTTOM; }
        else if (p.y > p.w) { code |= TOP; }
        if (p.z < 0.0f) { code |= OUTCODE_NEAR; }
        else if (p.z > p.w) { code |= OUTCODE_FAR; }
        return code;
    }

    // Cohen-Sutherland 線分クリッピング関数
    // 戻り値: 線分が一部でも描画領域内にあれば true
    // 元のコードのロジックを保持
    bool ClipLineCohenSutherland(Vector4D& p1_clip, Vector4D& p2_clip) {
        int outcode1 = ComputeOutCode(p1_clip);
        int outcode2 = ComputeOutCode(p2_clip);
        const int MAX_ITERATIONS = 10;
        int iterations = 0;

        while (iterations < MAX_ITERATIONS) {
            iterations++;
            if ((outcode1 | outcode2) == 0) { // Trivial Accept
                return true;
            }
            else if ((outcode1 & outcode2) != 0) { // Trivial Reject
                return false;
            }
            else { // Clipping needed
                int outcode_outside = (outcode1 != 0) ? outcode1 : outcode2;
                float t = 0.0f;
                Vector4D intersection_point;
                float dx = p2_clip.x - p1_clip.x, dy = p2_clip.y - p1_clip.y;
                float dz = p2_clip.z - p1_clip.z, dw = p2_clip.w - p1_clip.w;
                float denominator;

                // Calculate intersection parameter t
                if (outcode_outside & LEFT) {
                    denominator = dx + dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (-p1_clip.x - p1_clip.w) / denominator;
                }
                else if (outcode_outside & RIGHT) {
                    denominator = dx - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (p1_clip.w - p1_clip.x) / denominator;
                }
                else if (outcode_outside & BOTTOM) {
                    denominator = dy + dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (-p1_clip.y - p1_clip.w) / denominator;
                }
                else if (outcode_outside & TOP) {
                    denominator = dy - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (p1_clip.w - p1_clip.y) / denominator;
                }
                else if (outcode_outside & OUTCODE_NEAR) {
                    if (std::fabsf(dz) < 1e-6f) { return false; }
                    t = -p1_clip.z / dz;
                }
                else if (outcode_outside & OUTCODE_FAR) {
                    denominator = dz - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (p1_clip.w - p1_clip.z) / denominator;
                }
                else {
                    return false; // Should not happen
                }

                // Check parameter validity
                if (t < 0.0f || t > 1.0f) { return false; }

                // Calculate intersection point
                intersection_point = VectorLerp4D(p1_clip, p2_clip, t);

                // Update the clipped point and its outcode
                if (outcode_outside == outcode1) {
                    p1_clip = intersection_point;
                    outcode1 = ComputeOutCode(p1_clip);
                }
                else {
                    p2_clip = intersection_point;
                    outcode2 = ComputeOutCode(p2_clip);
                }
            }
        }
        return false; // Max iterations reached
    }

    // カメラ制御用の定数
    static const float MOVE_SPEED = 2.5f;
    // static const float ROTATION_SENSITIVITY = 5.0f; // 使われていない
    static const float MOUSE_ANGLE_RATE = ONE_DEGREE * 0.1f;
    static const float ROLL_SPEED = 2.0f * ONE_DEGREE;

    // --- Camera クラスに必要なメンバ変数の仮宣言 (コメント) ---
    // float fovY; float aspectRatio; float nearZ; float farZ;
    // int lastMouseMoveX, lastMouseMoveY;
    // float lastYawAngle, lastPitchAngle, lastRollAngle;
    // Vector3D currentForward, currentRight, currentUp;
    // float lastMoveForward, lastMoveRight, lastMoveUp;
    // Vector3D lastWorldMoveOffset;

} // 匿名名前空間の終わり

// --- Camera クラス メンバー関数の実装 ---

// コンストラクタ
Camera::Camera() : position{ 0.0f, 0.0f, -50.0f }, orientation() {
    // マウスカーソルを画面中央に初期化
    SetMousePoint(static_cast<int>(WINDOW_WIDTH / 2), static_cast<int>(WINDOW_HEIGHT / 2));
    // 必要に応じて他のメンバ変数(fovYなど)もここで初期化
}

// デストラクタ
Camera::~Camera() {}

// カメラ位置を返す Getter
Vector3D Camera::GetPosition() const {
    return position;
}

// ビュー行列を計算して返す Getter
Matrix Camera::GetViewMatrix() const {
    Quaternion invOrientation = orientation.Conjugate();
    Matrix invRotationMatrix = invOrientation.ToRotationMatrix();
    Matrix invTranslate = Matrix::Identity(); // Matrix::Translation(-position) がより適切
    invTranslate.m[3][0] = -position.x;
    invTranslate.m[3][1] = -position.y;
    invTranslate.m[3][2] = -position.z;
    Matrix viewMatrix = MatrixMultiply(invTranslate, invRotationMatrix); // 行列の乗算順序が重要
    return viewMatrix;
}

// プロジェクション行列を計算して返す Getter
Matrix Camera::GetProjectionMatrix() const {
    // Camera.hで定義された fovY, aspectRatio, nearZ, farZ を使う想定
    // return PerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
    // 仮の値 (もしメンバ変数が未定義の場合)
    return PerspectiveFovLH(60.0f * ONE_DEGREE, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);
}

// カメラの前方ベクトルを計算して返す Getter
Vector3D Camera::GetForwardVector() const {
    Matrix rotMat = orientation.ToRotationMatrix();
    // VEC3TransformNormal + Normalized() の方が方向ベクトルとしては正確
    return VEC3Transform({ 0.0f, 0.0f, 1.0f }, rotMat);
}

// カメラの右方ベクトルを計算して返す Getter
Vector3D Camera::GetRightVector() const {
    Matrix rotMat = orientation.ToRotationMatrix();
    // VEC3TransformNormal + Normalized() の方が方向ベクトルとしては正確
    return VEC3Transform({ 1.0f, 0.0f, 0.0f }, rotMat);
}

// カメラの上方ベクトルを計算して返す Getter
Vector3D Camera::GetUpVector() const {
    Matrix rotMat = orientation.ToRotationMatrix();
    // VEC3TransformNormal + Normalized() の方が方向ベクトルとしては正確
    return VEC3Transform({ 0.0f, 1.0f, 0.0f }, rotMat);
}

// ワールド座標をスクリーン座標に変換するヘルパー関数 (クラス外)
Vector3D WorldToScreen(const Vector3D& worldPos, const Matrix& viewProjMatrix) {
    Vector4D clipPos = VEC4Transform({ worldPos.x, worldPos.y, worldPos.z, 1.0f }, viewProjMatrix);
    if (std::fabsf(clipPos.w) < 1e-6f || clipPos.z < 0.0f || clipPos.z > clipPos.w) {
        return { -1.0f, -1.0f, 0.0f }; // 範囲外なら無効値を返す
    }
    Vector3D ndc = { clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w };
    float hW = WINDOW_WIDTH / 2.0f;
    float hH = WINDOW_HEIGHT / 2.0f;
    int sx = static_cast<int>(ndc.x * hW + hW); // 元のコードの int キャストを保持
    int sy = static_cast<int>(-ndc.y * hH + hH); // Y軸反転 & int キャスト
    return { (float)sx, (float)sy, ndc.z }; // float に戻して返す
}

// ワールドの線分を描画するメソッド
void Camera::Draw(const std::vector<std::vector<Vector3D>>& worldLines) {
    Matrix viewMatrix = GetViewMatrix();
    Matrix projMatrix = GetProjectionMatrix();
    Matrix viewProjMatrix = MatrixMultiply(viewMatrix, projMatrix);

    for (const auto& line : worldLines) { // 各線分についてループ
        if (line.size() < 2) { continue; } // 頂点が2つ未満ならスキップ

        // 始点・終点をクリップ座標に変換
        Vector4D p1_clip = VEC4Transform({ line[0].x, line[0].y, line[0].z, 1.0f }, viewProjMatrix);
        Vector4D p2_clip = VEC4Transform({ line[1].x, line[1].y, line[1].z, 1.0f }, viewProjMatrix);
        Vector4D p1_clipped = p1_clip; // クリッピング用にコピー
        Vector4D p2_clipped = p2_clip;

        // 線分をクリッピング (元のコードのロジックを保持)
        if (ClipLineCohenSutherland(p1_clipped, p2_clipped)) {
            // NDC/スクリーン座標計算 (元のコードのロジックを保持)
            Vector3D p1_ndc = { 0.0f, 0.0f, 0.0f };
            Vector3D p2_ndc = { 0.0f, 0.0f, 0.0f };
            if (fabsf(p1_clipped.w) > 1e-6f && fabsf(p2_clipped.w) > 1e-6f) { // wチェック
                p1_ndc = { p1_clipped.x / p1_clipped.w, p1_clipped.y / p1_clipped.w, p1_clipped.z / p1_clipped.w };
                p2_ndc = { p2_clipped.x / p2_clipped.w, p2_clipped.y / p2_clipped.w, p2_clipped.z / p2_clipped.w };
                float hW = WINDOW_WIDTH / 2.f, hH = WINDOW_HEIGHT / 2.f;
                int sx1 = static_cast<int>(p1_ndc.x * hW + hW);
                int sy1 = static_cast<int>(-p1_ndc.y * hH + hH);
                int sx2 = static_cast<int>(p2_ndc.x * hW + hW);
                int sy2 = static_cast<int>(-p2_ndc.y * hH + hH);
                DrawLine(sx1, sy1, sx2, sy2, GetColor(255, 255, 255)); // 描画
            }
        }
    }
    // デバッグ用移動線描画 (lastWorldMoveOffset メンバ変数が必要)
    // 元のコードのロジックを保持
    // if (lastWorldMoveOffset.LengthSq() > 1e-12f) {
    //     Vector3D moveDir = lastWorldMoveOffset.Normalized();
    //     Vector3D moveEndPos = position + moveDir * 10.0f;
    //     Vector3D startScreen = WorldToScreen(position, viewProjMatrix);
    //     Vector3D endScreen = WorldToScreen(moveEndPos, viewProjMatrix);
    //     if (startScreen.x >= 0.0f && endScreen.x >= 0.0f) {
    //         DrawLine(static_cast<int>(startScreen.x), static_cast<int>(startScreen.y),
    //                  static_cast<int>(endScreen.x), static_cast<int>(endScreen.y),
    //                  GetColor(0, 255, 255));
    //     }
    // }
} // Draw 関数の終わり


// カメラの状態を更新するメソッド
void Camera::Update() {
    // --- 1. マウスによる視点回転 ---
    int currentMouseX = 0, currentMouseY = 0;
    GetMousePoint(&currentMouseX, &currentMouseY);
    const int centerX = static_cast<int>(WINDOW_WIDTH / 2);
    const int centerY = static_cast<int>(WINDOW_HEIGHT / 2);
    int mouseMoveX = currentMouseX - centerX;
    int mouseMoveY = currentMouseY - centerY;
    // カメラクラスのメンバ変数に保存する想定
    lastMouseMoveX = mouseMoveX;
    lastMouseMoveY = mouseMoveY;

    // ローカル軸周りの回転角度を計算 (ラジアン)
    // カメラクラスのメンバ変数に保存する想定
    lastYawAngle = static_cast<float>(mouseMoveX) * MOUSE_ANGLE_RATE;
    lastPitchAngle = static_cast<float>(mouseMoveY) * MOUSE_ANGLE_RATE;

    // --- 2. キーボードによるロール回転 ---
    // カメラクラスのメンバ変数に保存する想定
    lastRollAngle = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) { lastRollAngle += ROLL_SPEED; }
    if (CheckHitKey(KEY_INPUT_Q)) { lastRollAngle -= ROLL_SPEED; }

    // --- 3. 回転の適用 ---
    // 現在のカメラのローカル軸を取得
    // カメラクラスのメンバ変数に保存する想定
    currentUp = GetUpVector();
    currentRight = GetRightVector();
    currentForward = GetForwardVector();

    // 各ローカル軸周りの差分回転クォータニオンを生成
    Quaternion yawDelta = Quaternion::FromAxisAngle(currentUp, lastYawAngle);
    Quaternion pitchDelta = Quaternion::FromAxisAngle(currentRight, lastPitchAngle);
    Quaternion rollDelta = Quaternion::FromAxisAngle(currentForward, lastRollAngle);

    // 差分回転を合成 (Roll -> Pitch -> Yaw)
    Quaternion deltaRotation = rollDelta * pitchDelta * yawDelta;

    // 現在の姿勢に差分回転を適用 (ワールド回転である点に注意)
    orientation = deltaRotation * orientation;
    orientation.Normalize(); // 正規化

    // ピッチ角制限は削除 // 元のコードにあったコメント

    // --- 4. カメラの向きベクトル再計算 ---
    // カメラクラスのメンバ変数に保存する想定
    currentForward = GetForwardVector();
    currentRight = GetRightVector();
    currentUp = GetUpVector();

    // --- 5. キーボードによる移動入力 ---
    // カメラクラスのメンバ変数に保存する想定
    lastMoveForward = 0.0f;
    lastMoveRight = 0.0f;
    lastMoveUp = 0.0f;
    if (CheckHitKey(KEY_INPUT_W)) { lastMoveForward += 1.0f; }
    if (CheckHitKey(KEY_INPUT_S)) { lastMoveForward -= 1.0f; }
    if (CheckHitKey(KEY_INPUT_D)) { lastMoveRight += 1.0f; }
    if (CheckHitKey(KEY_INPUT_A)) { lastMoveRight -= 1.0f; }
    if (CheckHitKey(KEY_INPUT_SPACE)) { lastMoveUp += 1.0f; }
    if (CheckHitKey(KEY_INPUT_LCONTROL)) { lastMoveUp -= 1.0f; }

    // --- 6. 移動量の計算と位置更新 ---
    // カメラクラスのメンバ変数に保存する想定
    lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f };
    lastWorldMoveOffset += currentForward * lastMoveForward * MOVE_SPEED;
    lastWorldMoveOffset += currentRight * lastMoveRight * MOVE_SPEED;
    lastWorldMoveOffset += currentUp * lastMoveUp * MOVE_SPEED;

    position += lastWorldMoveOffset; // 位置を更新

    // --- 7. マウスカーソルを画面中央に戻す ---
    SetMousePoint(centerX, centerY);

    // --- デバッグログ出力 ---
    LogDebug(GetDetailedDebugInfo()); // GetDetailedDebugInfoが実装されていれば呼び出す
} // Update 関数の終わり

// デバッグ情報 (画面表示用)
std::string Camera::GetDebugInfo() const {
    // 元のコードの通り。メンバ変数 (lastMouseMoveX/Y, lastMove*) が必要。
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Pos:(" << position.x << "," << position.y << "," << position.z << ") ";
    ss << "Mouse:(" << lastMouseMoveX << "," << lastMouseMoveY << ") ";
    float rollInput = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) { rollInput += 1.0f; }
    if (CheckHitKey(KEY_INPUT_Q)) { rollInput -= 1.0f; }
    ss << "Key: F" << lastMoveForward << " R" << lastMoveRight << " U" << lastMoveUp << " Roll" << rollInput;
    return ss.str();
} // GetDebugInfo 関数の終わり

// デバッグ情報 (ログ用)
std::string Camera::GetDetailedDebugInfo() const {
    // 元のコードの通り。メンバ変数 (last*, current*) が必要。
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << "Pos(x:" << position.x << ",y:" << position.y << ",z:" << position.z << ") ";
    ss << "Ori(x:" << orientation.x << ",y:" << orientation.y << ",z:" << orientation.z << ",w:" << orientation.w << ") ";
    ss << "MouseIn(x:" << lastMouseMoveX << ",y:" << lastMouseMoveY << ") ";
    ss << "RotAngle(Yaw:" << lastYawAngle * 180 / PI << ",Pitch:" << lastPitchAngle * 180 / PI << ",Roll:" << lastRollAngle * 180 / PI << ") ";
    float rollInput = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) { rollInput += 1.0f; }
    if (CheckHitKey(KEY_INPUT_Q)) { rollInput -= 1.0f; }
    ss << "KeyIn(F:" << lastMoveForward << ",R:" << lastMoveRight << ",U:" << lastMoveUp << ",Roll:" << rollInput << ") ";
    ss << "AxisF(x:" << currentForward.x << ",y:" << currentForward.y << ",z:" << currentForward.z << ") ";
    ss << "AxisR(x:" << currentRight.x << ",y:" << currentRight.y << ",z:" << currentRight.z << ") ";
    ss << "AxisU(x:" << currentUp.x << ",y:" << currentUp.y << ",z:" << currentUp.z << ") ";
    ss << "MoveOffset(x:" << lastWorldMoveOffset.x << ",y:" << lastWorldMoveOffset.y << ",z:" << lastWorldMoveOffset.z << ")";
    return ss.str();
} // GetDetailedDebugInfo 関数の終わり