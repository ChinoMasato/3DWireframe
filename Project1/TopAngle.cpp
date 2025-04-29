#include "TopAngle.h" // 対応するヘッダーファイル
#include "Camera.h"   // Cameraクラスの定義を参照するため (GetPosition, GetForwardVectorなどを使う)
#include "DxLib.h"    // DxLibの描画関数を使うため
#include "Common.h"   // 定数(PI, ONE_DEGREE, WINDOW_WIDTH, WINDOW_HEIGHT) を使うため
#include <cmath>      // sinf, cosf, atan2f などの数学関数を使うため (<math.h> より <cmath> が C++ では推奨)
#include <vector>     // std::vector (Drawの引数型)

/*
 * TopAngle.cpp (コメント修正版 - 元コード保持)
 * 概要:
 *   TopAngleクラスの具体的な処理を実装するファイルです。
 *   画面左上に表示するトップダウンビュー（真上からの視点）の描画ロジックを担当します。
 *   主な機能は以下の通りです。
 *   - コンストラクタでの初期化（監視対象のCameraオブジェクトを受け取る）
 *   - ワールド空間の座標を、トップダウンビュー表示領域内の2Dスクリーン座標に変換する
 *   - 毎フレーム、ビュー領域の背景、メインカメラの位置と向き、視野角、
 *     そしてワールド内のオブジェクト（線分）を描画する
 *
 * 元のコードからの主な変更点:
 *   - カメラの向きを取得する方法が、新しい `Camera` クラスに合わせて変更されています。
 *     元のコードでは直接回転角度を取得していたと思われますが、新しいカメラでは
 *     クォータニオンで向きを管理しているため、代わりに `GetForwardVector()` で
 *     前方ベクトルを取得し、そのXZ成分から `atan2f` を使って上から見たときの
 *     向きの角度（ヨー角）を計算しています。
 *
 * このファイル内の処理の流れ:
 * 1. 定数定義: トップダウンビューの表示位置、サイズ、スケールなどの設定値を定義します。
 * 2. コンストラクタ: `Camera` オブジェクトへのポインタを受け取り、保持します。
 * 3. `ConvertWorldToView`: ワールド座標(X, Z)を受け取り、カメラからの相対位置と
 *    スケールに基づいて、ビュー領域内のスクリーン座標(X, Y)に変換します。
 * 4. `Draw`:
 *    - 背景と枠線を描画します。
 *    - ビュー領域の中心にメインカメラを表す円を描画します。
 *    - `GetForwardVector` と `atan2f` を使ってカメラの向き角度を計算し、
 *      その方向に線を描画します。
 *    - 設定された視野角 (`CAMERA_FOV_H`) に基づいて、視野範囲を示す線を描画します。
 *    - `worldLines` で渡された各線分を `ConvertWorldToView` で変換し、
 *      ビュー領域内に線として描画します (`SetDrawArea` で描画範囲を制限)。
 */

 // --- トップダウンビューの表示設定値の定義 ---
 // TopAngle.h で宣言された静的定数メンバーの値をここで定義します。
 // (注: C++17以降ではインライン変数としてヘッダー内で定義することも可能です)
const int TopAngle::VIEW_POS_X = 10;      // ビュー領域の左上のX座標 (画面左から10ピクセル)
const int TopAngle::VIEW_POS_Y = 10;      // ビュー領域の左上のY座標 (画面上から10ピクセル)
const int TopAngle::VIEW_WIDTH = 200;     // ビュー領域の幅 (200ピクセル)
const int TopAngle::VIEW_HEIGHT = 200;    // ビュー領域の高さ (200ピクセル)
const float TopAngle::VIEW_SCALE = 1.5f;  // 描画スケール (ワールドの1単位を1.5ピクセルで表示)
const float TopAngle::CAMERA_FOV_H = 75.0f * ONE_DEGREE; // カメラの水平視野角 (約75度) - 視野角表示に使用
const float TopAngle::VIEW_RANGE = 60.0f; // 視野角やカメラの向きを示す線の長さ (60ピクセル)

// コンストラクタ:
//   メインカメラへのポインタを受け取り、メンバ変数 `camera` に保存します。
TopAngle::TopAngle(Camera* cam) : camera(cam)
{
    // 念のため、渡されたポインタが nullptr (無効なポインタ) でないかチェックします。
    if (camera == nullptr) {
        // もし nullptr なら、DxLib のデバッグ表示機能を使って警告メッセージを表示します。
        printfDx("警告: TopAngle に渡されたカメラポインタが nullptr です。\n");
        // この場合、以降の Draw 関数などで `camera` を参照するとエラーになる可能性があります。
    }
}

// デストラクタ:
//   今回は特に後片付け処理は必要ありません。
TopAngle::~TopAngle() {}

