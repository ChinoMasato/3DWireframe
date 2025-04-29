#pragma once
#include <cmath> // sqrtf �Ȃ� (<math.h> ��萄��)

/*
 * Vector.h
 * ����:
 *   3D��Ԃɂ�����x�N�g���i�ʒu������Ȃǁj��\�����߂� `Vector3D` �\���̂ƁA
 *   �x�N�g�����m�̊�{�I�Ȍv�Z�i���Z�A���Z�A�X�J���[�{�Ȃǁj���s�����߂�
 *   ���Z�q�I�[�o�[���[�h��񋟂��܂��B
 *   �܂��A�x�N�g���̒����v�Z�␳�K���Ƃ������A�悭�g���鑀��̂��߂�
 *   �����o�֐�����`����Ă��܂��B
 *
 * ���̃o�[�W�����ł̕ύX�_ (���� Vector.h ����):
 * - �R���X�g���N�^�̒ǉ�: �f�t�H���g�l�t���̃R���X�g���N�^��ǉ����A
 *   `Vector3D vec;` �� `Vector3D pos(1, 2, 3);` �̂悤�ɊȒP�ɏ������ł���悤�ɂ��܂����B
 * - ���Z�q�� const ���m������: `operator+`, `operator-`, `operator*` �� `const` ��ǉ����A
 *   �����̉��Z���I�u�W�F�N�g�̏�Ԃ�ύX���Ȃ����Ƃ𖾎����܂����B
 * - LengthSq() �֐��̒ǉ�: �x�N�g���̒�����2����v�Z����֐���ǉ����܂����B
 *   �����̑召��r�����Ȃ�A������(`sqrtf`)�̌v�Z���s�v�Ȃ�����̕��������ł��B
 * - Length() �֐��̎����ύX: `LengthSq()` �𗘗p���Ē������v�Z����悤�ɂ��܂����B
 * - Normalize() �֐��̉��P: �[�����Z�����m���ɔ����邽�߁A������2�� (`LengthSq`) ���g����
 *   �[���ɋ߂����ǂ����𔻒肷��悤�ɂ��A臒l (`1e-12f`) ���g�p���܂����B
 *   �������[���ɋ߂��ꍇ�̓[���x�N�g���ɂ���悤�ɕύX���܂����B
 * - Normalized() �֐��̒ǉ�: ���̃x�N�g����ύX�����ɁA���K�����ꂽ�V�����x�N�g����
 *   �Ԃ��֐���ǉ����܂����B
 * - �X�J���[�{�̍������Z�q: `2.0f * vec` �̂悤�ȋL�q���\�ɂ��鉉�Z�q��ǉ����܂����B
 *
 * �g����:
 *   - `#include "Vector.h"` ���C���N���[�h���܂��B
 *   - `Vector3D position(10.0f, 0.0f, 5.0f);` �̂悤�ɕϐ���錾�E���������܂��B
 *   - `Vector3D velocity = vec1 + vec2;` �̂悤�Ƀx�N�g�����Z���s���܂��B
 *   - `float speed = velocity.Length();` �̂悤�ɒ������擾���܂��B
 *   - `Vector3D direction = velocity.Normalized();` �̂悤�ɐ��K���i�����x�N�g�����j���܂��B
 *   - `velocity.Normalize();` �̂悤�Ƀx�N�g�����g�𐳋K�����܂��B
 */

struct Vector3D
{
    float x, y, z; // �x�N�g����3�̐���

