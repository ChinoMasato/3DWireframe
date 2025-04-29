#pragma once
#include <math.h>
#include <stdexcept> // invalid_argument

/*
 * Matrix.h
 * 役割: 4x4行列構造体と、回転行列、プロジェクション行列を生成する関数を提供します。
 *
 * 変更点:
 * - GetMatrixAxisX, GetMatrixAxisY を左手座標系(LH)の定義に修正。
 * - (前回から) PerspectiveFovLH, MatrixMultiply, Identity を追加。
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

// X軸周りの回転行列 (左手座標系 / LH)
// 反時計回りが正
static Matrix GetMatrixAxisXLH(float XAxisRotation)
{
    float sin_val = sinf(XAxisRotation);
    float cos_val = cosf(XAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[1][1] = cos_val; result.m[1][2] = -sin_val; // LH
    result.m[2][1] = sin_val; result.m[2][2] = cos_val;  // LH
    return result;
}

// Y軸周りの回転行列 (左手座標系 / LH)
// 反時計回りが正
static Matrix GetMatrixAxisYLH(float YAxisRotation)
{
    float sin_val = sinf(YAxisRotation);
    float cos_val = cosf(YAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[0][0] = cos_val; result.m[0][2] = sin_val;  // LH
    result.m[2][0] = -sin_val; result.m[2][2] = cos_val; // LH
    return result;
}

// Z軸周りの回転行列 (左手座標系 / LH) - 参考
static Matrix GetMatrixAxisZLH(float ZAxisRotation)
{
    float sin_val = sinf(ZAxisRotation);
    float cos_val = cosf(ZAxisRotation);
    Matrix result = Matrix::Identity();
    result.m[0][0] = cos_val; result.m[0][1] = -sin_val; // LH
    result.m[1][0] = sin_val; result.m[1][1] = cos_val;  // LH
    return result;
}


// 行列の乗算 C = A * B
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


// 左手座標系、視野角ベースの透視投影行列を作成
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