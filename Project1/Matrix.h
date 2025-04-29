#pragma once
#include <cmath>      // sinf, cosf, tanf �Ȃ� (<math.h> ��萄��)
#include <stdexcept> // ��O���� (std::invalid_argument) �̂��߂ɕK�v
#include <cstring>   // memset �Ȃ� (�����g���ꍇ)

/*
 * Matrix.h
 * ����:
 *   ���̃w�b�_�[�t�@�C���́A3D�O���t�B�b�N�X�Ŕ��ɏd�v�Ȗ������ʂ���
 *   4x4�s�� (`Matrix` �\����) �̒�`�ƁA����Ɋ֘A�����{�I�Ȋ֐���񋟂��܂��B
 *   ��ȋ@�\�͈ȉ��̒ʂ�ł��B
 *   - `Matrix` �\����: 4x4�̕��������_��(float)���i�[����f�[�^�\���B
 *   - `Matrix::Identity()`: �����ϊ����Ȃ��u�P�ʍs��v���쐬����ÓI���\�b�h�B
 *   - �e������̉�]�s��𐶐�����֐� (`GetMatrixAxisXLH`, `GetMatrixAxisYLH`, `GetMatrixAxisZLH`)�B
 *     �����́u������W�n (Left-Handed, LH)�v�Ɋ�Â��Ă��܂��B
 *   - `MatrixMultiply`: 2�̍s����|�����킹��֐��B�ϊ��̍����Ɏg���܂��B
 *   - `PerspectiveFovLH`: �������e�i���ߊ��j��^����v���W�F�N�V�����s��𐶐�����֐��B
 *
 * ���̃o�[�W�����ł̕ύX�_ (���� Matrix.h ����):
 * - ��]�s�񐶐��֐�: ���� `GetMatrixAxisX`, `GetMatrixAxisY` ��������W�n(LH)��
 *   ��`�Ɋ�Â��Ă��邱�Ƃ����m�ɂȂ�悤�ɁA�֐����� `LH` ���ǉ�����A
 *   �����̌v�Z����������W�n�̕W���I�Ȓ�`�ɍ��킹�ďC������Ă��܂��B
 *   Z������̉�]�s�� `GetMatrixAxisZLH` ���Q�l�Ƃ��Ēǉ�����Ă��܂��B
 * - �@�\�ǉ�: �s��̏�Z (`MatrixMultiply`)�A�P�ʍs�� (`Identity`)�A
 *   �������e�s�� (`PerspectiveFovLH`) �𐶐�����֐����ǉ�����Ă��܂��B
 *   �����͌���I��3D�J�����V�X�e���̎����ɕs���ȗv�f�ł��B
 * - �G���[�`�F�b�N: `PerspectiveFovLH` �Ɋ֐��̈������s���ȏꍇ�ɗ�O�𓊂���
 *   ��{�I�ȃG���[�`�F�b�N���ǉ�����܂����B
 *
 * �g����:
 *   - ���̃w�b�_�[�t�@�C�����C���N���[�h (`#include "Matrix.h"`) ���܂��B
 *   - `Matrix` �^�̕ϐ����쐬���čs��f�[�^�������܂��B
 *   - `Matrix::Identity()` �ŒP�ʍs����쐬������A`GetMatrixAxisYLH(angle)` �Ȃǂ�
 *     ��]�s����쐬�����肵�܂��B
 *   - `MatrixMultiply(matA, matB)` �ōs��A�ƍs��B���|�����킹�����ʂ𓾂܂��B
 *   - `PerspectiveFovLH(...)` �Ńv���W�F�N�V�����s����쐬���܂��B
 *
 * ���ӓ_:
 * - �s��̗v�f�A�N�Z�X: `m[�s][��]` �̌`���iRow-major order�j�ŃA�N�Z�X���܂��B
 * - ������W�n(LH): ���̃w�b�_�[���̉�]�s���v���W�F�N�V�����s��́A
 *   DirectX�ȂǂŕW���I�Ɏg���鍶����W�n����ɂ��Ă��܂��B
 *   OpenGL�ȂǂŎg����E����W�n�Ƃ͈ꕔ�̕����Ȃǂ��قȂ�܂��B
 * - �s��̏�Z����: `MatrixMultiply(A, B)` �� A * B ���v�Z���܂����A
 *   ���W�ϊ���K�p����ۂ́A�ϊ���K�p�����������Ƌt�̏�����
 *   �s����|����̂���ʓI�ł��i��: �ړ� * ��] �Ȃ� `matRot * matTrans`�j�B
 */

 // 4x4 �s���\���\����
struct Matrix
{
    float m[4][4]; // 4x4 �� float �^�v�f

    // �f�t�H���g�R���X�g���N�^: �S�v�f�� 0.0f �ŏ�����
    Matrix() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = 0.0f;
            }
        }
        // ���邢�� <cstring> ���C���N���[�h����: std::memset(m, 0, sizeof(float) * 16);
    }

    // �ÓI���\�b�h: �P�ʍs�� (�Ίp������1�ő���0�̍s��) ���쐬���ĕԂ�
    static Matrix Identity() {
        Matrix result; // �[���s��ŏ����������
        result.m[0][0] = 1.0f;
        result.m[1][1] = 1.0f;
        result.m[2][2] = 1.0f;
        result.m[3][3] = 1.0f;
        return result;
    }
};

