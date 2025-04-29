#pragma once
#include "Vector.h" // Vector3D �\���̂��g������
#include <vector>   // std::vector ���g������ (Draw �֐��̈���)

/*
 * TopAngle.h
 * ����:
 *   ���̃w�b�_�[�t�@�C���́A��ʂ̍���ɕ\������u�g�b�v�_�E���r���[�v
 *   �i���[���h��^�ォ�猩���낵���悤��2D�}�b�v�j��`�悷�邽�߂� `TopAngle` �N���X���`���܂��B
 *   ���̃r���[�́A���C���ƂȂ�3D���_�̃J���� (`Camera` �N���X�̃I�u�W�F�N�g) ��
 *   ���[���h���̂ǂ̈ʒu�ɂ��āA�ǂ̕����������Ă��邩�����o�I�Ɋm�F������A
 *   ���[���h�S�̂̃I�u�W�F�N�g�z�u��c�������肷��̂ɖ𗧂��܂��B
 *
 * ��ȋ@�\:
 *   - �Ď��Ώۂ� `Camera` �I�u�W�F�N�g�ւ̃|�C���^��ێ����܂��B
 *   - ���[���h��Ԃ̍��W (X, Z) ���A��ʍ���̃r���[�̈����2D�X�N���[�����W�ɕϊ����܂��B
 *   - `Draw` ���\�b�h�ŁA�r���[�̈�̔w�i�A�J�����̈ʒu�E�����E����p�A
 *     ����у��[���h���̃I�u�W�F�N�g�i�����j���r���[��ɕ`�悵�܂��B
 *
 * ���̃R�[�h����̕ύX�_�E�R�����g (����):
 *   - ���� `TopAngle` �N���X�́A���̏����R�[�h�ɂ͂Ȃ��A�J���̉ߒ���
 *     �f�o�b�O��󋵔c���̂��߂ɒǉ����ꂽ�@�\�ł���ƍl�����܂��B
 *   - `Camera` �N���X�ւ̈ˑ������邽�߁A`#include "Camera.h"` �𒼐ڋL�q��������
 *     �O���錾 (`class Camera;`) ���g�p���Ă��܂��B����́A�w�b�_�[�t�@�C�����m��
 *     �݂����C���N���[�h���������Ƃɂ���������邽�߂̈�ʓI�ȃe�N�j�b�N�ł��B
 *   - �g�b�v�_�E���r���[�̕\���ʒu�A�T�C�Y�A�k�ڂȂǂ̐ݒ�l���萔�Ƃ��Đ錾����Ă��܂��B
 *     �����̋�̓I�Ȓl�� `TopAngle.cpp` �t�@�C�����Œ�`����܂��B
 *   - ���[���h���W����r���[���W�֕ϊ����邽�߂̕⏕�֐� `ConvertWorldToView` ��
 *     �v���C�x�[�g�����o�Ƃ��Đ錾����Ă��܂��B
 *
 * �g����:
 *   - `#include "TopAngle.h"` ���C���N���[�h���܂��B
 *   - `TopAngle` �I�u�W�F�N�g���쐬����ۂɁA�Ď������� `Camera` �I�u�W�F�N�g�ւ̃|�C���^��n���܂�
 *     (��: `TopAngle* topView = new TopAngle(mainCameraPtr);`)�B
 *   - ���C�����[�v�̕`�揈���̒��� `topView->Draw(worldLines)` ���Ăяo���ƁA
 *     ��ʍ���Ƀg�b�v�_�E���r���[���`�悳��܂��B
 */

 // �O���錾 (Forward Declaration)
 // `Camera` �N���X�̊��S�Ȓ�`�������œǂݍ��ޕK�v�͂Ȃ��A
 // `Camera` �Ƃ������O�̃N���X�����݂��邱�Ƃ������R���p�C���ɓ`���܂��B
 // ����ɂ��A�R���p�C�����Ԃ̒Z�k��w�b�_�[�Ԃ̈ˑ��֌W�̐����ɖ𗧂��܂��B
class Camera;

class TopAngle
{
public: // �N���X�̊O������A�N�Z�X�\�ȃ����o
    // �R���X�g���N�^: �Ď��ΏۂƂȂ� `Camera` �I�u�W�F�N�g�ւ̃|�C���^ `cam` ���󂯎��܂��B
    TopAngle(Camera* cam);
    // �f�X�g���N�^: `TopAngle` �I�u�W�F�N�g���s�v�ɂȂ����Ƃ��ɌĂяo����܂��B
    ~TopAngle();

    // �`��֐�:
    //   ���C�����[�v���疈�t���[���Ăяo����A�g�b�v�_�E���r���[��`�悵�܂��B
    //   `worldLines` �́A���[���h��Ԃɑ��݂���I�u�W�F�N�g�̐����f�[�^�ł��B
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);

private: // �N���X�̓����ł̂݃A�N�Z�X�\�ȃ����o
    // �Ď��Ώۂ̃��C���J�����I�u�W�F�N�g�ւ̃|�C���^�B
    // ���̃|�C���^��ʂ��āADraw�֐����ŃJ�����̈ʒu��������擾���܂��B
    Camera* camera;

    // --- �g�b�v�_�E���r���[�̕\���ݒ� (�ÓI�萔�����o�[) ---
    //     �����̒萔�̎��ۂ̒l�� TopAngle.cpp �Œ�`����܂��B
    //     (��: C++17�ȍ~�ł̓C�����C���ϐ��Ƃ��ăw�b�_�[���Œ�`���邱�Ƃ��\�ł�)
    static const int VIEW_POS_X;     // �r���[�\���̈�̍����X���W (�X�N���[�����W)
    static const int VIEW_POS_Y;     // �r���[�\���̈�̍����Y���W (�X�N���[�����W)
    static const int VIEW_WIDTH;     // �r���[�\���̈�̕� (�s�N�Z���P��)
    static const int VIEW_HEIGHT;    // �r���[�\���̈�̍��� (�s�N�Z���P��)
    static const float VIEW_SCALE;   // ���[���h��Ԃ̍��W���r���[�\���̈�̍��W�ɕϊ�����ۂ̊g�嗦�E�k��
    static const float CAMERA_FOV_H; // ���C���J�����̐�������p (���W�A���P��)�B�r���[��Ɏ���͈͂�`�悷�邽�߂Ɏg�p�B
    static const float VIEW_RANGE;   // �r���[��ŃJ�����̌����⎋��p���������̒��� (�s�N�Z���P��)�B

    // �w���p�[�֐� (�N���X�����Ŏg���⏕�I�Ȋ֐�):
    //   ���[���h��Ԃ̍��W (X, Z) ���A��ʍ���̃g�b�v�_�E���r���[�\���̈����
    //   2D�X�N���[�����W (X, Y) �ɕϊ����܂��B
    Vector3D ConvertWorldToView(float worldX, float worldZ) const;
};