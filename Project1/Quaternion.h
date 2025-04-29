#pragma once
#include <cmath>     // sqrtf, sinf, cosf �Ȃ� (<math.h> ��萄��)
#include "Vector.h" // Vector3D ���g�p
#include "Matrix.h" // Matrix ���g�p
#include <sstream> // �f�o�b�O���O�p (stringstream)
#include <iomanip> // �f�o�b�O���O�p (setprecision)
#include "Logger.h" // �f�o�b�O���O�p (LogDebug) (�K�v�Ȃ�C���N���[�h)

/*
 * Quaternion.h
 * ����:
 *   3D��Ԃɂ�����u��]�v��\�����A���삷�邽�߂̃N�H�[�^�j�I���i�l�����j�N���X���`���܂��B
 *   �I�C���[�p�iXYZ������̉�]�p�x�j�����W���o�����b�N�̖�肪�Ȃ��A��]�̍������e�Ղł��邽�߁A
 *   3D�Q�[����V�~�����[�V�����̃J��������A�I�u�W�F�N�g�̎p������ȂǂōL�����p����܂��B
 *
 * ��ȋ@�\:
 *   - `Quaternion` �\����: �N�H�[�^�j�I����4�̗v�f (x, y, z, w) ��ێ����܂��B
 *   - �P�ʃN�H�[�^�j�I�� (`Identity`) ��A���Ɗp�x����̐��� (`FromAxisAngle`) �ȂǁA
 *     ��{�I�ȃN�H�[�^�j�I�����쐬����֐���񋟂��܂��B
 *   - �N�H�[�^�j�I�����m�̏�Z (`operator*`) �ɂ��A��]�������ł��܂��B
 *   - �����̌v�Z (`Length`), ���K�� (`Normalize`), ���� (`Conjugate`) �ȂǁA
 *     �N�H�[�^�j�I���̊�{�I�ȑ����񋟂��܂��B
 *   - ��]�s��ւ̕ϊ� (`ToRotationMatrix`) �@�\��񋟂��܂��B
 *   - (�R�����g�A�E�g����Ă��܂���) �x�N�g�����N�H�[�^�j�I���ŉ�]������ `RotateVector` �@�\�B
 *
 * ���̃o�[�W�����ł̕ύX�_ (�ȑO�� Quaternion.h ����):
 * - �o�O�C��: `ToRotationMatrix` �֐����ňꎞ�ϐ� `yw` �̌v�Z��������Ă����̂��C�����܂��� (`qy * qz` -> `qy * qw`)�B
 * - �f�o�b�O���O�̒ǉ�: `FromAxisAngle`, `ToRotationMatrix` �֐����ɁA
 *   ���͒l��v�Z���ʂ����O�t�@�C���ɏo�͂��邽�߂� `LogDebug` �Ăяo�����ǉ�����܂����B
 * - `ToRotationMatrix` �ł̐��K��: ��]�s��ɕϊ�����O�ɁA�N�H�[�^�j�I��������Ő��K�����鏈�����ǉ�����Ă��܂��B
 */

struct Quaternion
{
    float x, y, z, w; // �N�H�[�^�j�I���̐��� (x, y, z: �x�N�g����, w: �X�J���[��)

    // �f�t�H���g�R���X�g���N�^ (�P�ʃN�H�[�^�j�I���ŏ�����)
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    // �e�������w�肵�ď���������R���X�g���N�^
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    // �ÓI���\�b�h: �P�ʃN�H�[�^�j�I�� (��]�Ȃ�) ��Ԃ�
    static Quaternion Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

    // �ÓI���\�b�h: �w�肳�ꂽ�� `axis` ����� `angle` (���W�A��) ��]����N�H�[�^�j�I���𐶐�
    static Quaternion FromAxisAngle(const Vector3D& axis, float angle) {
        // --- �f�o�b�O���O�o�͊J�n ---
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4); // �����_�ȉ�4���\��
        ss_log << "[QuatFAA] Input Axis(" << axis.x << "," << axis.y << "," << axis.z
            << ") Angle(" << angle * 180.0f / PI << " deg)"; // PI��Common.h�Œ�`����Ă���z��

        float halfAngle = angle * 0.5f;
        float s = sinf(halfAngle);
        float c = cosf(halfAngle);
        Vector3D normalizedAxis = axis.Normalized(); // ��]���͕K�����K������

