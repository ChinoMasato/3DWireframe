#pragma once
#include <cmath>     // fabsf, sinf, cosf �Ȃ� (<math.h> ��萄��)
#include "Vector.h" // Vector3D ���g�p
#include "Matrix.h" // Matrix ���g�p
#include <sstream> // �f�o�b�O���O�p (stringstream)
#include <iomanip> // �f�o�b�O���O�p (setprecision)
#include "Logger.h" // �f�o�b�O���O�p (LogDebug) (�K�v�Ȃ�C���N���[�h)

/*
 * CameraMath.h
 * ����:
 *   ���̃w�b�_�[�t�@�C���́A�J�����֘A�A���Ƀx�N�g���ƍs���p����
 *   ���W�ϊ��v�Z�ȂǁA3D�O���t�B�b�N�X�ɂ����鐔�w�I�ȏ������s�����߂�
 *   �w���p�[�֐���\���̂��`���܂��B
 *   �ȑO�̃o�[�W�������琮���E�ύX����Ă��܂��B
 *
 * ���̃o�[�W�����ł̕ύX�_ (�ȑO�� CameraMath.h ����):
 * - Vector4D �\����: 4�����x�N�g���i�������W�j���������߂̍\���̂���`����Ă��܂��B
 *                    �����3D���W�ϊ��i���ɓ������e�j�ŏd�v�Ȗ������ʂ����܂��B
 * - VEC4Transform �֐�: 4�����x�N�g�����s��ŕϊ�����֐�����������Ă��܂��B
 *                      �R�����g�A�E�g����Ă��܂����A�f�o�b�O���O�o�͂̋@�\���܂܂�Ă��܂��B
 * - TransformCoord �֐�: 3�����́u���W�_�v���s��ŕϊ����A�p�[�X�y�N�e�B�u���Z�iw�Ŋ���j��
 *                       �s���čĂ�3�������W�ɖ߂��֐�����������Ă��܂��B
 *                       ����́A�r���[�E�v���W�F�N�V�����ϊ��ȂǂŎg���܂��B
 * - VEC3Transform �֐�: 3�����x�N�g�����s��̍���3x3�����i��]�E�X�P�[�����O�j�ŕϊ�����֐��ł��B
 *                      ����͎�Ɂu�����x�N�g���v�̕ϊ��Ɏg���܂��i���s�ړ��̉e���͎󂯂܂���j�B
 *                      �i���� `CameraMath.h` �ɂ��ގ��̊֐�����������������܂���j
 *
 * �g����:
 *   - ���̃t�@�C�����C���N���[�h (`#include "CameraMath.h"`) ���܂��B
 *   - �K�v�ɉ����� `Vector4D` �\���̂��g�p���܂��B
 *   - �x�N�g������W���s��ŕϊ��������ꍇ�ɁA`VEC3Transform`, `VEC4Transform`, `TransformCoord`
 *     �Ƃ������֐����Ăяo���Ďg�p���܂��B
 *
 * ���ӓ_:
 * - ���̃t�@�C�����̊֐��́A`Vector.h` �� `Matrix.h` �Œ�`���ꂽ�\���̂�A
 *   �ꍇ�ɂ���Ă� `Logger.h` �Ɉˑ����Ă��܂��B�����̃t�@�C����������
 *   �v���W�F�N�g�Ɋ܂܂�Ă���K�v������܂��B
 * - `VEC3Transform` �͕����x�N�g���p�A`TransformCoord` �͍��W�_�p�ƁA
 *   �p�r�ɉ����Ċ֐����g�������邱�Ƃ��d�v�ł��B
 * - �p�[�X�y�N�e�B�u���Z (`TransformCoord` ��) �ł́Aw�������[���ɋ߂��ꍇ��
 *   �[�����Z���������Ȃ��悤�ɒ��ӂ��K�v�ł��i���̃R�[�h�ł̓`�F�b�N����Ă��܂��j�B
 */

 // 4D�x�N�g��(�������W)���������߂̍\����
struct Vector4D {
    float x, y, z, w; // 4�̐���

    // �f�t�H���g�R���X�g���N�^ (�����l��ݒ�)
    Vector4D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
        : x(_x), y(_y), z(_z), w(_w) {
    }
    // �K�v�ł���΁AVector3D ����̕ϊ��R���X�g���N�^�Ȃǂ��ǉ��ł��܂�
    // Vector4D(const Vector3D& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
};


// 3D�x�N�g�����s��ŕϊ�����֐� (��ɕ����x�N�g���p)
// �s��̉�]�E�X�P�[�����O�����i����3x3�j�݂̂�K�p���A���s�ړ������͖������܂��B
inline Vector3D VEC3Transform(const Vector3D& _vec, const Matrix& _mat)
{
    // �f�o�b�O���O�o�͗� (�K�v�Ȃ�R�����g�A�E�g�������Ďg�p)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC3Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << ")";
    // // �K�v�Ȃ�s��̓��e�����O�ɏo��
    // LogDebug(ss_log.str());

    Vector3D result;
    // �s��̍���3x3�����ƃx�N�g���̏�Z
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2];

    // ss_log.str(""); // �X�g���[�����N���A
    // ss_log << "[VEC3Trans] OutVec(" << result.x << "," << result.y << "," << result.z << ")";
    // LogDebug(ss_log.str());

    // ����: �����x�N�g���Ƃ��Ďg���ꍇ�A�ϊ���ɐ��K��(Normalize)���K�v�ȏꍇ������܂�
    return result;
}


