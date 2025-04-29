#include "TopAngle.h"
#include "Camera.h"   // Camera�N���X�̒�`���Q�Ƃ��邽�� (GetPosition, GetForwardVector�Ȃ�)
#include "DxLib.h"    // DxLib�̕`��֐����g������
#include "Common.h"   // �萔(PI, ONE_DEGREE, WINDOW_WIDTH, WINDOW_HEIGHT) ���g������
#include <math.h>     // sinf, cosf, atan2 ���g������
#include <vector>     // std::vector (Draw�̈����^)

/*
 * TopAngle.cpp
 * �g�b�v�_�E���r���[�`��N���X�̎����B
 * �ύX�_:
 * - �J�����̌����擾�� GetRotation() ���� GetForwardVector() ���g�����v�Z�ɕύX�B
 */

 // --- �g�b�v�_�E���r���[�̕\���ݒ�l�̒�` ---
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
        printfDx("�x��: TopAngle �ɓn���ꂽ�J�����|�C���^�� nullptr �ł��B\n");
    }
}

TopAngle::~TopAngle() {}

// �w���p�[�֐�: ���[���h���W(X, Z)���g�b�v�_�E���r���[���W�ɕϊ�
Vector3D TopAngle::ConvertWorldToView(float worldX, float worldZ) const
{
    if (!camera) return { (float)VIEW_POS_X, (float)VIEW_POS_Y, 0.0f };
    Vector3D camPos = camera->GetPosition();
    float relativeX = worldX - camPos.x;
    float relativeZ = worldZ - camPos.z;
    float viewCenterX = (float)(VIEW_POS_X + VIEW_WIDTH / 2);
    float viewCenterY = (float)(VIEW_POS_Y + VIEW_HEIGHT / 2);
    float viewX = viewCenterX + relativeX * VIEW_SCALE;
    float viewY = viewCenterY + relativeZ * VIEW_SCALE; // Z+��Y+ (��)
    return { viewX, viewY, 0.0f };
}


// �g�b�v�_�E���r���[�̕`��֐�
void TopAngle::Draw(const std::vector<std::vector<Vector3D>>& worldLines)
{
    if (!camera) return;

    // --- 1. �r���[�̈�̔w�i�Ƙg���̕`�� ---
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(255, 255, 255), FALSE);

    // --- 2. �J�����̕`�� ---
    int camViewX = VIEW_POS_X + VIEW_WIDTH / 2;
    int camViewY = VIEW_POS_Y + VIEW_HEIGHT / 2;
    unsigned int camColor = GetColor(255, 0, 0);
    DrawCircle(camViewX, camViewY, 4, camColor, TRUE);

    // --- 3. �J�����̌����Ǝ���p�̕`�� ---
    // !! �C��: �N�H�[�^�j�I�����烈�[�p���擾 !!
    // GetRotation() �̑���ɑO���x�N�g������XZ���ʏ�̌������v�Z
    Vector3D forward = camera->GetForwardVector();
    // XZ���ʏ�ł̊p�x (���[�p) �� atan2 �Ōv�Z (Z���O����A���v���)
    float camRotY = atan2f(forward.x, forward.z); // atan2(x, z) �� Z������̊p�x

    // �J�����̑O���x�N�g�� (�r���[���W�n�ł̃I�t�Z�b�g)
    float forwardXOffset = sinf(camRotY) * VIEW_RANGE;
    float forwardYOffset = cosf(camRotY) * VIEW_RANGE;

    // �J�����̌�����������
    DrawLine(camViewX, camViewY, camViewX + (int)forwardXOffset, camViewY + (int)forwardYOffset, camColor);

    // ����p�̍��E�̐��̊p�x
    float leftFovRot = camRotY - CAMERA_FOV_H / 2.0f;
    float rightFovRot = camRotY + CAMERA_FOV_H / 2.0f;
    float leftXOffset = sinf(leftFovRot) * VIEW_RANGE;
    float leftYOffset = cosf(leftFovRot) * VIEW_RANGE;
    float rightXOffset = sinf(rightFovRot) * VIEW_RANGE;
    float rightYOffset = cosf(rightFovRot) * VIEW_RANGE;

    // ����p��������
    DrawLine(camViewX, camViewY, camViewX + (int)leftXOffset, camViewY + (int)leftYOffset, camColor);
    DrawLine(camViewX, camViewY, camViewX + (int)rightXOffset, camViewY + (int)rightYOffset, camColor);
    DrawLine(camViewX + (int)leftXOffset, camViewY + (int)leftYOffset, camViewX + (int)rightXOffset, camViewY + (int)rightYOffset, camColor);


    // --- 4. �I�u�W�F�N�g (����) �̕`�� ---
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