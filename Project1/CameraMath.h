#pragma once
#include <math.h>
#include "Vector.h" // Vector3D ���g�p
#include "Matrix.h" // Matrix ���g�p
#include <sstream> // �f�o�b�O���O�p
#include <iomanip> // �f�o�b�O���O�p
#include "Logger.h" // �f�o�b�O���O�p (�K�v�Ȃ�)

/*
 * CameraMath.h
 * ����: �J�����֘A�̐��w�֐���񋟂��܂��B
 *
 * �ύX�_:
 * - Vector4D �\���̂�ǉ��B
 * - VEC4Transform �֐��𕜊��E�������A�f�o�b�O���O��ǉ��B
 * - TransformCoord �֐��𕜊��E�����B
 * - VEC3Transform �͕ύX�Ȃ��B
 */

 // 4D�x�N�g��(�������W)
struct Vector4D {
    float x, y, z, w;

    // �f�t�H���g�R���X�g���N�^
    Vector4D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
        : x(_x), y(_y), z(_z), w(_w) {
    }
};


// 3D�x�N�g�����s��ŕϊ� (��]�E�X�P�[�����O�̂�)
inline Vector3D VEC3Transform(const Vector3D& _vec, const Matrix& _mat)
{
    // �f�o�b�O���O��Quaternion.h���ɂ�����̂ŁA������͏ȗ����Ă��ǂ�����
    // std::stringstream ss_log; ss_log << std::fixed << std::setprecision(4); /* ... ���O�o�� ... */ LogDebug(ss_log.str());

    Vector3D result;
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2];
    return result;
}


// 4D�x�N�g��(�������W)���s��ŕϊ�
inline Vector4D VEC4Transform(const Vector4D& _vec, const Matrix& _mat)
{
    // �f�o�b�O���O (�K�v�ɉ����ăR�����g�A�E�g����)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC4Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << "," << _vec.w << ")";
    // LogDebug(ss_log.str()); // �s����e���o�͂���ƒ����Ȃ�̂ŏȗ�

    Vector4D result;
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0] + _vec.w * _mat.m[3][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1] + _vec.w * _mat.m[3][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2] + _vec.w * _mat.m[3][2];
    result.w = _vec.x * _mat.m[0][3] + _vec.y * _mat.m[1][3] + _vec.z * _mat.m[2][3] + _vec.w * _mat.m[3][3];

    // ss_log.str(""); // Clear stream
    // ss_log << "[VEC4Trans] OutVec(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
    // LogDebug(ss_log.str());

    return result;
}

// 3D���W���s��ŕϊ����AW�ŏ��Z����3D���W�ɖ߂� (�A�t�B���ϊ� + �����ϊ�)
inline Vector3D TransformCoord(const Vector3D& _coord, const Matrix& _mat)
{
    Vector4D temp; // w=1�Ƃ��Čv�Z
    temp.x = _coord.x * _mat.m[0][0] + _coord.y * _mat.m[1][0] + _coord.z * _mat.m[2][0] + 1.0f * _mat.m[3][0];
    temp.y = _coord.x * _mat.m[0][1] + _coord.y * _mat.m[1][1] + _coord.z * _mat.m[2][1] + 1.0f * _mat.m[3][1];
    temp.z = _coord.x * _mat.m[0][2] + _coord.y * _mat.m[1][2] + _coord.z * _mat.m[2][2] + 1.0f * _mat.m[3][2];
    temp.w = _coord.x * _mat.m[0][3] + _coord.y * _mat.m[1][3] + _coord.z * _mat.m[2][3] + 1.0f * _mat.m[3][3];

    Vector3D result = { 0, 0, 0 };
    if (fabsf(temp.w) > 1e-6f) { // �[�����Z���
        float invW = 1.0f / temp.w;
        result.x = temp.x * invW;
        result.y = temp.y * invW;
        result.z = temp.z * invW;
    }
    // W���[���ɋ߂��ꍇ�̓[���x�N�g����Ԃ��i�܂��̓G���[�����j
    return result;
}