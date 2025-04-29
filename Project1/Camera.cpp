/*
 * Camera.cpp
 *
 * 元のコード (最初のバージョン) からの改善点と修正内容の説明:
 * (このファイルは、元のコードから大幅に機能拡張された後のバージョンです)
 *
 * [元のコード(最初のバージョン)の問題点 と このコードでの対応]
 *
 * 1. カメラの回転方法について:
 *    - 最初のバージョンでは、X軸とY軸の回転角度（オイラー角）を直接 `rotation` という Vector3D 型の変数で管理していました。
 *    - オイラー角は直感的ですが、「ジンバルロック」という現象が起こる欠点があります。
 *      これは、特定の角度（例えば真上を向いたとき）になると、それ以上思った方向に回転できなくなる問題です。
 *    - このコードでは、カメラの向きを「クォータニオン」という数学的な表現 (`orientation` メンバ変数) で管理しています。
 *    - クォータニオンは4つの数値 (x, y, z, w) で回転を表し、ジンバルロックの問題が発生しません。
 *    - `Update` 関数内では、マウスの動きやキー入力から、各軸（カメラ自身の上下左右、前後）周りの
 *      「どれだけ回転するか」という差分回転量を計算し、それをクォータニオン（`yawDelta`, `pitchDelta`, `rollDelta`）で表現します。
 *    - これらの差分回転クォータニオンを掛け合わせることで、1フレーム分の全体の回転 (`deltaRotation`) を合成します。
 *    - 最後に、現在のカメラの向き (`orientation`) に合成した回転 (`deltaRotation`) を適用することで、
 *      滑らかで問題の少ない回転を実現しています。
 *
 * 2. カメラの移動方法について:
 *    - 最初のバージョンでは、Wキーを押すと常にワールド座標系のZ軸方向に、Dキーを押すとX軸方向に移動していました。
 *      カメラがどの方向を向いていても移動方向が変わらないため、FPSゲームとしては不自然な操作感でした。
 *    - このコードでは、「カメラから見た方向」を基準に移動するように変更されています。
 *    - `Update` 関数内で、まず `GetForwardVector()`, `GetRightVector()`, `GetUpVector()` を呼び出して、
 *      現在のカメラの前方、右方、上方がワールド座標系でどちらを向いているかを示すベクトルを取得します。
 *    - キー入力（W/S, D/A, Space/LCtrl）に応じて、これらのローカル軸ベクトルに移動速度 (`MOVE_SPEED`) を掛けた
 *      移動量を計算します。
 *    - 計算された移動量を合成し（`lastWorldMoveOffset`）、カメラの現在位置 (`position`) に加算します。
 *    - これにより、Wキーを押せば常にカメラが向いている方向に前進する、直感的な操作が可能になります。
 *
 * 3. 座標変換と描画について:
 *    - 最初のバージョンでは、`GetScreenPos` 関数で、奥行き(Z)情報だけを使って簡単な比率計算で
 *      スクリーン上の位置を求めていました。これは簡易的な方法で、正確な遠近感や視野角は表現できませんでした。
 *    - このコードでは、3Dグラフィックスの標準的な手法である「座標変換パイプライン」を実装しています。
 *      - まず、`GetViewMatrix()` で「ビュー行列」を計算します。これは、ワールド空間にあるオブジェクトを、
 *        カメラの位置と向きを基準とした「カメラ視点空間（ビュー空間）」に変換するための行列です。
 *      - 次に、`GetProjectionMatrix()` で「プロジェクション行列」を計算します。これは、カメラ視点空間の
 *        オブジェクトを、最終的にスクリーンに表示するための「クリップ空間」と呼ばれる座標系に変換する行列です。
 *        この変換によって、遠近法（遠いものが小さく見える）や視野角（見える範囲）が適用されます。
 *      - `Draw` メソッドでは、描画したい線分のワールド座標に、まずビュー行列、次にプロジェクション行列を
 *        順番に掛ける（実際には事前に掛け合わせた `viewProjMatrix` を使う）ことで、クリップ座標に変換します。
 *      - クリップ座標は、後述するクリッピング処理や、最終的なスクリーン座標への変換に使われます。
 *      - この行列ベースの変換により、数学的に正確で、様々なカメラ設定（視野角など）に対応できる描画が可能になります。
 *
 * 4. クリッピングについて:
 *    - 最初のバージョンでは、オブジェクトがカメラより奥にあるかどうかのZ座標チェックのみでした。
 *      画面の左右上下にはみ出す場合や、遠すぎる場合の処理が不十分でした。
 *    - このコードでは、「Cohen-Sutherlandアルゴリズム」という手法 (`ClipLineCohenSutherland` 関数) を使って、
 *      線分がカメラの「視錐台（見える範囲を表す四角錐）」の内側にあるかどうかを判定し、
 *      はみ出した部分を正確に切り取る「クリッピング」処理を行っています。
 *    - 視錐台は、Near平面、Far平面、左、右、上、下の合計6つの平面で定義されます。
 *    - `ComputeOutCode` 関数は、点がこれらの平面のどちら側にあるかを判定し、領域を示すコードを返します。
 *    - `ClipLineCohenSutherland` は、線分の両端点のコードを使って、線分全体が視錐台の外にあるか（描画不要）、
 *      完全に内側にあるか（そのまま描画）、あるいは一部がはみ出しているか（クリッピング必要）を判断します。
 *    - クリッピングが必要な場合は、線分と視錐台の境界平面との交点を計算し、線分を短くする処理を繰り返します。
 *    - これにより、画面に表示されるべき部分だけが正確に描画されるようになります。
 *
 * [このコードに関する注意点]
 * - 回転の適用順序: `Update` 関数内の `orientation = deltaRotation * orientation;` という行は、
 *   数学的には「ワールド座標系において `deltaRotation` の回転を適用する」という意味になります。
 *   FPSゲームのように「プレイヤー自身の視点」を回転させる場合、通常は「ローカル座標系」で回転させたいので、
 *   `orientation = orientation * deltaRotation;` という順序でクォータニオンを掛ける方が、
 *   マウス操作に対する視点の動きがより直感的になることが多いです。このコードのままでも動作はしますが、
 *   操作感が期待と異なる場合は、この乗算順序を見直してみてください。
 * - ピッチ角制限: このコードには、カメラが上下方向（ピッチ）に回転できる角度を制限する機能がありません。
 *   （元の初期コードには `MIN_V`, `MAX_V` による制限がありました）。
 *   制限がないと、マウスを操作し続けるとカメラが真上や真下を通り越してしまい、天地が逆転するような
 *   不自然な動きになる可能性があります。通常は、累積ピッチ角を管理し、±89度程度の範囲内に収める処理を追加します。
 * - メンバ変数の依存: このコード（特に `Update` やデバッグ情報表示関数）は、`Camera.h` で多くのメンバ変数が
 *   宣言・初期化されていることを前提として書かれています（例: `fovY`, `lastMouseMoveX`, `currentForward` など）。
 *   もし `Camera.h` の内容が異なると、コンパイルエラーになったり、意図しない動作をする可能性があります。
 *
 */
