#pragma once
#include "Common.h"     // 定数 (WINDOW_WIDTH, WINDOW_HEIGHT, ONE_DEGREE, PI)
#include "Vector.h"     // Vector3D 構造体
#include <vector>       // std::vector (Drawメソッドの引数で使用)
#include <string>       // std::string (デバッグ情報用)
#include <sstream>      // std::stringstream (デバッグ情報用)
#include <iomanip>      // std::setprecision (デバッグ情報用)
#include "Matrix.h"     // Matrix 構造体 (ビュー・プロジェクション行列用)
#include "Quaternion.h" // Quaternion 構造体 (カメラの向き管理用)

/*
 * Camera.h
 * 概要:
 *   FPS (First Person Shooter) 視点のカメラ機能を提供するクラスのヘッダーファイルです。
 *   カメラの位置、向き（回転）、視野角などの情報を管理し、
 *   カメラ視点からの描画に必要な計算や、カメラ自体の操作（移動、回転）を行うための
 *   基本的な機能（メンバ変数やメンバ関数）を定義します。
 *
 * 元のコード(最初のバージョン)からの改善点 (このヘッダーファイルで定義されている内容):
 * 1. 回転の管理方法:
 *    - 元のコードではオイラー角（X,Y,Z軸周りの回転角度）で向きを管理していましたが、
 *      このヘッダーではクォータニオン (`orientation` メンバ変数) を使用しています。
 *    - クォータニオンを使うことで、特定の角度で回転がおかしくなる「ジンバルロック」問題を回避できます。
 *
 * 2. 標準的な座標変換機能:
 *    - 3Dグラフィックスで一般的に使われるビュー行列 (`GetViewMatrix`) と
 *      プロジェクション行列 (`GetProjectionMatrix`) を計算・取得する関数を提供します。
 *    - これにより、ワールド座標からスクリーン座標への変換を正確に行うことができます。
 *
 * 3. ローカル座標軸の取得:
 *    - カメラ自身の前・右・上方向を示すベクトル (`GetForwardVector` など) を取得できます。
 *    - これを使うことで、カメラの向いている方向を基準とした移動（ローカル移動）が可能になります。
 *
 * 4. 状態管理用のメンバ変数:
 *    - カメラの視野角(`fovY`)、画面比率(`aspectRatio`)、描画する奥行き範囲(`nearZ`, `farZ`)など、
 *      プロジェクション計算に必要なパラメータをメンバ変数として持ちます。
 *    - また、デバッグ情報の表示に必要な、前回のフレームでのマウス移動量(`lastMouseMoveX`)や
 *      計算された回転角度(`lastYawAngle`)などもメンバ変数として保持します。
 *
 * このヘッダーファイルの使い方:
 *   - 他のファイル (例: Main.cpp) で `#include "Camera.h"` します。
 *   - `Camera` クラスのオブジェクトを作成します (例: `Camera mainCamera;`)。
 *   - ゲームループの中で、毎フレーム `mainCamera.Update()` を呼び出して、
 *     プレイヤーの入力などに応じてカメラの状態（位置や向き）を更新します。
 *   - 描画の際には `mainCamera.Draw(worldLines)` を呼び出して、
 *     3Dオブジェクト（線データ）をカメラの視点から描画します。
 *   - 必要に応じて `mainCamera.GetPosition()` や `mainCamera.GetViewMatrix()` などで
 *     カメラの情報を取得して利用します。
 *   - デバッグ用に `mainCamera.GetDebugInfo()` や `mainCamera.GetDetailedDebugInfo()` を
 *     呼び出して、カメラの状態を確認できます。
 */

class Camera
{
public: // クラスの外部からアクセスできるメンバ (関数や変数)
    // コンストラクタ: Cameraオブジェクトが生成されるときに自動的に呼び出される関数
    Camera();
    // デストラクタ: Cameraオブジェクトが破棄されるときに自動的に呼び出される関数
    ~Camera();

