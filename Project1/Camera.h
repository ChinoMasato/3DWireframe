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

    std::string GetDebugInfo() const; // ��ʕ\���p�̒Z�����
    std::string GetDetailedDebugInfo() const; // ���O�t�@�C���p�̏ڍ׏��

private:
    Vector3D position;
    Quaternion orientation;

    float fovY = 60.0f * ONE_DEGREE;
    float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    // --- �f�o�b�O�\���p�ϐ� (Update�ōX�V) ---
    int lastMouseMoveX = 0;
    int lastMouseMoveY = 0;
    float lastYawAngle = 0.0f;   // �v�Z���ꂽ���[�p
    float lastPitchAngle = 0.0f; // �v�Z���ꂽ�s�b�`�p
    float lastRollAngle = 0.0f;  // �v�Z���ꂽ���[���p
    float lastMoveForward = 0.0f;
    float lastMoveRight = 0.0f;
    float lastMoveUp = 0.0f;
    Vector3D lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f };
    // ���݂̎��x�N�g�����ێ�
    Vector3D currentForward = { 0.0f, 0.0f, 1.0f };
    Vector3D currentRight = { 1.0f, 0.0f, 0.0f };
    Vector3D currentUp = { 0.0f, 1.0f, 0.0f };
};