#define NOMINMAX // Windows.h の min/max マクロ定義を無効化し、他のライブラリとの衝突を防ぐ
#include "Camera.h"     // Cameraクラスの宣言を読み込む
#include "DxLib.h"      // DxLibライブラリの関数を使うために必要
#include "CameraMath.h" // PerspectiveFovLHなどの数学関数 (自作ヘッダーと想定)
#include "Common.h"     // WINDOW_WIDTHなどの共通定数 (自作ヘッダーと想定)
#include <cmath>        // fabsf, sqrtf, sinf, cosf などの数学関数
#include <algorithm>    // std::min, std::max など (現在は未使用だが、将来使う可能性あり)
#include <stdexcept>    // 例外処理クラス (現在は未使用だが、エラー処理で使う可能性あり)
#include <string>       // std::string クラス (デバッグ情報用)
#include <sstream>      // std::stringstream クラス (デバッグ情報の文字列組み立て用)
#include <iomanip>      // std::setprecision など (デバッグ情報の書式設定用)
#include "Logger.h"     // LogDebugなどのログ出力関数 (自作ヘッダーと想定)
#include "Vector.h"     // Vector3D, Vector4D 構造体 (自作ヘッダーと想定)
#include "Matrix.h"     // Matrix 構造体, MatrixMultiply など (自作ヘッダーと想定)
#include "Quaternion.h" // Quaternion 構造体, FromAxisAngle など (自作ヘッダーと想定)

 // --- 匿名名前空間 ---
 // この .cpp ファイルの内部でのみ使用される関数や定数を定義する。
 // 他のファイルで同じ名前が使われていても、名前の衝突を防ぐことができる。