// 4D�x�N�g��(�������W)���s��ŕϊ�����֐�
// �x�N�g����4�������ׂĂ��s��ƌv�Z���܂��B
inline Vector4D VEC4Transform(const Vector4D& _vec, const Matrix& _mat)
{
    // �f�o�b�O���O�o�͗� (�K�v�Ȃ�R�����g�A�E�g�������Ďg�p)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC4Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << "," << _vec.w << ")";
    // LogDebug(ss_log.str()); // �s����e�����O�ɏo�͂���ƒ����Ȃ�̂Œ���

    Vector4D result;
    // 4x4�s���4D�x�N�g���̏�Z
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0] + _vec.w * _mat.m[3][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1] + _vec.w * _mat.m[3][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2] + _vec.w * _mat.m[3][2];
    result.w = _vec.x * _mat.m[0][3] + _vec.y * _mat.m[1][3] + _vec.z * _mat.m[2][3] + _vec.w * _mat.m[3][3];

    // ss_log.str(""); // �X�g���[�����N���A
    // ss_log << "[VEC4Trans] OutVec(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
    // LogDebug(ss_log.str());

    return result;
}

// 3D���W�_���s��ŕϊ����A�p�[�X�y�N�e�B�u���Z���s���֐� (��ɍ��W�_�p)
// �����I�ɂ� (x, y, z, 1) ��4D�x�N�g���Ƃ��ĕϊ����A���ʂ�w������ x, y, z ������܂��B
// ����ɂ��A���s�ړ��ⓧ�����e�̌��ʂ��K�p���ꂽ3D���W�������܂��B
inline Vector3D TransformCoord(const Vector3D& _coord, const Matrix& _mat)
{
    Vector4D temp; // (x, y, z, 1) �Ƃ��Čv�Z���邽�߂̈ꎞ�ϐ�

    // 4x4�s��� (x, y, z, 1) �x�N�g���̏�Z�����s
    temp.x = _coord.x * _mat.m[0][0] + _coord.y * _mat.m[1][0] + _coord.z * _mat.m[2][0] + /* 1.0f * */ _mat.m[3][0]; // w=1 ���|����
    temp.y = _coord.x * _mat.m[0][1] + _coord.y * _mat.m[1][1] + _coord.z * _mat.m[2][1] + /* 1.0f * */ _mat.m[3][1];
    temp.z = _coord.x * _mat.m[0][2] + _coord.y * _mat.m[1][2] + _coord.z * _mat.m[2][2] + /* 1.0f * */ _mat.m[3][2];
    temp.w = _coord.x * _mat.m[0][3] + _coord.y * _mat.m[1][3] + _coord.z * _mat.m[2][3] + /* 1.0f * */ _mat.m[3][3];

    Vector3D result = { 0.0f, 0.0f, 0.0f }; // ���ʂ�3D�x�N�g����������

    // �p�[�X�y�N�e�B�u���Z: ���ʂ� w ������ x, y, z ������
    // �������Aw ���[���ɋ߂��ꍇ�̓[�����Z�G���[�ɂȂ邽�߃`�F�b�N���s��
    if (std::fabsf(temp.w) > 1e-6f) { // w �̐�Βl�����ɏ������l���傫���ꍇ�̂݌v�Z
        float invW = 1.0f / temp.w; // ����Z�̑���ɋt�����|����i�v�Z�����̂��߁j
        result.x = temp.x * invW;
        result.y = temp.y * invW;
        result.z = temp.z * invW;
    }
    else {
        // w ���[���ɋ߂��ꍇ�́A�G���[�Ƃ��Ĉ������A���邢�͓���̒l��Ԃ��B
        // �����ł̓[���x�N�g����Ԃ��Ă���B
        // LogWarning("TransformCoord: w component is near zero during perspective divide."); // �K�v�Ȃ�x�����O
    }
    return result;
}

// ���� CameraMath.h �ɂ����� CameraTransform �֐� (�I�C���[�p�x�[�X�̌Â��ϊ�) ��
// �V�����J�������� (�N�H�[�^�j�I�� + �s��) �ł͕s�v�ɂȂ������ߍ폜����Ă��܂��B
/*
static Vector3D CameraTransform(Vector3D& _location, Vector3D& _rotation)
{
    Matrix rotV = GetMatrixAxisX(_rotation.x);
    Matrix rotH = GetMatrixAxisY(_rotation.y);
    Vector3D result = _location;
    result = VECTransform(result, rotV);
    result = VECTransform(result, rotH);
    return result;
}
*/