    // �R���X�g���N�^: �I�u�W�F�N�g�������ɒl��������
    // �������ȗ������ꍇ�� 0.0f �ŏ����������
    Vector3D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    // --- ���Z�q�̃I�[�o�[���[�h ---
    // �x�N�g�����m�̉��Z (v1 + v2)
    Vector3D operator+(const Vector3D& _vec) const { return { x + _vec.x, y + _vec.y, z + _vec.z }; }
    // �x�N�g�������Z���Ď��g�ɑ�� (v1 += v2)
    void operator+=(const Vector3D& _vec) { x += _vec.x; y += _vec.y; z += _vec.z; }
    // �x�N�g�����m�̌��Z (v1 - v2)
    Vector3D operator-(const Vector3D& _vec) const { return { x - _vec.x, y - _vec.y, z - _vec.z }; }
    // �x�N�g�������Z���Ď��g�ɑ�� (v1 -= v2)
    void operator-=(const Vector3D& _vec) { x -= _vec.x; y -= _vec.y; z -= _vec.z; }
    // �x�N�g���ƃX�J���[�l�̏�Z (vec * scale)
    Vector3D operator*(float _scale) const { return { x * _scale, y * _scale, z * _scale }; }
    // �x�N�g���ɃX�J���[�l����Z���Ď��g�ɑ�� (vec *= scale)
    void operator*=(float _scale) { x *= _scale; y *= _scale; z *= _scale; }
    // (����Z���Z�q operator/ ���K�v�Ȃ�ǉ��\)

    // --- �����o�֐� ---
    // �x�N�g���̒����i�傫���A�m�����j��2����v�Z����
    // �������̌v�Z�̓R�X�g���������߁A�P���ȑ召��r�Ȃǂł͂�������g���ƌ������ǂ�
    float LengthSq() const {
        return x * x + y * y + z * z;
    }

    // �x�N�g���̒����i�傫���A�m�����j���v�Z����
    float Length() const {
        return sqrtf(LengthSq()); // LengthSq()�̌��ʂ̕��������v�Z
    }

    // ���̃x�N�g�����g�𐳋K���i������1�Ɂj����
    // �����x�N�g���Ƃ��Ĉ��������ꍇ�ȂǂɎg�p����
    void Normalize() {
        float lenSq = LengthSq(); // �܂�������2����v�Z
        // ������2�悪���ɏ������l���傫���ꍇ�̂ݐ��K���������s�� (�[�����Z���)
        // 1e-12f �� 0.000000000001 (10��-12��)
        if (lenSq > 1e-12f) {
            float len = sqrtf(lenSq); // �������v�Z
            float invLen = 1.0f / len; // ����Z�̑���ɋt�����|����
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
        else {
            // �������قڃ[���̏ꍇ�́A���K���ł��Ȃ��̂Ń[���x�N�g���ɂ���
            x = y = z = 0.0f;
        }
    }

    // ���K�����ꂽ�V�����x�N�g���𐶐����ĕԂ� (���̃x�N�g���͕ύX���Ȃ�)
    // ��: Vector3D direction = velocity.Normalized();
    Vector3D Normalized() const {
        Vector3D result = *this; // �������g�̃R�s�[���쐬
        result.Normalize();      // �R�s�[�𐳋K��
        return result;           // ���K�����ꂽ�R�s�[��Ԃ�
    }
    // (���� Dot() �� �O�� Cross() �̊֐����K�v�ɉ����Ēǉ�����ƕ֗�)
    // static float Dot(const Vector3D& a, const Vector3D& b) { ... }
    // static Vector3D Cross(const Vector3D& a, const Vector3D& b) { ... }
};

// �O���[�o���֐�: 3��float�l����Vector3D���쐬���� (�R���X�g���N�^������̂ŕK�{�ł͂Ȃ�)
// _inline �̓C�����C���W�J���R���p�C���ɒ�Ă���w���q (����̃R���p�C���ł͎����Ŕ��f���邱�Ƃ�����)
inline Vector3D SetVec3D(float _x, float _y, float _z) { return { _x, _y, _z }; }

// �O���[�o���֐�: �X�J���[�l�ƃx�N�g���̏�Z (scale * vec �̏������\�ɂ���)
inline Vector3D operator*(float _scale, const Vector3D& _vec) {
    return _vec * _scale; // ������ Vector3D::operator* ���Ăяo��
}