namespace {
    // 4次元ベクトル start から end へ、パラメータ t (0.0～1.0) を使って線形補間するインライン関数
    // クリッピングで交点を計算する際に使用する。w成分も補間することが重要。
    inline Vector4D VectorLerp4D(const Vector4D& start, const Vector4D& end, float t) {
        return {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t,
            start.z + (end.z - start.z) * t,
            start.w + (end.w - start.w) * t
        };
    }

    // Cohen-Sutherlandアルゴリズムで使用する領域コード（アウトコード）の定数
    const int INSIDE = 0;        // 000000: 完全に視錐台の内部
    const int LEFT = 1;          // 000001: 左境界の外側 (x < -w)
    const int RIGHT = 2;         // 000010: 右境界の外側 (x > w)
    const int BOTTOM = 4;        // 000100: 下境界の外側 (y < -w)
    const int TOP = 8;           // 001000: 上境界の外側 (y > w)
    const int OUTCODE_NEAR = 16; // 010000: Near平面より手前 (z < 0)
    const int OUTCODE_FAR = 32;  // 100000: Far平面より奥 (z > w)

    // クリップ座標系の点 p が、視錐台のどの領域にあるかを示すアウトコードを計算する関数
    // 元のコードのロジックを保持。w<=0 の場合の厳密なチェックは含まれていない点に注意。
    int ComputeOutCode(const Vector4D& p) {
        int code = INSIDE; // まず内部と仮定
        // 各境界との比較を行い、外側にあれば対応するビットを立てる
        if (p.x < -p.w) { code |= LEFT; }
        else if (p.x > p.w) { code |= RIGHT; }
        if (p.y < -p.w) { code |= BOTTOM; }
        else if (p.y > p.w) { code |= TOP; }
        if (p.z < 0.0f) { code |= OUTCODE_NEAR; }
        else if (p.z > p.w) { code |= OUTCODE_FAR; }
        return code; // 計算されたアウトコードを返す
    }

    // Cohen-Sutherlandアルゴリズムによる線分クリッピング関数
    // 引数: p1_clip, p2_clip (クリップ座標系の線分端点、クリップされると値が変更される)
    // 戻り値: 線分の一部でも視錐台内にあれば true、完全に外部なら false
    // 元のコードのロジックを保持
    bool ClipLineCohenSutherland(Vector4D& p1_clip, Vector4D& p2_clip) {
        int outcode1 = ComputeOutCode(p1_clip); // 始点のアウトコード
        int outcode2 = ComputeOutCode(p2_clip); // 終点のアウトコード
        const int MAX_ITERATIONS = 10; // 無限ループ防止のための最大反復回数
        int iterations = 0; // 現在の反復回数

        while (iterations < MAX_ITERATIONS) { // 最大反復回数に達するまでループ
            iterations++;
            // Case 1: 両端点が内部にある場合 (Trivial Accept)
            if ((outcode1 | outcode2) == 0) {
                return true; // 線分全体が表示されるので true
            }
            // Case 2: 両端点が同じ外部領域にある場合 (Trivial Reject)
            else if ((outcode1 & outcode2) != 0) {
                return false; // 線分全体が表示されないので false
            }
            // Case 3: クリッピングが必要な場合
            else {
                // 外部にある方の点のアウトコードを選択
                int outcode_outside = (outcode1 != 0) ? outcode1 : outcode2;
                float t = 0.0f; // 交点のパラメータ (0.0 <= t <= 1.0)
                Vector4D intersection_point; // 交点の座標
                // 線分の方向ベクトル成分
                float dx = p2_clip.x - p1_clip.x, dy = p2_clip.y - p1_clip.y;
                float dz = p2_clip.z - p1_clip.z, dw = p2_clip.w - p1_clip.w;
                float denominator; // 割り算の分母

                // 外部コードに対応する境界平面との交差パラメータ t を計算
                if (outcode_outside & LEFT) {         // 左平面 (x = -w)
                    denominator = dx + dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; } // 平行チェック
                    t = (-p1_clip.x - p1_clip.w) / denominator;
                }
                else if (outcode_outside & RIGHT) {   // 右平面 (x = w)
                    denominator = dx - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (p1_clip.w - p1_clip.x) / denominator;
                }
                else if (outcode_outside & BOTTOM) {  // 下平面 (y = -w)
                    denominator = dy + dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (-p1_clip.y - p1_clip.w) / denominator;
                }
                else if (outcode_outside & TOP) {     // 上平面 (y = w)
                    denominator = dy - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; }
                    t = (p1_clip.w - p1_clip.y) / denominator;
                }
                else if (outcode_outside & OUTCODE_NEAR) { // Near平面 (z = 0)
                    if (std::fabsf(dz) < 1e-6f) { return false; } // 平行チェック
                    t = -p1_clip.z / dz;
                }
                else if (outcode_outside & OUTCODE_FAR) {  // Far平面 (z = w)
                    denominator = dz - dw;
                    if (std::fabsf(denominator) < 1e-6f) { return false; } // 平行チェック
                    t = (p1_clip.w - p1_clip.z) / denominator;
                }
                else {
                    return false; // 通常は到達しない
                }

                // パラメータ t が線分上にない場合は棄却
                if (t < 0.0f || t > 1.0f) { return false; }

                // 交点の座標を線形補間で計算
                intersection_point = VectorLerp4D(p1_clip, p2_clip, t);

                // 外部にあった点を交点に置き換え、その点のアウトコードを再計算
                if (outcode_outside == outcode1) {
                    p1_clip = intersection_point; // 始点を更新
                    outcode1 = ComputeOutCode(p1_clip); // 始点のアウトコードを再計算
                }
                else {
                    p2_clip = intersection_point; // 終点を更新
                    outcode2 = ComputeOutCode(p2_clip); // 終点のアウトコードを再計算
                }
                // ループの最初に戻り、再度判定を行う
            }
        }
        // 最大反復回数に達した場合 (通常は起こらないが、念のため)
        return false;
    }

    // カメラ制御用の定数
    static const float MOVE_SPEED = 2.5f;   // 移動速度 (単位/フレーム or 秒)
    // static const float ROTATION_SENSITIVITY = 5.0f; // この定数は MOUSE_ANGLE_RATE の計算に使われていない
    static const float MOUSE_ANGLE_RATE = ONE_DEGREE * 0.1f; // マウス感度 (1ピクセル移動あたり0.1度回転)
    static const float ROLL_SPEED = 2.0f * ONE_DEGREE; // ロール回転速度 (1フレームあたり2度回転)

    // --- Camera クラスに必要なメンバ変数の仮宣言 (コメント) ---
    // float fovY; float aspectRatio; float nearZ; float farZ; // プロジェクション用
    // int lastMouseMoveX, lastMouseMoveY; // マウス入力記録用
    // float lastYawAngle, lastPitchAngle, lastRollAngle; // 回転角度記録用
    // Vector3D currentForward, currentRight, currentUp; // 現在の軸ベクトル記録用
    // float lastMoveForward, lastMoveRight, lastMoveUp; // 移動入力記録用
    // Vector3D lastWorldMoveOffset; // 移動オフセット記録用

} // 匿名名前空間の終わり