// X������̉�]�s��𐶐�����֐� (������W�n / LH)
// XAxisRotation: ��]�p�x (���W�A���P��)�B���̒l�Ŕ����v��� (Y������Z���֌���������)�B
// (����: DxLib �̃f�t�H���g�Ƃ͉�]�������t�̉\��������܂�)
static Matrix GetMatrixAxisXLH(float XAxisRotation)
{
    float sin_val = sinf(XAxisRotation);
    float cos_val = cosf(XAxisRotation);
    Matrix result = Matrix::Identity(); // �P�ʍs�񂩂�n�߂�
    // X����]�̐�����ݒ� (������W�n�̒�`�Ɋ�Â�)
    result.m[1][1] = cos_val;  result.m[1][2] = sin_val;  // �C��: LH�ł� (1,2) �� sin
    result.m[2][1] = -sin_val; result.m[2][2] = cos_val;  // �C��: LH�ł� (2,1) �� -sin
    return result;
}

// Y������̉�]�s��𐶐�����֐� (������W�n / LH)
// YAxisRotation: ��]�p�x (���W�A���P��)�B���̒l�Ŕ����v��� (Z������X���֌���������)�B
static Matrix GetMatrixAxisYLH(float YAxisRotation)
{
    float sin_val = sinf(YAxisRotation);
    float cos_val = cosf(YAxisRotation);
    Matrix result = Matrix::Identity();
    // Y����]�̐�����ݒ� (������W�n�̒�`�Ɋ�Â�)
    result.m[0][0] = cos_val;  result.m[0][2] = -sin_val; // �C��: LH�ł� (0,2) �� -sin
    result.m[2][0] = sin_val;  result.m[2][2] = cos_val;  // �C��: LH�ł� (2,0) �� sin
    return result;
}

// Z������̉�]�s��𐶐�����֐� (������W�n / LH) - �Q�l�Ƃ��Ēǉ�
// ZAxisRotation: ��]�p�x (���W�A���P��)�B���̒l�Ŕ����v��� (X������Y���֌���������)�B
static Matrix GetMatrixAxisZLH(float ZAxisRotation)
{
    float sin_val = sinf(ZAxisRotation);
    float cos_val = cosf(ZAxisRotation);
    Matrix result = Matrix::Identity();
    // Z����]�̐�����ݒ� (������W�n�̒�`�Ɋ�Â�)
    result.m[0][0] = cos_val;  result.m[0][1] = sin_val;  // �C��: LH�ł� (0,1) �� sin
    result.m[1][0] = -sin_val; result.m[1][1] = cos_val;  // �C��: LH�ł� (1,0) �� -sin
    return result;
}


// �s��̏�Z C = A * B ���v�Z����֐�
static Matrix MatrixMultiply(const Matrix& a, const Matrix& b)
{
    Matrix c; // ���ʂ��i�[����s�� (�R���X�g���N�^�Ń[�������������)
    // �s���Z�̒�`�ɏ]���Čv�Z
    for (int i = 0; i < 4; i++) {       // ���ʂ̍s�C���f�b�N�X
        for (int j = 0; j < 4; j++) {   // ���ʂ̗�C���f�b�N�X
            // c.m[i][j] = 0; // �R���X�g���N�^�ŏ������ς݂Ȃ̂ŕs�v
            for (int k = 0; k < 4; k++) { // �����̃��[�v (�a���v�Z)
                c.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return c;
}


// ������W�n�A����p�x�[�X�̓������e�s�� (Perspective Projection Matrix) ���쐬����֐�
// fovY: ���������̎���p (���W�A���P��)
// aspectRatio: �X�N���[���̃A�X�y�N�g�� (�� / ����)
// zn: �j�A�N���b�v�ʂ܂ł̋��� (Near Plane Distance) > 0
// zf: �t�@�[�N���b�v�ʂ܂ł̋��� (Far Plane Distance) > zn
static Matrix PerspectiveFovLH(float fovY, float aspectRatio, float zn, float zf)
{
    // �����̑Ó������`�F�b�N
    if (aspectRatio <= 0.0f || zn <= 0.0f || zf <= zn) {
        // �s���Ȉ������n���ꂽ�ꍇ�́A�G���[��������O�𓊂���
        throw std::invalid_argument("PerspectiveFovLH �ɕs���Ȉ������n����܂����B");
        // �܂��́A�G���[���b�Z�[�W��\�����ĒP�ʍs���Ԃ��Ȃǂ̑�֏������l������
        // DxLib ���������ς݂Ȃ� printfDx(...) �ȂǂŃG���[�\��
        // return Matrix::Identity();
    }

    Matrix result; // �[���s��ŏ����������
    // Y�����̃X�P�[�� (����p����v�Z)
    float yScale = 1.0f / tanf(fovY / 2.0f);
    // X�����̃X�P�[�� (Y�X�P�[���ƃA�X�y�N�g�䂩��v�Z)
    float xScale = yScale / aspectRatio;
    // Z�����̕ϊ��W�� (������W�n�̒�`)
    float zRange = zf / (zf - zn); // LH

    // �s��̗v�f��ݒ�
    result.m[0][0] = xScale;
    result.m[1][1] = yScale;
    result.m[2][2] = zRange;        // Z�l�� [0, 1] �͈̔͂ɋ߂Â��邽�߂̃X�P�[�� (LH)
    result.m[2][3] = 1.0f;          // W' �Ɍ���Z�l���R�s�[���邽�߂̐ݒ� (LH)
    result.m[3][2] = -zn * zRange;  // Z�l�̃I�t�Z�b�g (LH)
    // result.m[3][3] �� 0.0f �̂܂�

    return result;
}