// ヘルパー関数: ワールド座標(X, Z)をトップダウンビューのスクリーン座標に変換する
Vector3D TopAngle::ConvertWorldToView(float worldX, float worldZ) const
{
    // カメラポインタが無効なら、デフォルト座標（ビュー左上）を返すなどしてエラーを防ぐ
    if (!camera) {
        // 定数を使うように修正 (以前の回答でヘッダーに constexpr で定義した場合)
        // return { (float)TopAngle::VIEW_POS_X, (float)TopAngle::VIEW_POS_Y, 0.0f };
        // このファイルで定義している定数をそのまま使う場合:
        return { (float)VIEW_POS_X, (float)VIEW_POS_Y, 0.0f };
    }

    // 1. メインカメラの現在のワールド座標を取得
    Vector3D camPos = camera->GetPosition();

    // 2. 描画したい点の、カメラからの相対座標 (XZ平面上) を計算
    float relativeX = worldX - camPos.x;
    float relativeZ = worldZ - camPos.z; // ワールドZ座標がビューのY座標に対応する

    // 3. ビュー領域の中心のスクリーン座標を計算
    float viewCenterX = (float)(VIEW_POS_X + VIEW_WIDTH / 2);
    float viewCenterY = (float)(VIEW_POS_Y + VIEW_HEIGHT / 2);

    // 4. 相対座標にスケールを掛けて、ビュー中心からのオフセットを計算し、最終的なスクリーン座標を求める
    float viewX = viewCenterX + relativeX * VIEW_SCALE;
    float viewY = viewCenterY + relativeZ * VIEW_SCALE; // Z+ を Y+ (下向き) にマッピング

    // 結果を Vector3D で返す (Z成分は使わない)
    return { viewX, viewY, 0.0f };
}


// トップダウンビューの描画関数 (毎フレーム呼び出される)
void TopAngle::Draw(const std::vector<std::vector<Vector3D>>& worldLines)
{
    // カメラポインタが無効なら、何も描画せずに終了
    if (!camera) {
        return;
    }

    // --- 1. ビュー領域の背景と枠線の描画 ---
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明モード設定
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(0, 0, 0), TRUE); // 背景描画
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // 通常モードに戻す
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(255, 255, 255), FALSE); // 枠線描画

    // --- 2. カメラ自身の描画 ---
    int camViewX = VIEW_POS_X + VIEW_WIDTH / 2; // ビュー中心X
    int camViewY = VIEW_POS_Y + VIEW_HEIGHT / 2; // ビュー中心Y
    unsigned int camColor = GetColor(255, 0, 0); // カメラの色(赤)
    DrawCircle(camViewX, camViewY, 4, camColor, TRUE); // カメラ位置を円で表示

    // --- 3. カメラの向きと視野角の描画 ---
    // カメラの前方ベクトル取得
    Vector3D forward = camera->GetForwardVector();
    // XZ平面上での向き角度(ヨー角)をatan2f(x, z)で計算 (Z軸前方基準)
    float camRotY = atan2f(forward.x, forward.z);

    // カメラの向きを示す線のオフセット計算
    float forwardXOffset = sinf(camRotY) * VIEW_RANGE;
    float forwardYOffset = cosf(camRotY) * VIEW_RANGE;

    // カメラの向きを示す線を描画
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(forwardXOffset), camViewY + static_cast<int>(forwardYOffset), camColor);

    // --- 視野角表示 ---
    float halfFovH = CAMERA_FOV_H / 2.0f; // 水平視野角の半分
    float leftFovRot = camRotY - halfFovH; // 左端の角度
    float rightFovRot = camRotY + halfFovH; // 右端の角度
    // 左右の線のオフセット計算
    float leftXOffset = sinf(leftFovRot) * VIEW_RANGE;
    float leftYOffset = cosf(leftFovRot) * VIEW_RANGE;
    float rightXOffset = sinf(rightFovRot) * VIEW_RANGE;
    float rightYOffset = cosf(rightFovRot) * VIEW_RANGE;

    // 視野角を示す線を描画
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(leftXOffset), camViewY + static_cast<int>(leftYOffset), camColor); // 左線
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(rightXOffset), camViewY + static_cast<int>(rightYOffset), camColor); // 右線
    DrawLine(camViewX + static_cast<int>(leftXOffset), camViewY + static_cast<int>(leftYOffset), // 先端線
        camViewX + static_cast<int>(rightXOffset), camViewY + static_cast<int>(rightYOffset), camColor);


    // --- 4. ワールドオブジェクト (線分) の描画 ---
    unsigned int objectColor = GetColor(0, 255, 0); // オブジェクトの色(緑)

    // 描画範囲をトップダウンビュー領域内に限定
    SetDrawArea(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT);

    // ワールドの線分を描画
    for (const auto& line : worldLines)
    {
        if (line.size() < 2) { continue; }
        // 線分の両端をビュー座標に変換
        Vector3D viewP1 = ConvertWorldToView(line[0].x, line[0].z);
        Vector3D viewP2 = ConvertWorldToView(line[1].x, line[1].z);
        // 線を描画 (アンチエイリアス付き)
        DrawLineAA(viewP1.x, viewP1.y, viewP2.x, viewP2.y, objectColor);
    }

    // 描画範囲の限定を解除
    SetDrawArea(0, 0, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT));
}