// --- Camera クラス メンバー関数の実装 ---

// コンストラクタ: Cameraオブジェクト生成時に呼び出される初期化処理
Camera::Camera() : position{ 0.0f, 0.0f, -50.0f }, orientation() {
    // メンバ変数の初期化リストで position と orientation を初期化
    // position は (0, 0, -50) に設定
    // orientation はデフォルトコンストラクタで単位クォータニオン (回転なし) に初期化される

    // マウスカーソルを画面中央に設定し、最初のフレームでの予期せぬ視点移動を防ぐ
    SetMousePoint(static_cast<int>(WINDOW_WIDTH / 2), static_cast<int>(WINDOW_HEIGHT / 2));

    // 注意: Camera.h で宣言されている他のメンバ変数 (fovY, aspectRatio, nearZ, farZ など) は
    //       ここで初期化されていない場合、Camera.h でのデフォルト初期化に依存します。
}

// デストラクタ: Cameraオブジェクトが破棄されるときに呼び出される (今回は特に処理なし)
Camera::~Camera() {}

// カメラの現在のワールド座標を返す Getter 関数
Vector3D Camera::GetPosition() const {
    return position; // メンバ変数 position の値を返す
}

// 現在のカメラの状態からビュー行列 (View Matrix) を計算して返す Getter 関数
// ビュー行列は、ワールド座標系の点をカメラ視点の座標系（ビュー座標系）に変換する
Matrix Camera::GetViewMatrix() const {
    // 1. カメラの回転の逆を表すクォータニオンを計算 (共役を取る)
    Quaternion invOrientation = orientation.Conjugate();
    // 2. 逆回転クォータニオンを回転行列に変換
    Matrix invRotationMatrix = invOrientation.ToRotationMatrix();
    // 3. カメラの位置を打ち消す逆平行移動行列を計算
    Matrix invTranslate = Matrix::Identity(); // まず単位行列を作成
    invTranslate.m[3][0] = -position.x; // 平行移動成分を設定
    invTranslate.m[3][1] = -position.y;
    invTranslate.m[3][2] = -position.z;
    // 4. ビュー行列 = 逆平行移動行列 * 逆回転行列 (DirectX スタイル)
    Matrix viewMatrix = MatrixMultiply(invTranslate, invRotationMatrix);
    return viewMatrix; // 計算したビュー行列を返す
}

