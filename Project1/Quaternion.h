#pragma once
#include <math.h>
#include "Vector.h"
#include "Matrix.h"
#include <cmath>
#include <sstream> // デバッグログ用
#include <iomanip> // デバッグログ用
#include "Logger.h" // デバッグログ用

/*
 * Quaternion.h
 * 修正点:
 * - FromAxisAngle, ToRotationMatrix, RotateVector に詳細なデバッグログを追加。
 */

struct Quaternion
{
    float x, y, z, w;
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    static Quaternion Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

    static Quaternion FromAxisAngle(const Vector3D& axis, float angle) {
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4);
        ss_log << "[QuatFAA] Input Axis(" << axis.x << "," << axis.y << "," << axis.z << ") Angle(" << angle * 180 / PI << ")";

        float halfAngle = angle * 0.5f;
        float s = sinf(halfAngle);
        float c = cosf(halfAngle);
        Vector3D normalizedAxis = axis.Normalized(); // 正規化必須

        ss_log << " NormAxis(" << normalizedAxis.x << "," << normalizedAxis.y << "," << normalizedAxis.z << ")";

        Quaternion result(normalizedAxis.x * s, normalizedAxis.y * s, normalizedAxis.z * s, c);

        ss_log << " Result(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
        LogDebug(ss_log.str());

        return result;
    }

    Quaternion operator*(const Quaternion& q2) const { /* ... 省略 ... */
        Quaternion result; result.w = w * q2.w - x * q2.x - y * q2.y - z * q2.z; result.x = w * q2.x + x * q2.w + y * q2.z - z * q2.y; result.y = w * q2.y - x * q2.z + y * q2.w + z * q2.x; result.z = w * q2.z + x * q2.y - y * q2.x + z * q2.w; return result;
    }
    Quaternion& operator*=(const Quaternion& q2) { *this = *this * q2; return *this; }

    float LengthSq() const { return x * x + y * y + z * z + w * w; }
    float Length() const { return sqrtf(LengthSq()); }

    void Normalize() { /* ... 省略 ... */
        float len = Length(); if (len > 1e-6f) { float invLen = 1.0f / len; x *= invLen; y *= invLen; z *= invLen; w *= invLen; }
        else { *this = Identity(); }
    }
    Quaternion Normalized() const { Quaternion result = *this; result.Normalize(); return result; }
    Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

    Matrix ToRotationMatrix() const {
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4);
        ss_log << "[QuatToMat] Input Quat(" << x << "," << y << "," << z << "," << w << ")";

        Matrix result = Matrix::Identity();
        Quaternion q = this->Normalized(); // 計算前に正規化

        ss_log << " NormQuat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";

        float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
        float xx = qx * qx; float yy = qy * qy; float zz = qz * qz;
        float xy = qx * qy; float xz = qx * qz; float xw = qx * qw;
        float yz = qy * qz; float yw = qy * qw; float zw = qz * qw;

        result.m[0][0] = 1.0f - 2.0f * (yy + zz); result.m[0][1] = 2.0f * (xy + zw); result.m[0][2] = 2.0f * (xz - yw);
        result.m[1][0] = 2.0f * (xy - zw); result.m[1][1] = 1.0f - 2.0f * (xx + zz); result.m[1][2] = 2.0f * (yz + xw);
        result.m[2][0] = 2.0f * (xz + yw); result.m[2][1] = 2.0f * (yz - xw); result.m[2][2] = 1.0f - 2.0f * (xx + yy);

        ss_log << " ResultMat[0](" << result.m[0][0] << "," << result.m[0][1] << "," << result.m[0][2] << ")";
        LogDebug(ss_log.str());

        return result;
    }

    // コメントアウト中
    /*
    Vector3D RotateVector(const Vector3D& v) const {
        // ...
    }
    */
};