#include "TopAngle.h" // �Ή�����w�b�_�[�t�@�C��
#include "Camera.h"   // Camera�N���X�̒�`���Q�Ƃ��邽�� (GetPosition, GetForwardVector�Ȃǂ��g��)
#include "DxLib.h"    // DxLib�̕`��֐����g������
#include "Common.h"   // �萔(PI, ONE_DEGREE, WINDOW_WIDTH, WINDOW_HEIGHT) ���g������
#include <cmath>      // sinf, cosf, atan2f �Ȃǂ̐��w�֐����g������ (<math.h> ��� <cmath> �� C++ �ł͐���)
#include <vector>     // std::vector (Draw�̈����^)

/*
 * TopAngle.cpp (�R�����g�C���� - ���R�[�h�ێ�)
 * �T�v:
 *   TopAngle�N���X�̋�̓I�ȏ�������������t�@�C���ł��B
 *   ��ʍ���ɕ\������g�b�v�_�E���r���[�i�^�ォ��̎��_�j�̕`�惍�W�b�N��S�����܂��B
 *   ��ȋ@�\�͈ȉ��̒ʂ�ł��B
 *   - �R���X�g���N�^�ł̏������i�Ď��Ώۂ�Camera�I�u�W�F�N�g���󂯎��j
 *   - ���[���h��Ԃ̍��W���A�g�b�v�_�E���r���[�\���̈����2D�X�N���[�����W�ɕϊ�����
 *   - ���t���[���A�r���[�̈�̔w�i�A���C���J�����̈ʒu�ƌ����A����p�A
 *     �����ă��[���h���̃I�u�W�F�N�g�i�����j��`�悷��
 *
 * ���̃R�[�h����̎�ȕύX�_:
 *   - �J�����̌������擾������@���A�V���� `Camera` �N���X�ɍ��킹�ĕύX����Ă��܂��B
 *     ���̃R�[�h�ł͒��ډ�]�p�x���擾���Ă����Ǝv���܂����A�V�����J�����ł�
 *     �N�H�[�^�j�I���Ō������Ǘ����Ă��邽�߁A����� `GetForwardVector()` ��
 *     �O���x�N�g�����擾���A����XZ�������� `atan2f` ���g���ďォ�猩���Ƃ���
 *     �����̊p�x�i���[�p�j���v�Z���Ă��܂��B
 *
 * ���̃t�@�C�����̏����̗���:
 * 1. �萔��`: �g�b�v�_�E���r���[�̕\���ʒu�A�T�C�Y�A�X�P�[���Ȃǂ̐ݒ�l���`���܂��B
 * 2. �R���X�g���N�^: `Camera` �I�u�W�F�N�g�ւ̃|�C���^���󂯎��A�ێ����܂��B
 * 3. `ConvertWorldToView`: ���[���h���W(X, Z)���󂯎��A�J��������̑��Έʒu��
 *    �X�P�[���Ɋ�Â��āA�r���[�̈���̃X�N���[�����W(X, Y)�ɕϊ����܂��B
 * 4. `Draw`:
 *    - �w�i�Ƙg����`�悵�܂��B
 *    - �r���[�̈�̒��S�Ƀ��C���J������\���~��`�悵�܂��B
 *    - `GetForwardVector` �� `atan2f` ���g���ăJ�����̌����p�x���v�Z���A
 *      ���̕����ɐ���`�悵�܂��B
 *    - �ݒ肳�ꂽ����p (`CAMERA_FOV_H`) �Ɋ�Â��āA����͈͂���������`�悵�܂��B
 *    - `worldLines` �œn���ꂽ�e������ `ConvertWorldToView` �ŕϊ����A
 *      �r���[�̈���ɐ��Ƃ��ĕ`�悵�܂� (`SetDrawArea` �ŕ`��͈͂𐧌�)�B
 */

 // --- �g�b�v�_�E���r���[�̕\���ݒ�l�̒�` ---
 // TopAngle.h �Ő錾���ꂽ�ÓI�萔�����o�[�̒l�������Œ�`���܂��B
 // (��: C++17�ȍ~�ł̓C�����C���ϐ��Ƃ��ăw�b�_�[���Œ�`���邱�Ƃ��\�ł�)