    // 描画メソッド: ワールド空間の線分データ(`worldLines`)を受け取り、カメラから見た景色として描画する
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);
    // 更新メソッド: マウスやキーボードの入力に応じて、カメラの位置や向きを更新する
    void Update();

    // --- ゲッター (Getter) 関数 ---
    // クラスの内部データを取得するための関数群 (const指定で内部データを変更しないことを保証)

    // カメラの現在のワールド座標 (Vector3D) を取得する
    Vector3D GetPosition() const;

    // ビュー行列 (Matrix) を取得する。これはワールド座標系からカメラ座標系への変換を行う行列
    Matrix GetViewMatrix() const;
    // プロジェクション行列 (Matrix) を取得する。これはカメラ座標系からクリップ座標系への変換（透視投影）を行う行列
    Matrix GetProjectionMatrix() const;

    // カメラのローカル座標系の各軸が、現在ワールド座標系でどちらを向いているかを示すベクトルを取得する
    Vector3D GetForwardVector() const; // カメラの前方 (+Z軸方向)
    Vector3D GetRightVector() const;   // カメラの右方 (+X軸方向)
    Vector3D GetUpVector() const;      // カメラの上方 (+Y軸方向)

    // デバッグ情報取得用の関数
    std::string GetDebugInfo() const;         // 画面表示に適した、短い形式のデバッグ情報文字列を返す
    std::string GetDetailedDebugInfo() const; // ログファイル出力などに適した、詳細なデバッグ情報文字列を返す

private: // クラスの内部からのみアクセスできるメンバ (外部からは直接アクセスできない)
    // --- カメラの主要な状態を表すメンバ変数 ---
    Vector3D position;      // カメラの現在のワールド座標 (x, y, z)
    Quaternion orientation; // カメラの現在の向き（回転状態）を表すクォータニオン

    // --- プロジェクション（透視投影）関連のパラメータ (初期値付き) ---
    // これらの値は GetProjectionMatrix() 関数で使用される
    float fovY = 60.0f * ONE_DEGREE;            // 垂直方向の視野角 (Field of View Y)。ラジアン単位。
    float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT; // スクリーンのアスペクト比 (幅 / 高さ)。
    float nearZ = 0.1f;                         // ニアクリップ面。これより手前は描画されない。
    float farZ = 1000.0f;                       // ファークリップ面。これより奥は描画されない。

    // --- デバッグ表示用変数 (Updateメソッド内で毎フレーム更新される) ---
    // これらの変数は、主にデバッグ情報の表示やログ出力のために Update() 内で計算・保存される値。
    int lastMouseMoveX = 0;     // 前回のUpdate()でのマウスカーソルのX方向移動量
    int lastMouseMoveY = 0;     // 前回のUpdate()でのマウスカーソルのY方向移動量
    float lastYawAngle = 0.0f;    // 前回のUpdate()で計算されたヨー角（左右回転）の大きさ (ラジアン)
    float lastPitchAngle = 0.0f;  // 前回のUpdate()で計算されたピッチ角（上下回転）の大きさ (ラジアン)
    float lastRollAngle = 0.0f;   // 前回のUpdate()で計算されたロール角（傾き回転）の大きさ (ラジアン)
    float lastMoveForward = 0.0f; // 前回のUpdate()での前進/後退の入力状態 (-1.0, 0.0, 1.0)
    float lastMoveRight = 0.0f;   // 前回のUpdate()での右/左移動の入力状態 (-1.0, 0.0, 1.0)
    float lastMoveUp = 0.0f;      // 前回のUpdate()での上昇/下降の入力状態 (-1.0, 0.0, 1.0)
    Vector3D lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f }; // 前回のUpdate()で実際にカメラが移動したワールド空間でのベクトル

    // 現在のカメラのローカル軸ベクトルもメンバ変数として保持 (Update() の最後で更新される)
    // Update()内で計算した最新の軸ベクトルを保存しておくことで、GetForwardVector()などが呼ばれた時に
    // 再計算する手間を省くことができる（パフォーマンス向上の可能性）。
    Vector3D currentForward = { 0.0f, 0.0f, 1.0f }; // 現在のカメラの前方ベクトル (初期値はワールドZ+)
    Vector3D currentRight = { 1.0f, 0.0f, 0.0f };   // 現在のカメラの右方ベクトル (初期値はワールドX+)
    Vector3D currentUp = { 0.0f, 1.0f, 0.0f };     // 現在のカメラの上方ベクトル (初期値はワールドY+)
};