// 現在のカメラの設定からプロジェクション行列 (Projection Matrix) を計算して返す Getter 関数
// プロジェクション行列は、ビュー座標系の点をクリップ座標系に変換し、遠近感を適用する
Matrix Camera::GetProjectionMatrix() const {
    // Camera.h で定義されているメンバ変数 fovY, aspectRatio, nearZ, farZ を使って
    // PerspectiveFovLH 関数 (CameraMath.h で定義想定) を呼び出し、透視投影行列を計算する
    // return PerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);

    // 仮の実装 (もしメンバ変数が Camera.h で初期化されていない場合)
    // これらの値が Camera.h で正しく設定されていることを確認してください。
    return PerspectiveFovLH(60.0f * ONE_DEGREE, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);
}

// カメラの前方ベクトル (+Z方向) の現在のワールド空間での向きを計算して返す Getter 関数
Vector3D Camera::GetForwardVector() const {
    // 現在のカメラの向きを表すクォータニオンを行列に変換
    Matrix rotMat = orientation.ToRotationMatrix();
    // ローカル座標系の前方ベクトル (0, 0, 1) を回転行列で変換
    // VEC3TransformNormal を使い、結果を Normalize() するのがより正確
    return VEC3Transform({ 0.0f, 0.0f, 1.0f }, rotMat);
}

// カメラの右方ベクトル (+X方向) の現在のワールド空間での向きを計算して返す Getter 関数
Vector3D Camera::GetRightVector() const {
    Matrix rotMat = orientation.ToRotationMatrix();
    // ローカル座標系の右方ベクトル (1, 0, 0) を回転行列で変換
    return VEC3Transform({ 1.0f, 0.0f, 0.0f }, rotMat);
}

// カメラの上方ベクトル (+Y方向) の現在のワールド空間での向きを計算して返す Getter 関数
Vector3D Camera::GetUpVector() const {
    Matrix rotMat = orientation.ToRotationMatrix();
    // ローカル座標系の上方ベクトル (0, 1, 0) を回転行列で変換
    return VEC3Transform({ 0.0f, 1.0f, 0.0f }, rotMat);
}

// ワールド座標をスクリーン座標に変換するヘルパー関数 (これはCameraクラスのメンバではないグローバル関数)
// viewProjMatrix: 事前に計算されたビュー行列とプロジェクション行列の積
// 戻り値: スクリーン座標 (x, y) と NDCのz値 (深度) を格納した Vector3D。画面外なら (-1, -1, 0)。
Vector3D WorldToScreen(const Vector3D& worldPos, const Matrix& viewProjMatrix) {
    // 1. ワールド座標 -> クリップ座標 (同次座標) へ変換
    Vector4D clipPos = VEC4Transform({ worldPos.x, worldPos.y, worldPos.z, 1.0f }, viewProjMatrix);

    // 2. 視錐台内外の簡易チェック (パースペクティブ除算前)
    if (std::fabsf(clipPos.w) < 1e-6f || clipPos.z < 0.0f || clipPos.z > clipPos.w) {
        return { -1.0f, -1.0f, 0.0f }; // 範囲外 (または視点の後ろ)
    }

    // 3. パースペクティブ除算 (クリップ座標 -> 正規化デバイス座標 NDC)
    Vector3D ndc = { clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w };

    // 4. NDC -> スクリーン座標 へ変換
    float hW = WINDOW_WIDTH / 2.0f; // 画面幅の半分
    float hH = WINDOW_HEIGHT / 2.0f; // 画面高さの半分
    // NDC[-1, 1] を Screen[0, Width/Height] にマッピング。Y軸は反転。
    // 元のコードに合わせて int にキャストしているが、float のまま返す方が精度は良い。
    int sx = static_cast<int>(ndc.x * hW + hW);
    int sy = static_cast<int>(-ndc.y * hH + hH); // Y軸反転

    // 結果を Vector3D で返す (スクリーン座標 x, y と NDC の z値)
    return { (float)sx, (float)sy, ndc.z };
}

