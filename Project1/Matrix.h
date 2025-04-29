#pragma once
#include <math.h>
#include <stdexcept> // invalid_argument

/*
 * Matrix.h
 * ����: 4x4�s��\���̂ƁA��]�s��A�v���W�F�N�V�����s��𐶐�����֐���񋟂��܂��B
 *
 * �ύX�_:
 * - GetMatrixAxisX, GetMatrixAxisY ��������W�n(LH)�̒�`�ɏC���B
 * - (�O�񂩂�) PerspectiveFovLH, MatrixMultiply, Identity ��ǉ��B
 */

struct Matrix
{
    float m[4][4];
    Matrix() { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] = 0.0f; }
    static Matrix Identity() {
        Matrix result;
        result.m[0][0] = result.m[1][1] = result.m[2][2] = result.m[3][3] = 1.0f;
        return result;
    }
};

// X������̉�]�s�� (������W�n / LH)
// �����v��肪��
static Matrix GetMatrixAxisXLH(float XAxisRotation)
{
    float sin_val = sinf(XAxisRotation);
    float cos_val = cosf(XAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[1][1] = cos_val; result.m[1][2] = -sin_val; // LH
    result.m[2][1] = sin_val; result.m[2][2] = cos_val;  // LH
    return result;
}

// Y������̉�]�s�� (������W�n / LH)
// �����v��肪��
static Matrix GetMatrixAxisYLH(float YAxisRotation)
{
    float sin_val = sinf(YAxisRotation);
    float cos_val = cosf(YAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[0][0] = cos_val; result.m[0][2] = sin_val;  // LH
    result.m[2][0] = -sin_val; result.m[2][2] = cos_val; // LH
    return result;
}

// Z������̉�]�s�� (������W�n / LH) - �Q�l
static Matrix GetMatrixAxisZLH(float ZAxisRotation)
{
    float sin_val = sinf(ZAxisRotation);
    float cos_val = cosf(ZAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[0][0] = cos_val; result.m[0][1] = -sin_val; // LH
    result.m[1][0] = sin_val; result.m[1][1] = cos_val;  // LH
    return result;
}


// �s��̏�Z C = A * B
static Matrix MatrixMultiply(const Matrix& a, const Matrix& b)
{
    Matrix c;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            c.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                c.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return c;
}


// ������W�n�A����p�x�[�X�̓������e�s����쐬
static Matrix PerspectiveFovLH(float fovY, float aspectRatio, float zn, float zf)
{
    if (aspectRatio <= 0.0f || zn <= 0.0f || zf <= zn) {
        throw std::invalid_argument("Invalid arguments for PerspectiveFovLH");
    }
    Matrix result;
    float yScale = 1.0f / tanf(fovY / 2.0f);
    float xScale = yScale / aspectRatio;
    float zRange = zf / (zf - zn); // LH
    result.m[0][0] = xScale;
    result.m[1][1] = yScale;
    result.m[2][2] = zRange;
    result.m[2][3] = 1.0f; // LH
    result.m[3][2] = -zn * zRange; // LH
    return result;
}