        ss_log << " NormAxis(" << normalizedAxis.x << "," << normalizedAxis.y << "," << normalizedAxis.z << ")";

        Quaternion result(normalizedAxis.x * s, normalizedAxis.y * s, normalizedAxis.z * s, c);

        ss_log << " Result(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
        LogDebug(ss_log.str()); // ���O���b�Z�[�W�� Logger �ɏ�������
        // --- �f�o�b�O���O�o�͏I�� ---

        return result;
    }

    // �N�H�[�^�j�I���̏�Z (��]�̍���: result = this * q2)
    Quaternion operator*(const Quaternion& q2) const {
        /* �ȗ�: ���̃R�[�h�̏ڍ׎��� */
        Quaternion result; result.w = w * q2.w - x * q2.x - y * q2.y - z * q2.z; result.x = w * q2.x + x * q2.w + y * q2.z - z * q2.y; result.y = w * q2.y - x * q2.z + y * q2.w + z * q2.x; result.z = w * q2.z + x * q2.y - y * q2.x + z * q2.w; return result;
    }
    // ��Z���Ď��g�ɑ�� (this = this * q2)
    Quaternion& operator*=(const Quaternion& q2) {
        *this = *this * q2;
        return *this;
    }

    // �N�H�[�^�j�I���̒�����2����v�Z
    float LengthSq() const { return x * x + y * y + z * z + w * w; }
    // �N�H�[�^�j�I���̒������v�Z
    float Length() const { return sqrtf(LengthSq()); }

    // �N�H�[�^�j�I���𐳋K������ (������1�ɂ���)
    void Normalize() {
        /* �ȗ�: ���̃R�[�h�̏ڍ׎��� */
        float len = Length(); if (len > 1e-6f) { float invLen = 1.0f / len; x *= invLen; y *= invLen; z *= invLen; w *= invLen; }
        else { *this = Identity(); }
    }
    // ���K�����ꂽ�V�����N�H�[�^�j�I����Ԃ�
    Quaternion Normalized() const { Quaternion result = *this; result.Normalize(); return result; }
    // �����N�H�[�^�j�I�� (�x�N�g�����̕����𔽓]) ��Ԃ�
    Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

    // ���̃N�H�[�^�j�I�����\����]��4x4�̉�]�s��ɕϊ�����
    Matrix ToRotationMatrix() const {
        // --- �f�o�b�O���O�o�͊J�n ---
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4);
        ss_log << "[QuatToMat] Input Quat(" << x << "," << y << "," << z << "," << w << ")";

        Matrix result = Matrix::Identity();
        Quaternion q = this->Normalized(); // �v�Z�O�ɐ��K��

        ss_log << " NormQuat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";

        float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
        float xx = qx * qx; float yy = qy * qy; float zz = qz * qz;
        float xy = qx * qy; float xz = qx * qz; float xw = qx * qw;
        // ���� �o�O�C��: yw �̌v�Z�����C�� ����
        float yz = qy * qz; float yw = qy * qw; float zw = qz * qw; // ���� yw = qy * qz ������

        // �N�H�[�^�j�I�������]�s��ւ̕ϊ�����
        result.m[0][0] = 1.0f - 2.0f * (yy + zz); result.m[0][1] = 2.0f * (xy + zw); result.m[0][2] = 2.0f * (xz - yw);
        result.m[1][0] = 2.0f * (xy - zw); result.m[1][1] = 1.0f - 2.0f * (xx + zz); result.m[1][2] = 2.0f * (yz + xw);
        result.m[2][0] = 2.0f * (xz + yw); result.m[2][1] = 2.0f * (yz - xw); result.m[2][2] = 1.0f - 2.0f * (xx + yy);

        ss_log << " ResultMat[0](" << result.m[0][0] << "," << result.m[0][1] << "," << result.m[0][2] << ")";
        LogDebug(ss_log.str());
        // --- �f�o�b�O���O�o�͏I�� ---

        return result;
    }

    // �x�N�g�������̃N�H�[�^�j�I���ŉ�]������֐� (�R�����g�A�E�g��)
    /*
    Vector3D RotateVector(const Vector3D& v) const {
        // ������:
        // Quaternion p(v.x, v.y, v.z, 0.0f);
        // Quaternion q_inv = this->Conjugate();
        // Quaternion rotated_p = (*this) * p * q_inv;
        // return Vector3D(rotated_p.x, rotated_p.y, rotated_p.z);
    }
    */
};