#pragma once
#include "Common.h"
#include "Vector.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "Matrix.h"
#include "Quaternion.h"

class Camera
{
public:
    Camera();
    ~Camera();
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);
    void Update();
    Vector3D GetPosition() const;

    Matrix GetViewMatrix() const;
    Matrix GetProjectionMatrix() const;

    Vector3D GetForwardVector() const;
    Vector3D GetRightVector() const;
    Vector3D GetUpVector() const;

    std::string GetDebugInfo() const; // 画面表示用の短い情報
    std::string GetDetailedDebugInfo() const; // ログファイル用の詳細情報

private:
    Vector3D position;
    Quaternion orientation;

    float fovY = 60.0f * ONE_DEGREE;
    float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    // --- デバッグ表示用変数 (Updateで更新) ---
    int lastMouseMoveX = 0;
    int lastMouseMoveY = 0;
    float lastYawAngle = 0.0f;   // 計算されたヨー角
    float lastPitchAngle = 0.0f; // 計算されたピッチ角
    float lastRollAngle = 0.0f;  // 計算されたロール角
    float lastMoveForward = 0.0f;
    float lastMoveRight = 0.0f;
    float lastMoveUp = 0.0f;
    Vector3D lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f };
    // 現在の軸ベクトルも保持
    Vector3D currentForward = { 0.0f, 0.0f, 1.0f };
    Vector3D currentRight = { 1.0f, 0.0f, 0.0f };
    Vector3D currentUp = { 0.0f, 1.0f, 0.0f };
};