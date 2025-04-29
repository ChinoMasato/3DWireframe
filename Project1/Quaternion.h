#pragma once
#include <cmath>     // sqrtf, sinf, cosf など (<math.h> より推奨)
#include "Vector.h" // Vector3D を使用
#include "Matrix.h" // Matrix を使用
#include <sstream> // デバッグログ用 (stringstream)
#include <iomanip> // デバッグログ用 (setprecision)
#include "Logger.h" // デバッグログ用 (LogDebug) (必要ならインクルード)

/*
 * Quaternion.h
 * 役割:
 *   3D空間における「回転」を表現し、操作するためのクォータニオン（四元数）クラスを定義します。
 *   オイラー角（XYZ軸周りの回転角度）よりもジンバルロックの問題がなく、回転の合成が容易であるため、
 *   3Dゲームやシミュレーションのカメラ制御、オブジェクトの姿勢制御などで広く利用されます。
 *
 * 主な機能:
 *   - `Quaternion` 構造体: クォータニオンの4つの要素 (x, y, z, w) を保持します。
 *   - 単位クォータニオン (`Identity`) や、軸と角度からの生成 (`FromAxisAngle`) など、
 *     基本的なクォータニオンを作成する関数を提供します。
 *   - クォータニオン同士の乗算 (`operator*`) により、回転を合成できます。
 *   - 長さの計算 (`Length`), 正規化 (`Normalize`), 共役 (`Conjugate`) など、
 *     クォータニオンの基本的な操作を提供します。
 *   - 回転行列への変換 (`ToRotationMatrix`) 機能を提供します。
 *   - (コメントアウトされていますが) ベクトルをクォータニオンで回転させる `RotateVector` 機能。
 *
 * このバージョンでの変更点 (以前の Quaternion.h から):
 * - バグ修正: `ToRotationMatrix` 関数内で一時変数 `yw` の計算式が誤っていたのを修正しました (`qy * qz` -> `qy * qw`)。
 * - デバッグログの追加: `FromAxisAngle`, `ToRotationMatrix` 関数内に、
 *   入力値や計算結果をログファイルに出力するための `LogDebug` 呼び出しが追加されました。
 * - `ToRotationMatrix` での正規化: 回転行列に変換する前に、クォータニオンを内部で正規化する処理が追加されています。
 */

struct Quaternion
{
    float x, y, z, w; // クォータニオンの成分 (x, y, z: ベクトル部, w: スカラー部)

    // デフォルトコンストラクタ (単位クォータニオンで初期化)
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    // 各成分を指定して初期化するコンストラクタ
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    // 静的メソッド: 単位クォータニオン (回転なし) を返す
    static Quaternion Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

    // 静的メソッド: 指定された軸 `axis` 周りに `angle` (ラジアン) 回転するクォータニオンを生成
    static Quaternion FromAxisAngle(const Vector3D& axis, float angle) {
        // --- デバッグログ出力開始 ---
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4); // 小数点以下4桁表示
        ss_log << "[QuatFAA] Input Axis(" << axis.x << "," << axis.y << "," << axis.z
            << ") Angle(" << angle * 180.0f / PI << " deg)"; // PIはCommon.hで定義されている想定

        float halfAngle = angle * 0.5f;
        float s = sinf(halfAngle);
        float c = cosf(halfAngle);
        Vector3D normalizedAxis = axis.Normalized(); // 回転軸は必ず正規化する

        ss_log << " NormAxis(" << normalizedAxis.x << "," << normalizedAxis.y << "," << normalizedAxis.z << ")";

        Quaternion result(normalizedAxis.x * s, normalizedAxis.y * s, normalizedAxis.z * s, c);

        ss_log << " Result(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
        LogDebug(ss_log.str()); // ログメッセージを Logger に書き込む
        // --- デバッグログ出力終了 ---

        return result;
    }

    // クォータニオンの乗算 (回転の合成: result = this * q2)
    Quaternion operator*(const Quaternion& q2) const {
        /* 省略: 元のコードの詳細実装 */
        Quaternion result; result.w = w * q2.w - x * q2.x - y * q2.y - z * q2.z; result.x = w * q2.x + x * q2.w + y * q2.z - z * q2.y; result.y = w * q2.y - x * q2.z + y * q2.w + z * q2.x; result.z = w * q2.z + x * q2.y - y * q2.x + z * q2.w; return result;
    }
    // 乗算して自身に代入 (this = this * q2)
    Quaternion& operator*=(const Quaternion& q2) {
        *this = *this * q2;
        return *this;
    }

    // クォータニオンの長さの2乗を計算
    float LengthSq() const { return x * x + y * y + z * z + w * w; }
    // クォータニオンの長さを計算
    float Length() const { return sqrtf(LengthSq()); }

    // クォータニオンを正規化する (長さを1にする)
    void Normalize() {
        /* 省略: 元のコードの詳細実装 */
        float len = Length(); if (len > 1e-6f) { float invLen = 1.0f / len; x *= invLen; y *= invLen; z *= invLen; w *= invLen; }
        else { *this = Identity(); }
    }
    // 正規化された新しいクォータニオンを返す
    Quaternion Normalized() const { Quaternion result = *this; result.Normalize(); return result; }
    // 共役クォータニオン (ベクトル部の符号を反転) を返す
    Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

    // このクォータニオンが表す回転を4x4の回転行列に変換する
    Matrix ToRotationMatrix() const {
        // --- デバッグログ出力開始 ---
        std::stringstream ss_log;
        ss_log << std::fixed << std::setprecision(4);
        ss_log << "[QuatToMat] Input Quat(" << x << "," << y << "," << z << "," << w << ")";

        Matrix result = Matrix::Identity();
        Quaternion q = this->Normalized(); // 計算前に正規化

        ss_log << " NormQuat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";

        float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
        float xx = qx * qx; float yy = qy * qy; float zz = qz * qz;
        float xy = qx * qy; float xz = qx * qz; float xw = qx * qw;
        // ★★ バグ修正: yw の計算式を修正 ★★
        float yz = qy * qz; float yw = qy * qw; float zw = qz * qw; // 元は yw = qy * qz だった

        // クォータニオンから回転行列への変換公式
        result.m[0][0] = 1.0f - 2.0f * (yy + zz); result.m[0][1] = 2.0f * (xy + zw); result.m[0][2] = 2.0f * (xz - yw);
        result.m[1][0] = 2.0f * (xy - zw); result.m[1][1] = 1.0f - 2.0f * (xx + zz); result.m[1][2] = 2.0f * (yz + xw);
        result.m[2][0] = 2.0f * (xz + yw); result.m[2][1] = 2.0f * (yz - xw); result.m[2][2] = 1.0f - 2.0f * (xx + yy);

        ss_log << " ResultMat[0](" << result.m[0][0] << "," << result.m[0][1] << "," << result.m[0][2] << ")";
        LogDebug(ss_log.str());
        // --- デバッグログ出力終了 ---

        return result;
    }

    // ベクトルをこのクォータニオンで回転させる関数 (コメントアウト中)
    /*
    Vector3D RotateVector(const Vector3D& v) const {
        // 実装例:
        // Quaternion p(v.x, v.y, v.z, 0.0f);
        // Quaternion q_inv = this->Conjugate();
        // Quaternion rotated_p = (*this) * p * q_inv;
        // return Vector3D(rotated_p.x, rotated_p.y, rotated_p.z);
    }
    */
};