#pragma once
#include <math.h> // sqrtf を使用するため

/*
 * Vector.h
 * 役割: 3次元ベクトル(Vector3D)の構造体と関連演算子を提供します。
 *
 * 変更点:
 * - LengthSq() : ベクトルの長さの2乗を計算するメンバー関数を追加。
 * - (前回から) Length(), Normalize(), Normalized() を追加済み。
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

    // ベクトルの長さの2乗を計算 (sqrtを呼ばないので高速)
    float LengthSq() const {
        return x * x + y * y + z * z;
    }

    // ベクトルの長さを計算
    float Length() const {
        return sqrtf(LengthSq()); // LengthSqを利用
    }

    // ベクトル自身を正規化
    void Normalize() {
        float lenSq = LengthSq(); // 長さの2乗で比較
        if (lenSq > 1e-12f) { // 非常に小さい値でチェック (ゼロ除算回避)
            float len = sqrtf(lenSq);
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
        else {
            x = y = z = 0.0f; // ゼロベクトルにする
        }
    }

    // 正規化された新しいベクトルを返す
    Vector3D Normalized() const {
        Vector3D result = *this;
        result.Normalize();
        return result;
    }
};

inline Vector3D SetVec3D(float _x, float _y, float _z) { return { _x, _y, _z }; }
inline Vector3D operator*(float _scale, const Vector3D& _vec) { return _vec * _scale; }