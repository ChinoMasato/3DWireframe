#pragma once
#include "Common.h"     // �萔 (WINDOW_WIDTH, WINDOW_HEIGHT, ONE_DEGREE, PI)
#include "Vector.h"     // Vector3D �\����
#include <vector>       // std::vector (Draw���\�b�h�̈����Ŏg�p)
#include <string>       // std::string (�f�o�b�O���p)
#include <sstream>      // std::stringstream (�f�o�b�O���p)
#include <iomanip>      // std::setprecision (�f�o�b�O���p)
#include "Matrix.h"     // Matrix �\���� (�r���[�E�v���W�F�N�V�����s��p)
#include "Quaternion.h" // Quaternion �\���� (�J�����̌����Ǘ��p)

/*
 * Camera.h
 * �T�v:
 *   FPS (First Person Shooter) ���_�̃J�����@�\��񋟂���N���X�̃w�b�_�[�t�@�C���ł��B
 *   �J�����̈ʒu�A�����i��]�j�A����p�Ȃǂ̏����Ǘ����A
 *   �J�������_����̕`��ɕK�v�Ȍv�Z��A�J�������̂̑���i�ړ��A��]�j���s�����߂�
 *   ��{�I�ȋ@�\�i�����o�ϐ��⃁���o�֐��j���`���܂��B
 *
 * ���̃R�[�h(�ŏ��̃o�[�W����)����̉��P�_ (���̃w�b�_�[�t�@�C���Œ�`����Ă�����e):
 * 1. ��]�̊Ǘ����@:
 *    - ���̃R�[�h�ł̓I�C���[�p�iX,Y,Z������̉�]�p�x�j�Ō������Ǘ����Ă��܂������A
 *      ���̃w�b�_�[�ł̓N�H�[�^�j�I�� (`orientation` �����o�ϐ�) ���g�p���Ă��܂��B
 *    - �N�H�[�^�j�I�����g�����ƂŁA����̊p�x�ŉ�]�����������Ȃ�u�W���o�����b�N�v��������ł��܂��B
 *
 * 2. �W���I�ȍ��W�ϊ��@�\:
 *    - 3D�O���t�B�b�N�X�ň�ʓI�Ɏg����r���[�s�� (`GetViewMatrix`) ��
 *      �v���W�F�N�V�����s�� (`GetProjectionMatrix`) ���v�Z�E�擾����֐���񋟂��܂��B
 *    - ����ɂ��A���[���h���W����X�N���[�����W�ւ̕ϊ��𐳊m�ɍs�����Ƃ��ł��܂��B
 *
 * 3. ���[�J�����W���̎擾:
 *    - �J�������g�̑O�E�E�E������������x�N�g�� (`GetForwardVector` �Ȃ�) ���擾�ł��܂��B
 *    - ������g�����ƂŁA�J�����̌����Ă����������Ƃ����ړ��i���[�J���ړ��j���\�ɂȂ�܂��B
 *
 * 4. ��ԊǗ��p�̃����o�ϐ�:
 *    - �J�����̎���p(`fovY`)�A��ʔ䗦(`aspectRatio`)�A�`�悷�鉜�s���͈�(`nearZ`, `farZ`)�ȂǁA
 *      �v���W�F�N�V�����v�Z�ɕK�v�ȃp�����[�^�������o�ϐ��Ƃ��Ď����܂��B
 *    - �܂��A�f�o�b�O���̕\���ɕK�v�ȁA�O��̃t���[���ł̃}�E�X�ړ���(`lastMouseMoveX`)��
 *      �v�Z���ꂽ��]�p�x(`lastYawAngle`)�Ȃǂ������o�ϐ��Ƃ��ĕێ����܂��B
 *
 * ���̃w�b�_�[�t�@�C���̎g����:
 *   - ���̃t�@�C�� (��: Main.cpp) �� `#include "Camera.h"` ���܂��B
 *   - `Camera` �N���X�̃I�u�W�F�N�g���쐬���܂� (��: `Camera mainCamera;`)�B
 *   - �Q�[�����[�v�̒��ŁA���t���[�� `mainCamera.Update()` ���Ăяo���āA
 *     �v���C���[�̓��͂Ȃǂɉ����ăJ�����̏�ԁi�ʒu������j���X�V���܂��B
 *   - �`��̍ۂɂ� `mainCamera.Draw(worldLines)` ���Ăяo���āA
 *     3D�I�u�W�F�N�g�i���f�[�^�j���J�����̎��_����`�悵�܂��B
 *   - �K�v�ɉ����� `mainCamera.GetPosition()` �� `mainCamera.GetViewMatrix()` �Ȃǂ�
 *     �J�����̏����擾���ė��p���܂��B
 *   - �f�o�b�O�p�� `mainCamera.GetDebugInfo()` �� `mainCamera.GetDetailedDebugInfo()` ��
 *     �Ăяo���āA�J�����̏�Ԃ��m�F�ł��܂��B
 */

class Camera
{
public: // �N���X�̊O������A�N�Z�X�ł��郁���o (�֐���ϐ�)
    // �R���X�g���N�^: Camera�I�u�W�F�N�g�����������Ƃ��Ɏ����I�ɌĂяo�����֐�
    Camera();
    // �f�X�g���N�^: Camera�I�u�W�F�N�g���j�������Ƃ��Ɏ����I�ɌĂяo�����֐�
    ~Camera();

