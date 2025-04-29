#pragma once
#include <math.h> // sqrtf ���g�p���邽��

/*
 * Vector.h
 * ����: 3�����x�N�g��(Vector3D)�̍\���̂Ɗ֘A���Z�q��񋟂��܂��B
 *
 * �ύX�_:
 * - LengthSq() : �x�N�g���̒�����2����v�Z���郁���o�[�֐���ǉ��B
 * - (�O�񂩂�) Length(), Normalize(), Normalized() ��ǉ��ς݁B
 */

struct Vector3D
{
    float x, y, z;

    Vector3D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    Vector3D operator+(const Vector3D& _vec) const { return { x + _vec.x, y + _vec.y, z + _vec.z }; }
    void operator+=(const Vector3D& _vec) { x += _vec.x; y += _vec.y; z += _vec.z; }
    Vector3D operator-(const Vector3D& _vec) const { return { x - _vec.x, y - _vec.y, z - _vec.z }; }
    void operator-=(const Vector3D& _vec) { x -= _vec.x; y -= _vec.y; z -= _vec.z; }
    Vector3D operator*(float _scale) const { return { x * _scale, y * _scale, z * _scale }; }
    void operator*=(float _scale) { x *= _scale; y *= _scale; z *= _scale; }

    // �x�N�g���̒�����2����v�Z (sqrt���Ă΂Ȃ��̂ō���)
    float LengthSq() const {
        return x * x + y * y + z * z;
    }

    // �x�N�g���̒������v�Z
    float Length() const {
        return sqrtf(LengthSq()); // LengthSq�𗘗p
    }

    // �x�N�g�����g�𐳋K��
    void Normalize() {
        float lenSq = LengthSq(); // ������2��Ŕ�r
        if (lenSq > 1e-12f) { // ���ɏ������l�Ń`�F�b�N (�[�����Z���)
            float len = sqrtf(lenSq);
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
        else {
            x = y = z = 0.0f; // �[���x�N�g���ɂ���
        }
    }

    // ���K�����ꂽ�V�����x�N�g����Ԃ�
    Vector3D Normalized() const {
        Vector3D result = *this;
        result.Normalize();
        return result;
    }
};

inline Vector3D SetVec3D(float _x, float _y, float _z) { return { _x, _y, _z }; }
inline Vector3D operator*(float _scale, const Vector3D& _vec) { return _vec * _scale; }