const int TopAngle::VIEW_POS_X = 10;      // �r���[�̈�̍����X���W (��ʍ�����10�s�N�Z��)
const int TopAngle::VIEW_POS_Y = 10;      // �r���[�̈�̍����Y���W (��ʏォ��10�s�N�Z��)
const int TopAngle::VIEW_WIDTH = 200;     // �r���[�̈�̕� (200�s�N�Z��)
const int TopAngle::VIEW_HEIGHT = 200;    // �r���[�̈�̍��� (200�s�N�Z��)
const float TopAngle::VIEW_SCALE = 1.5f;  // �`��X�P�[�� (���[���h��1�P�ʂ�1.5�s�N�Z���ŕ\��)
const float TopAngle::CAMERA_FOV_H = 75.0f * ONE_DEGREE; // �J�����̐�������p (��75�x) - ����p�\���Ɏg�p
const float TopAngle::VIEW_RANGE = 60.0f; // ����p��J�����̌������������̒��� (60�s�N�Z��)

// �R���X�g���N�^:
//   ���C���J�����ւ̃|�C���^���󂯎��A�����o�ϐ� `camera` �ɕۑ����܂��B
TopAngle::TopAngle(Camera* cam) : camera(cam)
{
    // �O�̂��߁A�n���ꂽ�|�C���^�� nullptr (�����ȃ|�C���^) �łȂ����`�F�b�N���܂��B
    if (camera == nullptr) {
        // ���� nullptr �Ȃ�ADxLib �̃f�o�b�O�\���@�\���g���Čx�����b�Z�[�W��\�����܂��B
        printfDx("�x��: TopAngle �ɓn���ꂽ�J�����|�C���^�� nullptr �ł��B\n");
        // ���̏ꍇ�A�ȍ~�� Draw �֐��Ȃǂ� `camera` ���Q�Ƃ���ƃG���[�ɂȂ�\��������܂��B
    }
}

// �f�X�g���N�^:
//   ����͓��Ɍ�Еt�������͕K�v����܂���B
TopAngle::~TopAngle() {}

// �w���p�[�֐�: ���[���h���W(X, Z)���g�b�v�_�E���r���[�̃X�N���[�����W�ɕϊ�����
Vector3D TopAngle::ConvertWorldToView(float worldX, float worldZ) const
{
    // �J�����|�C���^�������Ȃ�A�f�t�H���g���W�i�r���[����j��Ԃ��Ȃǂ��ăG���[��h��
    if (!camera) {
        // �萔���g���悤�ɏC�� (�ȑO�̉񓚂Ńw�b�_�[�� constexpr �Œ�`�����ꍇ)
        // return { (float)TopAngle::VIEW_POS_X, (float)TopAngle::VIEW_POS_Y, 0.0f };
        // ���̃t�@�C���Œ�`���Ă���萔�����̂܂܎g���ꍇ:
        return { (float)VIEW_POS_X, (float)VIEW_POS_Y, 0.0f };
    }

    // 1. ���C���J�����̌��݂̃��[���h���W���擾
    Vector3D camPos = camera->GetPosition();

    // 2. �`�悵�����_�́A�J��������̑��΍��W (XZ���ʏ�) ���v�Z
    float relativeX = worldX - camPos.x;
    float relativeZ = worldZ - camPos.z; // ���[���hZ���W���r���[��Y���W�ɑΉ�����

    // 3. �r���[�̈�̒��S�̃X�N���[�����W���v�Z
    float viewCenterX = (float)(VIEW_POS_X + VIEW_WIDTH / 2);
    float viewCenterY = (float)(VIEW_POS_Y + VIEW_HEIGHT / 2);

    // 4. ���΍��W�ɃX�P�[�����|���āA�r���[���S����̃I�t�Z�b�g���v�Z���A�ŏI�I�ȃX�N���[�����W�����߂�
    float viewX = viewCenterX + relativeX * VIEW_SCALE;
    float viewY = viewCenterY + relativeZ * VIEW_SCALE; // Z+ �� Y+ (������) �Ƀ}�b�s���O

    // ���ʂ� Vector3D �ŕԂ� (Z�����͎g��Ȃ�)
    return { viewX, viewY, 0.0f };
}


