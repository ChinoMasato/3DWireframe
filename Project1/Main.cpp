// !! NOMINMAX はファイルの先頭 !!
#define NOMINMAX
#include "Common.h"
#include "DxLib.h"
#include "Camera.h"
#include "TopAngle.h"   // インクルードは残すが、使用しない
#include "Vector.h"
#include <vector>
#include <string>
#include "Logger.h" // ロガークラスをインクルード
#include <math.h>  // sinf, cosf を使用するため

/*
 * Main.cpp
 * 役割: アプリケーションのエントリーポイント。
 *
 * 変更点:
 * - 地面グリッドの範囲を拡大し、Y座標を立方体の下に変更。
 * - ワイヤーフレームの球体オブジェクトを追加。
 * - TopAngle のコメントアウトを解除 (必要に応じて再度コメントアウトしてください)。
 */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib 初期化フェーズ ---
    ChangeWindowMode(TRUE);
    SetWindowSizeChangeEnableFlag(FALSE);
    SetGraphMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), 32);
    if (DxLib_Init() < 0) return -1;
    SetMouseDispFlag(FALSE);
    SetDrawScreen(DX_SCREEN_BACK);

    // --- ロガーの初期化 ---
    if (!Logger::GetInstance().Open("debug_log.txt")) {
        printfDx("警告: デバッグログファイルを開けませんでした。\n");
    }
    LogDebug("Application Started.");

    // --- オブジェクトデータの準備 ---
    std::vector<std::vector<Vector3D>> worldLine;

    // --- 立方体の辺データ作成 ---
    {
        float s = 25.0f; // 半分の辺の長さ
        Vector3D center = { 0.0f, 0.0f, 50.0f }; // 少し奥に配置
        Vector3D p[8] = {
            {center.x - s, center.y - s, center.z - s}, {center.x + s, center.y - s, center.z - s},
            {center.x + s, center.y + s, center.z - s}, {center.x - s, center.y + s, center.z - s},
            {center.x - s, center.y - s, center.z + s}, {center.x + s, center.y - s, center.z + s},
            {center.x + s, center.y + s, center.z + s}, {center.x - s, center.y + s, center.z + s}
        };
        worldLine.push_back({ p[0], p[1] }); worldLine.push_back({ p[1], p[2] });
        worldLine.push_back({ p[2], p[3] }); worldLine.push_back({ p[3], p[0] });
        worldLine.push_back({ p[4], p[5] }); worldLine.push_back({ p[5], p[6] });
        worldLine.push_back({ p[6], p[7] }); worldLine.push_back({ p[7], p[4] });
        worldLine.push_back({ p[0], p[4] }); worldLine.push_back({ p[1], p[5] });
        worldLine.push_back({ p[2], p[6] }); worldLine.push_back({ p[3], p[7] });
    }

    // --- ★地面のグリッドデータ作成 (拡張・位置変更) ---
    {
        float groundY = -25.0f; // 地面のY座標 (立方体の底面と同じ高さ)
        float groundExtent = 500.0f; // 地面を描画する範囲を拡大
        float gridSize = 10.0f;     // グリッドの1マスのサイズ (少し粗く)
        int lineCount = static_cast<int>(groundExtent / gridSize);

        // X軸に平行な線 (Z方向に線を引く)
        for (int i = -lineCount; i <= lineCount; ++i) {
            float currentX = i * gridSize;
            Vector3D startPoint = { currentX, groundY, -groundExtent };
            Vector3D endPoint = { currentX, groundY,  groundExtent };
            worldLine.push_back({ startPoint, endPoint });
        }
        // Z軸に平行な線 (X方向に線を引く)
        for (int i = -lineCount; i <= lineCount; ++i) {
            float currentZ = i * gridSize;
            Vector3D startPoint = { -groundExtent, groundY, currentZ };
            Vector3D endPoint = { groundExtent, groundY, currentZ };
            worldLine.push_back({ startPoint, endPoint });
        }
    }

    // --- ★球体のワイヤーフレームデータ作成 ---
    {
        Vector3D sphereCenter = { 80.0f, 0.0f, 80.0f }; // 球の中心座標 (立方体の右奥上空)
        float sphereRadius = 30.0f;                   // 球の半径
        int latitudeDivisions = 12;                   // 緯度の分割数
        int longitudeDivisions = 24;                  // 経度の分割数

        // 緯線 (水平な円)
        for (int i = 1; i < latitudeDivisions; ++i) { // i=0 と i=latitudeDivisions は極なので除く
            float phi = PI * ((float)i / latitudeDivisions - 0.5f); // -PI/2 ～ PI/2 の範囲
            float y = sphereCenter.y + sphereRadius * sinf(phi);
            float r = sphereRadius * cosf(phi); // この高さでの半径

            Vector3D prevPoint;
            for (int j = 0; j <= longitudeDivisions; ++j) {
                float theta = 2.0f * PI * (float)j / longitudeDivisions; // 0 ～ 2PI の範囲
                float x = sphereCenter.x + r * cosf(theta);
                float z = sphereCenter.z + r * sinf(theta);
                Vector3D currentPoint = { x, y, z };
                if (j > 0) {
                    worldLine.push_back({ prevPoint, currentPoint });
                }
                prevPoint = currentPoint;
            }
        }

        // 経線 (垂直な半円)
        for (int j = 0; j < longitudeDivisions; ++j) {
            float theta = 2.0f * PI * (float)j / longitudeDivisions;
            Vector3D prevPoint;
            for (int i = 0; i <= latitudeDivisions; ++i) {
                float phi = PI * ((float)i / latitudeDivisions - 0.5f);
                float x = sphereCenter.x + sphereRadius * cosf(phi) * cosf(theta);
                float y = sphereCenter.y + sphereRadius * sinf(phi);
                float z = sphereCenter.z + sphereRadius * cosf(phi) * sinf(theta);
                Vector3D currentPoint = { x, y, z };
                if (i > 0) {
                    worldLine.push_back({ prevPoint, currentPoint });
                }
                prevPoint = currentPoint;
            }
        }
    }

    // --- カメラとトップダウンビューの生成 ---
    Camera* camera = new Camera();
    // !! 修正: TopAngle のコメントアウトを解除 !!
    TopAngle* topangle = new TopAngle(camera); // 再度有効化

    // --- メインループ ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();
        camera->Update();
        LogDebug(camera->GetDetailedDebugInfo()); // 詳細情報をログへ

        camera->Draw(worldLine);
        // !! 修正: TopAngle の描画を再度有効化 !!
        topangle->Draw(worldLine); // 再度有効化

        // 十字マーク描画 (省略)
        { int cX = (int)(WINDOW_WIDTH / 2), cY = (int)(WINDOW_HEIGHT / 2), sz = 10; DrawLine(cX - sz, cY, cX + sz, cY, GetColor(255, 255, 0)); DrawLine(cX, cY - sz, cX, cY + sz, GetColor(255, 255, 0)); }

        // デバッグ情報画面表示 (省略)
        { std::string dt = camera->GetDebugInfo(); DrawFormatString(10, WINDOW_HEIGHT - 40, GetColor(255, 255, 255), dt.c_str()); } // Y座標調整

        ScreenFlip();
    }

    // --- 終了処理 ---
    LogDebug("Application Ended.");
    Logger::GetInstance().Close();
    // !! 修正: topangle の delete を再度有効化 !!
    delete topangle; // 再度有効化
    delete camera;
    DxLib_End();
    return 0;
}