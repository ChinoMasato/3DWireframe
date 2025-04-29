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

/*
 * Main.cpp
 * 役割: アプリケーションのエントリーポイント。
 *
 * 変更点:
 * - Logger を初期化し、毎フレーム詳細なデバッグ情報をファイルに書き込む。
 * - TopAngle の描画をコメントアウト。
 * - デバッグ表示のY座標をさらに調整。
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

    // --- ★ロガーの初期化 ---
    if (!Logger::GetInstance().Open("debug_log.txt")) {
        // ファイルオープン失敗時の処理（ここでは単純に続ける）
        printfDx("警告: デバッグログファイルを開けませんでした。\n");
    }
    LogDebug("Application Started."); // 開始ログ

    // --- オブジェクトデータの準備 ---
    std::vector<std::vector<Vector3D>> worldLine;
    // 立方体と地面グリッドデータ (省略)
    { float s = 25.0f; Vector3D p[8] = { {-s,-s,-s},{s,-s,-s},{s,s,-s},{-s,s,-s},{-s,-s,s},{s,-s,s},{s,s,s},{-s,s,s} }; worldLine.push_back({ p[0], p[1] }); worldLine.push_back({ p[1], p[2] }); worldLine.push_back({ p[2], p[3] }); worldLine.push_back({ p[3], p[0] }); worldLine.push_back({ p[4], p[5] }); worldLine.push_back({ p[5], p[6] }); worldLine.push_back({ p[6], p[7] }); worldLine.push_back({ p[7], p[4] }); worldLine.push_back({ p[0], p[4] }); worldLine.push_back({ p[1], p[5] }); worldLine.push_back({ p[2], p[6] }); worldLine.push_back({ p[3], p[7] }); }
    { float groundExtent = 100.0f; float gridSize = 5.0f; int lineCount = static_cast<int>(groundExtent / gridSize); for (int i = -lineCount; i <= lineCount; ++i) { float cX = i * gridSize; worldLine.push_back({ {cX,0.0f,-groundExtent},{cX,0.0f,groundExtent} }); } for (int i = -lineCount; i <= lineCount; ++i) { float cZ = i * gridSize; worldLine.push_back({ {-groundExtent,0.0f,cZ},{groundExtent,0.0f,cZ} }); } }


    // --- カメラの生成 ---
    Camera* camera = new Camera();
    // !! 修正: TopAngle は生成・使用しない !!
    // TopAngle* topangle = new TopAngle(camera);

    // --- メインループ ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();
        camera->Update();

        // --- ★デバッグログファイルへの書き込み ---
        LogDebug(camera->GetDetailedDebugInfo()); // 詳細情報をログへ

        camera->Draw(worldLine);
        // topangle->Draw(worldLine); // コメントアウト

        // --- 十字マークの描画 ---
        { int cX = (int)(WINDOW_WIDTH / 2), cY = (int)(WINDOW_HEIGHT / 2), sz = 10; DrawLine(cX - sz, cY, cX + sz, cY, GetColor(255, 255, 0)); DrawLine(cX, cY - sz, cX, cY + sz, GetColor(255, 255, 0)); }

        // --- デバッグ情報の画面表示 ---
        {
            std::string debugText = camera->GetDebugInfo(); // 画面用は短い情報
            // !! 修正: Y座標をさらに調整 !!
            DrawFormatString(10, WINDOW_HEIGHT - 60, GetColor(255, 255, 255), debugText.c_str()); // Yを60に変更
        }

        ScreenFlip();
    }

    // --- 終了処理 ---
    LogDebug("Application Ended."); // 終了ログ
    Logger::GetInstance().Close(); // 明示的に閉じる（デストラクタでも閉じるが）
    // delete topangle; // 生成しないので不要
    delete camera;
    DxLib_End();

    return 0;
}