    // �`�惁�\�b�h: ���[���h��Ԃ̐����f�[�^(`worldLines`)���󂯎��A�J�������猩���i�F�Ƃ��ĕ`�悷��
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);
    // �X�V���\�b�h: �}�E�X��L�[�{�[�h�̓��͂ɉ����āA�J�����̈ʒu��������X�V����
    void Update();

    // --- �Q�b�^�[ (Getter) �֐� ---
    // �N���X�̓����f�[�^���擾���邽�߂̊֐��Q (const�w��œ����f�[�^��ύX���Ȃ����Ƃ�ۏ�)

    // �J�����̌��݂̃��[���h���W (Vector3D) ���擾����
    Vector3D GetPosition() const;

    // �r���[�s�� (Matrix) ���擾����B����̓��[���h���W�n����J�������W�n�ւ̕ϊ����s���s��
    Matrix GetViewMatrix() const;
    // �v���W�F�N�V�����s�� (Matrix) ���擾����B����̓J�������W�n����N���b�v���W�n�ւ̕ϊ��i�������e�j���s���s��
    Matrix GetProjectionMatrix() const;

    // �J�����̃��[�J�����W�n�̊e�����A���݃��[���h���W�n�łǂ���������Ă��邩�������x�N�g�����擾����
    Vector3D GetForwardVector() const; // �J�����̑O�� (+Z������)
    Vector3D GetRightVector() const;   // �J�����̉E�� (+X������)
    Vector3D GetUpVector() const;      // �J�����̏�� (+Y������)

    // �f�o�b�O���擾�p�̊֐�
    std::string GetDebugInfo() const;         // ��ʕ\���ɓK�����A�Z���`���̃f�o�b�O��񕶎����Ԃ�
    std::string GetDetailedDebugInfo() const; // ���O�t�@�C���o�͂ȂǂɓK�����A�ڍׂȃf�o�b�O��񕶎����Ԃ�

private: // �N���X�̓�������̂݃A�N�Z�X�ł��郁���o (�O������͒��ڃA�N�Z�X�ł��Ȃ�)
    // --- �J�����̎�v�ȏ�Ԃ�\�������o�ϐ� ---
    Vector3D position;      // �J�����̌��݂̃��[���h���W (x, y, z)
    Quaternion orientation; // �J�����̌��݂̌����i��]��ԁj��\���N�H�[�^�j�I��

    // --- �v���W�F�N�V�����i�������e�j�֘A�̃p�����[�^ (�����l�t��) ---
    // �����̒l�� GetProjectionMatrix() �֐��Ŏg�p�����
    float fovY = 60.0f * ONE_DEGREE;            // ���������̎���p (Field of View Y)�B���W�A���P�ʁB
    float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT; // �X�N���[���̃A�X�y�N�g�� (�� / ����)�B
    float nearZ = 0.1f;                         // �j�A�N���b�v�ʁB�������O�͕`�悳��Ȃ��B
    float farZ = 1000.0f;                       // �t�@�[�N���b�v�ʁB�����艜�͕`�悳��Ȃ��B

    // --- �f�o�b�O�\���p�ϐ� (Update���\�b�h���Ŗ��t���[���X�V�����) ---
    // �����̕ϐ��́A��Ƀf�o�b�O���̕\���⃍�O�o�͂̂��߂� Update() ���Ōv�Z�E�ۑ������l�B
    int lastMouseMoveX = 0;     // �O���Update()�ł̃}�E�X�J�[�\����X�����ړ���
    int lastMouseMoveY = 0;     // �O���Update()�ł̃}�E�X�J�[�\����Y�����ړ���
    float lastYawAngle = 0.0f;    // �O���Update()�Ōv�Z���ꂽ���[�p�i���E��]�j�̑傫�� (���W�A��)
    float lastPitchAngle = 0.0f;  // �O���Update()�Ōv�Z���ꂽ�s�b�`�p�i�㉺��]�j�̑傫�� (���W�A��)
    float lastRollAngle = 0.0f;   // �O���Update()�Ōv�Z���ꂽ���[���p�i�X����]�j�̑傫�� (���W�A��)
    float lastMoveForward = 0.0f; // �O���Update()�ł̑O�i/��ނ̓��͏�� (-1.0, 0.0, 1.0)
    float lastMoveRight = 0.0f;   // �O���Update()�ł̉E/���ړ��̓��͏�� (-1.0, 0.0, 1.0)
    float lastMoveUp = 0.0f;      // �O���Update()�ł̏㏸/���~�̓��͏�� (-1.0, 0.0, 1.0)
    Vector3D lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f }; // �O���Update()�Ŏ��ۂɃJ�������ړ��������[���h��Ԃł̃x�N�g��

    // ���݂̃J�����̃��[�J�����x�N�g���������o�ϐ��Ƃ��ĕێ� (Update() �̍Ō�ōX�V�����)
    // Update()���Ōv�Z�����ŐV�̎��x�N�g����ۑ����Ă������ƂŁAGetForwardVector()�Ȃǂ��Ă΂ꂽ����
    // �Čv�Z�����Ԃ��Ȃ����Ƃ��ł���i�p�t�H�[�}���X����̉\���j�B
    Vector3D currentForward = { 0.0f, 0.0f, 1.0f }; // ���݂̃J�����̑O���x�N�g�� (�����l�̓��[���hZ+)
    Vector3D currentRight = { 1.0f, 0.0f, 0.0f };   // ���݂̃J�����̉E���x�N�g�� (�����l�̓��[���hX+)
    Vector3D currentUp = { 0.0f, 1.0f, 0.0f };     // ���݂̃J�����̏���x�N�g�� (�����l�̓��[���hY+)
};