// ワールド空間の線分 (`worldLines`) をカメラ視点で描画するメソッド
void Camera::Draw(const std::vector<std::vector<Vector3D>>& worldLines) {
    // 必要な行列を事前に計算
    Matrix viewMatrix = GetViewMatrix();
    Matrix projMatrix = GetProjectionMatrix();
    Matrix viewProjMatrix = MatrixMultiply(viewMatrix, projMatrix); // ビュー * プロジェクション

    // 受け取った全ての線分についてループ処理
    for (const auto& line : worldLines) {
        // 線分は2点以上必要
        if (line.size() < 2) { continue; }

        // 線分の始点と終点をワールド座標からクリップ座標へ変換
        Vector4D p1_clip = VEC4Transform({ line[0].x, line[0].y, line[0].z, 1.0f }, viewProjMatrix);
        Vector4D p2_clip = VEC4Transform({ line[1].x, line[1].y, line[1].z, 1.0f }, viewProjMatrix);

        // クリッピング処理のために座標をコピー
        Vector4D p1_clipped = p1_clip;
        Vector4D p2_clipped = p2_clip;

        // Cohen-Sutherlandアルゴリズムで線分をクリッピング
        if (ClipLineCohenSutherland(p1_clipped, p2_clipped)) {
            // 線分の一部でも視錐台内に残った場合...

            // NDC座標とスクリーン座標を計算 (元のコードのロジックを保持)
            Vector3D p1_ndc, p2_ndc; // NDC座標用
            // パースペクティブ除算の前に w 成分がゼロに近くないかチェック
            if (fabsf(p1_clipped.w) > 1e-6f && fabsf(p2_clipped.w) > 1e-6f) {
                // NDC座標を計算 (x/w, y/w, z/w)
                p1_ndc = { p1_clipped.x / p1_clipped.w, p1_clipped.y / p1_clipped.w, p1_clipped.z / p1_clipped.w };
                p2_ndc = { p2_clipped.x / p2_clipped.w, p2_clipped.y / p2_clipped.w, p2_clipped.z / p2_clipped.w };

                // NDC座標をスクリーン座標 (int) に変換
                float hW = WINDOW_WIDTH / 2.f, hH = WINDOW_HEIGHT / 2.f;
                int sx1 = static_cast<int>(p1_ndc.x * hW + hW);
                int sy1 = static_cast<int>(-p1_ndc.y * hH + hH); // Y軸反転
                int sx2 = static_cast<int>(p2_ndc.x * hW + hW);
                int sy2 = static_cast<int>(-p2_ndc.y * hH + hH); // Y軸反転

                // DxLibの関数で線を描画
                DrawLine(sx1, sy1, sx2, sy2, GetColor(255, 255, 255)); // 白色
            }
        }
        // else: 線分が完全に視錐台の外なら何もしない
    }

    // デバッグ用: 前回の移動方向を示す線を描画 (メンバ変数 lastWorldMoveOffset が必要)
    // if (lastWorldMoveOffset.LengthSq() > 1e-12f) { // 前回のフレームで移動があった場合
    //     Vector3D moveDir = lastWorldMoveOffset.Normalized(); // 移動方向ベクトル
    //     Vector3D moveEndPos = position + moveDir * 10.0f; // 現在位置から少し伸ばした点
    //     // 移動線の始点と終点をスクリーン座標に変換
    //     Vector3D startScreen = WorldToScreen(position, viewProjMatrix);
    //     Vector3D endScreen = WorldToScreen(moveEndPos, viewProjMatrix);
    //     // 両端が画面内なら線を描画
    //     if (startScreen.x >= 0.0f && endScreen.x >= 0.0f) {
    //         DrawLine(static_cast<int>(startScreen.x), static_cast<int>(startScreen.y),
    //                  static_cast<int>(endScreen.x), static_cast<int>(endScreen.y),
    //                  GetColor(0, 255, 255)); // 水色
    //     }
    // }
} // Draw 関数の終わり


