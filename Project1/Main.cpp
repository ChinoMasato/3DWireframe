// !! NOMINMAX はファイルの先頭 !!
#define NOMINMAX
#include "Common.h"     // 共通定数
#include "DxLib.h"      // DxLib ライブラリ
#include "Camera.h"     // Camera クラス
#include "TopAngle.h"   // TopAngle クラス
#include "Vector.h"     // Vector3D 構造体
#include <vector>       // std::vector
#include <string>       // std::string
#include "Logger.h"     // Logger クラス (ログ出力用)
#include <cmath>        // sinf, cosf (<math.h> より推奨)

/*
 * Main.cpp
 * 役割:
 *   このファイルはプログラム全体の開始点となる WinMain関数 を含んでいます。
 *   アプリケーションの起動から終了までの流れを管理します。
 *   主な処理内容は以下の通りです。
 *   - DxLibライブラリの初期化とウィンドウ設定
 *   - ログ出力機能(Logger)の準備
 *   - 3D空間に表示するオブジェクト（立方体、地面グリッド、球体）の線データ作成
 *   - カメラ(`Camera`)とトップダウンビュー(`TopAngle`)のオブジェクト生成
 *   - メインループ：プログラムが終了するまで繰り返し実行される部分
 *     - 入力処理 (DxLibのProcessMessage)
 *     - カメラとオブジェクトの状態更新 (camera->Update)
 *     - ログ出力 (LogDebug)
 *     - 画面への描画 (camera->Draw, topangle->Draw, デバッグ表示)
 *     - 画面の更新 (ScreenFlip)
 *   - プログラム終了時の後片付け (オブジェクトの解放、DxLib終了処理)
 *
 * このバージョンでの主な変更点・追加点 (元の Main.cpp からの変更点):
 * 1. ログ出力機能の利用 (Loggerクラス):
 *    - プログラムの動作状況をファイル("debug_log.txt")に記録する Logger クラスを利用します。
 *    - プログラムの開始・終了メッセージや、毎フレームのカメラの詳細な状態などを記録することで、
 *      動作確認や問題発生時の原因究明に役立ちます。
 *
 * 2. 描画する3Dオブジェクトの追加・変更:
 *    - 地面グリッド: 表示範囲を広げ、Y座標を調整して他のオブジェクトとの位置関係を分かりやすくしました。
 *    - 球体オブジェクト: 新たにワイヤーフレームの球体を追加しました。これにより、カメラから見たときの
 *                    曲面の表示や、より複雑なシーンでの動作を確認できます。
 *
 * 3. トップダウンビュー表示の有効化 (TopAngleクラス):
 *    - `TopAngle` クラスのオブジェクトを生成し、毎フレーム描画するようにしました。
 *    - これにより、画面左上にワールドを上から見た2Dマップが表示され、
 *      メインカメラの位置や向き、オブジェクトの配置を客観的に把握できます。
 *
 * 4. デバッグ用表示の強化:
 *    - 画面中央に十字マークを表示し、カメラの中心（注視点）を分かりやすくしました。
 *    - 画面左下にカメラの基本的な情報（座標、入力状態など）をリアルタイムで表示するようにしました。
 *      これにより、操作が正しく反映されているかなどを素早く確認できます。
 *
 * これらの変更は、主にカメラ機能の開発やテスト、デバッグを行いやすくすることを目的としています。
 *
 * 注意点:
 * - このプログラムを実行するには、`Camera`, `TopAngle`, `Logger`, `Vector`, `Matrix`, `Quaternion`,
 *   `CameraMath`, `Common` といった、関連する全てのクラスやヘッダーファイルが
 *   正しく実装され、プロジェクトに含まれている必要があります。
 */

 // ワイヤーフレームの立方体の頂点データを生成するヘルパー関数
std::vector<std::vector<Vector3D>> CreateCubeLines(float size, const Vector3D& center) {
    std::vector<std::vector<Vector3D>> lines;
    float halfSize = size * 0.5f;
    Vector3D vertices[8] = {
        center + Vector3D{-halfSize, -halfSize, -halfSize}, center + Vector3D{ halfSize, -halfSize, -halfSize},
        center + Vector3D{ halfSize,  halfSize, -halfSize}, center + Vector3D{-halfSize,  halfSize, -halfSize},
        center + Vector3D{-halfSize, -halfSize,  halfSize}, center + Vector3D{ halfSize, -halfSize,  halfSize},
        center + Vector3D{ halfSize,  halfSize,  halfSize}, center + Vector3D{-halfSize,  halfSize,  halfSize}
    };
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    for (const auto& edge : edges) {
        lines.push_back({ vertices[edge[0]], vertices[edge[1]] });
    }
    return lines;
}