// �g�b�v�_�E���r���[�̕`��֐� (���t���[���Ăяo�����)
void TopAngle::Draw(const std::vector<std::vector<Vector3D>>& worldLines)
{
    // �J�����|�C���^�������Ȃ�A�����`�悹���ɏI��
    if (!camera) {
        return;
    }

    // --- 1. �r���[�̈�̔w�i�Ƙg���̕`�� ---
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // ���������[�h�ݒ�
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(0, 0, 0), TRUE); // �w�i�`��
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // �ʏ탂�[�h�ɖ߂�
    DrawBox(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT, GetColor(255, 255, 255), FALSE); // �g���`��

    // --- 2. �J�������g�̕`�� ---
    int camViewX = VIEW_POS_X + VIEW_WIDTH / 2; // �r���[���SX
    int camViewY = VIEW_POS_Y + VIEW_HEIGHT / 2; // �r���[���SY
    unsigned int camColor = GetColor(255, 0, 0); // �J�����̐F(��)
    DrawCircle(camViewX, camViewY, 4, camColor, TRUE); // �J�����ʒu���~�ŕ\��

    // --- 3. �J�����̌����Ǝ���p�̕`�� ---
    // �J�����̑O���x�N�g���擾
    Vector3D forward = camera->GetForwardVector();
    // XZ���ʏ�ł̌����p�x(���[�p)��atan2f(x, z)�Ōv�Z (Z���O���)
    float camRotY = atan2f(forward.x, forward.z);

    // �J�����̌������������̃I�t�Z�b�g�v�Z
    float forwardXOffset = sinf(camRotY) * VIEW_RANGE;
    float forwardYOffset = cosf(camRotY) * VIEW_RANGE;

    // �J�����̌�������������`��
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(forwardXOffset), camViewY + static_cast<int>(forwardYOffset), camColor);

    // --- ����p�\�� ---
    float halfFovH = CAMERA_FOV_H / 2.0f; // ��������p�̔���
    float leftFovRot = camRotY - halfFovH; // ���[�̊p�x
    float rightFovRot = camRotY + halfFovH; // �E�[�̊p�x
    // ���E�̐��̃I�t�Z�b�g�v�Z
    float leftXOffset = sinf(leftFovRot) * VIEW_RANGE;
    float leftYOffset = cosf(leftFovRot) * VIEW_RANGE;
    float rightXOffset = sinf(rightFovRot) * VIEW_RANGE;
    float rightYOffset = cosf(rightFovRot) * VIEW_RANGE;

    // ����p����������`��
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(leftXOffset), camViewY + static_cast<int>(leftYOffset), camColor); // ����
    DrawLine(camViewX, camViewY, camViewX + static_cast<int>(rightXOffset), camViewY + static_cast<int>(rightYOffset), camColor); // �E��
    DrawLine(camViewX + static_cast<int>(leftXOffset), camViewY + static_cast<int>(leftYOffset), // ��[��
        camViewX + static_cast<int>(rightXOffset), camViewY + static_cast<int>(rightYOffset), camColor);


    // --- 4. ���[���h�I�u�W�F�N�g (����) �̕`�� ---
    unsigned int objectColor = GetColor(0, 255, 0); // �I�u�W�F�N�g�̐F(��)

    // �`��͈͂��g�b�v�_�E���r���[�̈���Ɍ���
    SetDrawArea(VIEW_POS_X, VIEW_POS_Y, VIEW_POS_X + VIEW_WIDTH, VIEW_POS_Y + VIEW_HEIGHT);

    // ���[���h�̐�����`��
    for (const auto& line : worldLines)
    {
        if (line.size() < 2) { continue; }
        // �����̗��[���r���[���W�ɕϊ�
        Vector3D viewP1 = ConvertWorldToView(line[0].x, line[0].z);
        Vector3D viewP2 = ConvertWorldToView(line[1].x, line[1].z);
        // ����`�� (�A���`�G�C���A�X�t��)
        DrawLineAA(viewP1.x, viewP1.y, viewP2.x, viewP2.y, objectColor);
    }

    // �`��͈͂̌��������
    SetDrawArea(0, 0, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT));
}