// カメラの状態を更新するメソッド (毎フレーム呼び出される)
void Camera::Update() {
    // --- 1. マウスによる視点回転量の計算 ---
    int currentMouseX = 0, currentMouseY = 0;
    GetMousePoint(&currentMouseX, &currentMouseY); // 現在のマウス座標を取得
    const int centerX = static_cast<int>(WINDOW_WIDTH / 2); // 画面中央 X
    const int centerY = static_cast<int>(WINDOW_HEIGHT / 2); // 画面中央 Y
    int mouseMoveX = currentMouseX - centerX; // 前フレームからのX移動量
    int mouseMoveY = currentMouseY - centerY; // 前フレームからのY移動量
    // 移動量をメンバ変数に保存 (デバッグ情報表示用)
    lastMouseMoveX = mouseMoveX;
    lastMouseMoveY = mouseMoveY;

    // マウス移動量から、このフレームでの回転角度（ラジアン単位）を計算
    // 回転量はメンバ変数に保存 (デバッグ情報表示用)
    lastYawAngle = static_cast<float>(mouseMoveX) * MOUSE_ANGLE_RATE;   // ヨー角 (左右回転、カメラの上軸周り)
    lastPitchAngle = static_cast<float>(mouseMoveY) * MOUSE_ANGLE_RATE; // ピッチ角 (上下回転、カメラの右軸周り)

    // --- 2. キーボードによるロール回転量の計算 ---
    // ロール角 (傾き回転、カメラの前方軸周り)
    lastRollAngle = 0.0f; // フレーム開始時にリセット
    if (CheckHitKey(KEY_INPUT_E)) { lastRollAngle += ROLL_SPEED; } // Eキーで右回りロール
    if (CheckHitKey(KEY_INPUT_Q)) { lastRollAngle -= ROLL_SPEED; } // Qキーで左回りロール

    // --- 3. 回転の適用 ---
    // まず、現在のカメラのローカル座標軸ベクトルを取得する
    // これらのベクトルは、この後の回転計算の軸として使われる
    // 計算結果はメンバ変数に保存 (デバッグ情報表示用、および移動計算用)
    currentUp = GetUpVector();
    currentRight = GetRightVector();
    currentForward = GetForwardVector();

    // 各回転軸と角度から、このフレームでの差分回転を表すクォータニオンを生成
    Quaternion yawDelta = Quaternion::FromAxisAngle(currentUp, lastYawAngle);
    Quaternion pitchDelta = Quaternion::FromAxisAngle(currentRight, lastPitchAngle);
    Quaternion rollDelta = Quaternion::FromAxisAngle(currentForward, lastRollAngle);

    // ３つの差分回転クォータニオンを合成して、このフレーム全体の回転を表すクォータニオンを計算
    // 合成順序は Roll -> Pitch -> Yaw (ローカル軸周りの回転順序)
    Quaternion deltaRotation = rollDelta * pitchDelta * yawDelta;

    // 現在のカメラの向き (`orientation`) に、計算した差分回転 (`deltaRotation`) を適用する
    // 注意: `deltaRotation * orientation` はワールド座標系での回転適用。
    //       FPS視点操作としては `orientation * deltaRotation` (ローカル座標系での適用) が一般的。
    orientation = deltaRotation * orientation;
    // クォータニオンは計算誤差で長さが1からずれる可能性があるので、正規化して長さを1に保つ
    orientation.Normalize();

    // ピッチ角制限は削除 // 元のコードにあったコメント。このコードにはピッチ制限処理自体がない。

    // --- 4. カメラの向きベクトル再計算 ---
    // 回転が適用された後の、最新のローカル軸ベクトルを計算し、メンバ変数に保存する
    // これは次の移動計算で使われる。
    currentForward = GetForwardVector();
    currentRight = GetRightVector();
    currentUp = GetUpVector();

    // --- 5. キーボードによる移動入力の取得 ---
    // 各方向への移動入力状態 (-1.0, 0.0, or 1.0) をメンバ変数に保存 (デバッグ情報表示用)
    lastMoveForward = 0.0f; // 前後移動用フラグをリセット
    lastMoveRight = 0.0f;   // 左右移動用フラグをリセット
    lastMoveUp = 0.0f;      // 上下移動用フラグをリセット
    // 対応するキーが押されていたらフラグを更新
    if (CheckHitKey(KEY_INPUT_W)) { lastMoveForward += 1.0f; } // 前進
    if (CheckHitKey(KEY_INPUT_S)) { lastMoveForward -= 1.0f; } // 後退
    if (CheckHitKey(KEY_INPUT_D)) { lastMoveRight += 1.0f; }   // 右移動
    if (CheckHitKey(KEY_INPUT_A)) { lastMoveRight -= 1.0f; }   // 左移動
    if (CheckHitKey(KEY_INPUT_SPACE)) { lastMoveUp += 1.0f; }    // 上昇
    if (CheckHitKey(KEY_INPUT_LCONTROL)) { lastMoveUp -= 1.0f; } // 下降

    // --- 6. 移動量の計算とカメラ位置の更新 ---
    // このフレームで実際に移動するワールド空間でのベクトル (`lastWorldMoveOffset`) を計算
    lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f }; // 移動オフセットをリセット
    // 各ローカル軸の方向に入力フラグと移動速度を掛けて、移動ベクトルを合成
    lastWorldMoveOffset += currentForward * lastMoveForward * MOVE_SPEED; // 前後方向の移動量
    lastWorldMoveOffset += currentRight * lastMoveRight * MOVE_SPEED;   // 左右方向の移動量
    lastWorldMoveOffset += currentUp * lastMoveUp * MOVE_SPEED;       // 上下方向の移動量
    // 計算された移動オフセットを、現在のカメラの位置 `position` に加算して、位置を更新
    position += lastWorldMoveOffset;

    // --- 7. マウスカーソルを画面中央に戻す ---
    // これにより、次のフレームで再び中央からの相対移動量を取得できる（相対マウスモード）
    SetMousePoint(centerX, centerY);

    // --- デバッグログ出力 ---
    // 現在のフレームの詳細なカメラ情報をログファイルに出力する
    LogDebug(GetDetailedDebugInfo());
} // Update 関数の終わり

