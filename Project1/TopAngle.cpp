#include "TopAngle.h"
#include "Camera.h"   // Cameraクラスの定義を参照するため (GetPosition, GetForwardVectorなど)
#include "DxLib.h"    // DxLibの描画関数を使うため
#include "Common.h"   // 定数(PI, ONE_DEGREE, WINDOW_WIDTH, WINDOW_HEIGHT) を使うため
#include <math.h>     // sinf, cosf, atan2 を使うため
#include <vector>     // std::vector (Drawの引数型)

/*
 * TopAngle.cpp
 * トップダウンビュー描画クラスの実装。
 * 変更点:
 * - カメラの向き取得を GetRotation() から GetForwardVector() を使った計算に変更。
 */

 // --- トップダウンビューの表示設定値の定義 ---
const int TopAngle::VIEW_POS_X = 10;
const int TopAngle::VIEW_POS_Y = 10;
const int TopAngle::VIEW_WIDTH = 200;
const int TopAngle::VIEW_HEIGHT = 200;
const float TopAngle::VIEW_SCALE = 1.5f;
const float TopAngle::CAMERA_FOV_H = 75.0f * ONE_DEGREE;
const float TopAngle::VIEW_RANGE = 60.0f;

TopAngle::TopAngle(Camera* cam) : camera(cam)
{
    if (camera == nullptr) {
        printfDx("警告: TopAngle に渡されたカメラポインタが nullptr です。\n");
    }
}

TopAngle::~TopAngle() {}

// ヘルパー関数: ワールド座標(X, Z)をトップダウンビュー座標に変換
Vector3D TopAngle::ConvertWorldToView(float worldX, float worldZ) const
{
    if (!camera) return { (float)VIEW_POS_X, (float)VIEW_POS_Y, 0.0f };
    Vector3D camPos = camera->GetPosition();
    float relativeX = worldX - camPos.x;
    float relativeZ = worldZ - camPos.z;
    float viewCenterX = (float)(VIEW_POS_X + VIEW_WIDTH / 2);
    float viewCenterY = (float)(VIEW_POS_Y + VIEW_HEIGHT / 2);
    float viewX = viewCenterX + relativeX * VIEW_SCALE;
    float viewY = viewCenterY + relativeZ * VIEW_SCALE; // Z+がY+ (下)
    return { viewX, viewY, 0.0f };
}


// トップダウンビューの描画関数
void TopAngle::Draw(const std::vector<std::vector<Vector3D>>& worldLines)
{
    if (!camera) return;

    // --- 1. ビュー領域の背景と枠線の描画 ---
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(255, 255, 255), FALSE);

    // --- 2. カメラの描画 ---
    int camViewX = VIEW_POS_X + VIEW_WIDTH / 2;
    int camViewY = VIEW_POS_Y + VIEW_HEIGHT / 2;
    unsigned int camColor = GetColor(255, 0, 0);
    DrawCircle(camViewX, camViewY, 4, camColor, TRUE);

    // --- 3. カメラの向きと視野角の描画 ---
    // !! 修正: クォータニオンからヨー角を取得 !!
    // GetRotation() の代わりに前方ベクトルからXZ平面上の向きを計算
    Vector3D forward = camera->GetForwardVector();
    // XZ平面上での角度 (ヨー角) を atan2 で計算 (Z軸前方基準、時計回り)
    float camRotY = atan2f(forward.x, forward.z); // atan2(x, z) で Z軸からの角度

    // カメラの前方ベクトル (ビュー座標系でのオフセット)
    float forwardXOffset = sinf(camRotY) * VIEW_RANGE;
    float forwardYOffset = cosf(camRotY) * VIEW_RANGE;

    // カメラの向きを示す線
    DrawLine(camViewX, camViewY, camViewX + (int)forwardXOffset, camViewY + (int)forwardYOffset, camColor);

    // 視野角の左右の線の角度
    float leftFovRot = camRotY - CAMERA_FOV_H / 2.0f;
    float rightFovRot = camRotY + CAMERA_FOV_H / 2.0f;
    float leftXOffset = sinf(leftFovRot) * VIEW_RANGE;
    float leftYOffset = cosf(leftFovRot) * VIEW_RANGE;
    float rightXOffset = sinf(rightFovRot) * VIEW_RANGE;
    float rightYOffset = cosf(rightFovRot) * VIEW_RANGE;

    // 視野角を示す線
    DrawLine(camViewX, camViewY, camViewX + (int)leftXOffset, camViewY + (int)leftYOffset, camColor);
    DrawLine(camViewX, camViewY, camViewX + (int)rightXOffset, camViewY + (int)rightYOffset, camColor);
    DrawLine(camViewX + (int)leftXOffset, camViewY + (int)leftYOffset, camViewX + (int)rightXOffset, camViewY + (int)rightYOffset, camColor);


    // --- 4. オブジェクト (線分) の描画 ---
    unsigned int objectColor = GetColor(0, 255, 0);
    SetDrawArea(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT);
    for (const auto& line : worldLines)
    {
        if (line.size() < 2) continue;
        Vector3D viewP1 = ConvertWorldToView(line[0].x, line[0].z);
        Vector3D viewP2 = ConvertWorldToView(line[1].x, line[1].z);
        DrawLineAA(viewP1.x, viewP1.y, viewP2.x, viewP2.y, objectColor);
    }
    SetDrawArea(0, 0, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT));
}