#pragma once
#include <math.h>
#include "Vector.h" // Vector3D を使用
#include "Matrix.h" // Matrix を使用
#include <sstream> // デバッグログ用
#include <iomanip> // デバッグログ用
#include "Logger.h" // デバッグログ用 (必要なら)

/*
 * CameraMath.h
 * 役割: カメラ関連の数学関数を提供します。
 *
 * 変更点:
 * - Vector4D 構造体を追加。
 * - VEC4Transform 関数を復活・実装し、デバッグログを追加。
 * - TransformCoord 関数を復活・実装。
 * - VEC3Transform は変更なし。
 */

 // 4Dベクトル(同次座標)
struct Vector4D {
    float x, y, z, w;

    // デフォルトコンストラクタ
    Vector4D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
        : x(_x), y(_y), z(_z), w(_w) {
    }
};


// 3Dベクトルを行列で変換 (回転・スケーリングのみ)
inline Vector3D VEC3Transform(const Vector3D& _vec, const Matrix& _mat)
{
    // デバッグログはQuaternion.h側にもあるので、こちらは省略しても良いかも
    // std::stringstream ss_log; ss_log << std::fixed << std::setprecision(4); /* ... ログ出力 ... */ LogDebug(ss_log.str());

    Vector3D result;
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2];
    return result;
}


// 4Dベクトル(同次座標)を行列で変換
inline Vector4D VEC4Transform(const Vector4D& _vec, const Matrix& _mat)
{
    // デバッグログ (必要に応じてコメントアウト解除)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC4Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << "," << _vec.w << ")";
    // LogDebug(ss_log.str()); // 行列内容も出力すると長くなるので省略

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

// 3D座標を行列で変換し、Wで除算して3D座標に戻す (アフィン変換 + 透視変換)
inline Vector3D TransformCoord(const Vector3D& _coord, const Matrix& _mat)
{
    Vector4D temp; // w=1として計算
    temp.x = _coord.x * _mat.m[0][0] + _coord.y * _mat.m[1][0] + _coord.z * _mat.m[2][0] + 1.0f * _mat.m[3][0];
    temp.y = _coord.x * _mat.m[0][1] + _coord.y * _mat.m[1][1] + _coord.z * _mat.m[2][1] + 1.0f * _mat.m[3][1];
    temp.z = _coord.x * _mat.m[0][2] + _coord.y * _mat.m[1][2] + _coord.z * _mat.m[2][2] + 1.0f * _mat.m[3][2];
    temp.w = _coord.x * _mat.m[0][3] + _coord.y * _mat.m[1][3] + _coord.z * _mat.m[2][3] + 1.0f * _mat.m[3][3];

    Vector3D result = { 0, 0, 0 };
    if (fabsf(temp.w) > 1e-6f) { // ゼロ除算回避
        float invW = 1.0f / temp.w;
        result.x = temp.x * invW;
        result.y = temp.y * invW;
        result.z = temp.z * invW;
    }
    // Wがゼロに近い場合はゼロベクトルを返す（またはエラー処理）
    return result;
}