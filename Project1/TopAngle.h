#pragma once
#include "Vector.h" // Vector3D ���g�p���邽��
#include <vector>   // std::vector ���g�p���邽��

/*
 * TopAngle.h
 * ����: ��ʍ���Ƀ��[���h���ォ�猩���}�i�g�b�v�_�E���r���[�j��`�悷��N���X�̒�`�ł��B
 *       �Ď��Ώۂ̃J�����I�u�W�F�N�g�ւ̃|�C���^�������A���̃J�����̈ʒu������A
 *       ����юw�肳�ꂽ���[���h�I�u�W�F�N�g��2D�ŕ\�����܂��B
 *
 * �ŏ��̃R�[�h����̕ύX�_�E�R�����g:
 * - ���̃t�@�C���͍ŏ��̃R�[�h�ɂ͑��݂����A�g�b�v�_�E���r���[�@�\�̂��߂ɐV�K�ɒǉ�����܂����B
 * - Camera�N���X�ւ̈ˑ������邽�߁A�O���錾 (`class Camera;`) ���g�p���ďz�Q�Ƃ�h���ł��܂��B
 * - �`��̈�̐ݒ�l��w���p�[�֐����`���Ă��܂��B
 */

 // �O���錾: Camera�N���X�̒�`�𒼐ڃC���N���[�h�������ɃN���X��������錾
class Camera;

class TopAngle
{
public:
    // �R���X�g���N�^: �Ď��Ώۂ�Camera�I�u�W�F�N�g�ւ̃|�C���^���󂯎��
    TopAngle(Camera* cam);
    // �f�X�g���N�^
    ~TopAngle();

    // �`��֐�: ���[���h�̐����f�[�^���󂯎��A�g�b�v�_�E���r���[��`�悷��
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);

private:
    Camera* camera; // �Ď��Ώۂ̃J�����I�u�W�F�N�g�ւ̃|�C���^

    // --- �g�b�v�_�E���r���[�̕\���ݒ� (�萔) ---
    // ������TopAngle.cpp�Œ�`����܂��B
    static const int VIEW_POS_X;     // �r���[�̈�̍���X���W (�X�N���[�����W)
    static const int VIEW_POS_Y;     // �r���[�̈�̍���Y���W (�X�N���[�����W)
    static const int VIEW_WIDTH;     // �r���[�̈�̕� (�s�N�Z��)
    static const int VIEW_HEIGHT;    // �r���[�̈�̍��� (�s�N�Z��)
    static const float VIEW_SCALE;   // ���[���h���W����r���[���W�ւ̊g�嗦
    static const float CAMERA_FOV_H; // �J�����̐�������p (���W�A��) - �`��p
    static const float VIEW_RANGE;   // ����p���������̒��� (�s�N�Z��) - �`��p

    // �w���p�[�֐�: ���[���h���W(X, Z)���g�b�v�_�E���r���[�̃X�N���[�����W�ɕϊ�
    Vector3D ConvertWorldToView(float worldX, float worldZ) const;
};