// 球体のワイヤーフレームデータを生成するヘルパー関数
std::vector<std::vector<Vector3D>> CreateSphereLines(const Vector3D& center, float radius, int latDivs, int lonDivs) {
    std::vector<std::vector<Vector3D>> lines;
    // 緯線
    for (int i = 1; i < latDivs; ++i) {
        float phi = PI * ((float)i / latDivs - 0.5f);
        float y = center.y + radius * sinf(phi);
        float r = radius * cosf(phi);
        Vector3D prevPoint;
        for (int j = 0; j <= lonDivs; ++j) {
            float theta = 2.0f * PI * (float)j / lonDivs;
            float x = center.x + r * cosf(theta);
            float z = center.z + r * sinf(theta);
            Vector3D currentPoint = { x, y, z };
            if (j > 0) { lines.push_back({ prevPoint, currentPoint }); }
            prevPoint = currentPoint;
        }
    }
    // 経線
    for (int j = 0; j < lonDivs; ++j) {
        float theta = 2.0f * PI * (float)j / lonDivs;
        Vector3D prevPoint;
        for (int i = 0; i <= latDivs; ++i) {
            float phi = PI * ((float)i / latDivs - 0.5f);
            float x = center.x + radius * cosf(phi) * cosf(theta);
            float y = center.y + radius * sinf(phi);
            float z = center.z + radius * cosf(phi) * sinf(theta);
            Vector3D currentPoint = { x, y, z };
            if (i > 0) { lines.push_back({ prevPoint, currentPoint }); }
            prevPoint = currentPoint;
        }
    }
    return lines;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib 初期化フェーズ ---
    ChangeWindowMode(TRUE); // ウィンドウモード
    SetWindowSizeChangeEnableFlag(FALSE); // サイズ変更不可
    SetGraphMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), 32); // 画面モード設定
    if (DxLib_Init() < 0) { // DxLib初期化
        MessageBox(NULL, TEXT("DxLib の初期化に失敗しました。"), TEXT("エラー"), MB_OK); // エラー表示
        return -1;
    }
    SetMouseDispFlag(FALSE);       // マウスカーソル非表示
    SetDrawScreen(DX_SCREEN_BACK); // 描画先を裏画面に設定

    // --- ロガーの初期化 ---
    if (!Logger::GetInstance().Open("debug_log.txt")) { // ログファイルを開く
        printfDx("警告: デバッグログファイルを開けませんでした。\n"); // 失敗したら警告
    }
    LogDebug("アプリケーションを開始しました。"); // 開始ログを出力

    // --- オブジェクトデータの準備 ---
    std::vector<std::vector<Vector3D>> worldLine; // 描画する線分を格納するリスト

    // --- 立方体の辺データ作成 ---
    auto cubeLines = CreateCubeLines(50.0f, { 0.0f, 0.0f, 50.0f }); // サイズ50, 中心(0,0,50)
    worldLine.insert(worldLine.end(), cubeLines.begin(), cubeLines.end());

    // --- 地面グリッドデータ作成 ---
    {
        float groundY = -25.0f; // 地面のY座標
        float groundExtent = 500.0f;
        float gridSize = 10.0f;
        int lineCount = static_cast<int>(groundExtent / gridSize);
        for (int i = -lineCount; i <= lineCount; ++i) { // X並行線
            worldLine.push_back({ { (float)i * gridSize, groundY, -groundExtent }, { (float)i * gridSize, groundY,  groundExtent } });
        }
        for (int i = -lineCount; i <= lineCount; ++i) { // Z並行線
            worldLine.push_back({ { -groundExtent, groundY, (float)i * gridSize }, {  groundExtent, groundY, (float)i * gridSize } });
        }
    }

    // --- 球体のワイヤーフレームデータ作成 ---
    {
        auto sphereLines = CreateSphereLines({ 80.0f, 0.0f, 80.0f }, 30.0f, 12, 24); // 中心(80,0,80), 半径30
        worldLine.insert(worldLine.end(), sphereLines.begin(), sphereLines.end());
    }

    // --- カメラとトップダウンビューの生成 ---
    Camera* camera = new Camera(); // Cameraオブジェクト生成
    TopAngle* topangle = new TopAngle(camera); // TopAngleオブジェクト生成

    // --- メインループ ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) // ウィンドウが閉じられるかESCが押されるまで
    {
        // 1. 画面クリア
        ClearDrawScreen();

        // 2. 更新処理
        camera->Update(); // カメラの状態更新
        LogDebug(camera->GetDetailedDebugInfo()); // カメラ詳細情報をログへ

        // 3. 描画処理
        camera->Draw(worldLine);   // メインカメラ視点描画
        topangle->Draw(worldLine); // トップダウンビュー描画

        // 4. UI・デバッグ表示描画
        // 画面中央に十字マーク描画
        {
            int cX = static_cast<int>(WINDOW_WIDTH / 2), cY = static_cast<int>(WINDOW_HEIGHT / 2), sz = 10;
            unsigned int color = GetColor(255, 255, 0); // 黄色
            DrawLine(cX - sz, cY, cX + sz, cY, color);
            DrawLine(cX, cY - sz, cX, cY + sz, color);
        }

        // 画面左下にカメラの簡易デバッグ情報を表示
        {
            std::string dt = camera->GetDebugInfo(); // カメラ情報取得
            DrawFormatString(10, static_cast<int>(WINDOW_HEIGHT) - 40, GetColor(255, 255, 255), dt.c_str());
        }

        // 5. 画面更新
        ScreenFlip(); // 裏画面を表画面に表示
    }

    // --- 終了処理 ---
    LogDebug("アプリケーションを終了します。");
    Logger::GetInstance().Close(); // ロガー終了

    // new で確保したメモリを解放
    delete topangle;
    delete camera;

    DxLib_End(); // DxLib終了
    return 0;
}