// デバッグ情報（画面表示用）を文字列として返す関数
std::string Camera::GetDebugInfo() const {
    // 元のコードの通り。対応するメンバ変数(lastMouseMoveX/Y, lastMove*)が必要。
    std::stringstream ss; // 文字列ストリームを使って文字列を組み立てる
    ss << std::fixed << std::setprecision(2); // 小数点以下2桁で表示
    // 位置座標を追加
    ss << "Pos:(" << position.x << "," << position.y << "," << position.z << ") ";
    // マウス移動量を追加
    ss << "Mouse:(" << lastMouseMoveX << "," << lastMouseMoveY << ") ";
    // ロールキー入力状態を確認
    float rollInput = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) { rollInput += 1.0f; }
    if (CheckHitKey(KEY_INPUT_Q)) { rollInput -= 1.0f; }
    // 移動キー入力状態とロール入力状態を追加
    ss << "Key: F" << lastMoveForward << " R" << lastMoveRight << " U" << lastMoveUp << " Roll" << rollInput;
    // 組み立てた文字列を返す
    return ss.str();
} // GetDebugInfo 関数の終わり

// デバッグ情報（ログファイル用）を文字列として返す関数
std::string Camera::GetDetailedDebugInfo() const {
    // 元のコードの通り。対応するメンバ変数(last*, current*)が必要。
    std::stringstream ss; // 文字列ストリーム
    ss << std::fixed << std::setprecision(4); // 小数点以下4桁で表示
    // 位置座標
    ss << "Pos(x:" << position.x << ",y:" << position.y << ",z:" << position.z << ") ";
    // クォータニオンの各成分
    ss << "Ori(x:" << orientation.x << ",y:" << orientation.y << ",z:" << orientation.z << ",w:" << orientation.w << ") ";
    // マウス入力
    ss << "MouseIn(x:" << lastMouseMoveX << ",y:" << lastMouseMoveY << ") ";
    // 計算された回転角度 (度数法に変換して表示)
    ss << "RotAngle(Yaw:" << lastYawAngle * 180.0f / PI << ",Pitch:" << lastPitchAngle * 180.0f / PI << ",Roll:" << lastRollAngle * 180.0f / PI << ") ";
    // ロールキー入力
    float rollInput = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) { rollInput += 1.0f; }
    if (CheckHitKey(KEY_INPUT_Q)) { rollInput -= 1.0f; }
    // 移動キー入力
    ss << "KeyIn(F:" << lastMoveForward << ",R:" << lastMoveRight << ",U:" << lastMoveUp << ",Roll:" << rollInput << ") ";
    // 現在のローカル軸ベクトル
    ss << "AxisF(x:" << currentForward.x << ",y:" << currentForward.y << ",z:" << currentForward.z << ") ";
    ss << "AxisR(x:" << currentRight.x << ",y:" << currentRight.y << ",z:" << currentRight.z << ") ";
    ss << "AxisU(x:" << currentUp.x << ",y:" << currentUp.y << ",z:" << currentUp.z << ") ";
    // 計算された移動オフセットベクトル
    ss << "MoveOffset(x:" << lastWorldMoveOffset.x << ",y:" << lastWorldMoveOffset.y << ",z:" << lastWorldMoveOffset.z << ")";
    // 組み立てた文字列を返す
    return ss.str();
} // GetDetailedDebugInfo 関数の終わり