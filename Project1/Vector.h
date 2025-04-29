#pragma once
#include <cmath> // sqrtf など (<math.h> より推奨)

/*
 * Vector.h
 * 役割:
 *   3D空間におけるベクトル（位置や方向など）を表すための `Vector3D` 構造体と、
 *   ベクトル同士の基本的な計算（加算、減算、スカラー倍など）を行うための
 *   演算子オーバーロードを提供します。
 *   また、ベクトルの長さ計算や正規化といった、よく使われる操作のための
 *   メンバ関数も定義されています。
 *
 * このバージョンでの変更点 (元の Vector.h から):
 * - コンストラクタの追加: デフォルト値付きのコンストラクタを追加し、
 *   `Vector3D vec;` や `Vector3D pos(1, 2, 3);` のように簡単に初期化できるようにしました。
 * - 演算子の const 正確性向上: `operator+`, `operator-`, `operator*` に `const` を追加し、
 *   これらの演算がオブジェクトの状態を変更しないことを明示しました。
 * - LengthSq() 関数の追加: ベクトルの長さの2乗を計算する関数を追加しました。
 *   長さの大小比較だけなら、平方根(`sqrtf`)の計算が不要なこちらの方が高速です。
 * - Length() 関数の実装変更: `LengthSq()` を利用して長さを計算するようにしました。
 * - Normalize() 関数の改善: ゼロ除算をより確実に避けるため、長さの2乗 (`LengthSq`) を使って
 *   ゼロに近いかどうかを判定するようにし、閾値 (`1e-12f`) を使用しました。
 *   長さがゼロに近い場合はゼロベクトルにするように変更しました。
 * - Normalized() 関数の追加: 元のベクトルを変更せずに、正規化された新しいベクトルを
 *   返す関数を追加しました。
 * - スカラー倍の左側演算子: `2.0f * vec` のような記述を可能にする演算子を追加しました。
 *
 * 使い方:
 *   - `#include "Vector.h"` をインクルードします。
 *   - `Vector3D position(10.0f, 0.0f, 5.0f);` のように変数を宣言・初期化します。
 *   - `Vector3D velocity = vec1 + vec2;` のようにベクトル演算を行います。
 *   - `float speed = velocity.Length();` のように長さを取得します。
 *   - `Vector3D direction = velocity.Normalized();` のように正規化（方向ベクトル化）します。
 *   - `velocity.Normalize();` のようにベクトル自身を正規化します。
 */

struct Vector3D
{
    float x, y, z; // ベクトルの3つの成分

    // コンストラクタ: オブジェクト生成時に値を初期化
    // 引数を省略した場合は 0.0f で初期化される
    Vector3D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    // --- 演算子のオーバーロード ---
    // ベクトル同士の加算 (v1 + v2)
    Vector3D operator+(const Vector3D& _vec) const { return { x + _vec.x, y + _vec.y, z + _vec.z }; }
    // ベクトルを加算して自身に代入 (v1 += v2)
    void operator+=(const Vector3D& _vec) { x += _vec.x; y += _vec.y; z += _vec.z; }
    // ベクトル同士の減算 (v1 - v2)
    Vector3D operator-(const Vector3D& _vec) const { return { x - _vec.x, y - _vec.y, z - _vec.z }; }
    // ベクトルを減算して自身に代入 (v1 -= v2)
    void operator-=(const Vector3D& _vec) { x -= _vec.x; y -= _vec.y; z -= _vec.z; }
    // ベクトルとスカラー値の乗算 (vec * scale)
    Vector3D operator*(float _scale) const { return { x * _scale, y * _scale, z * _scale }; }
    // ベクトルにスカラー値を乗算して自身に代入 (vec *= scale)
    void operator*=(float _scale) { x *= _scale; y *= _scale; z *= _scale; }
    // (割り算演算子 operator/ も必要なら追加可能)

    // --- メンバ関数 ---
    // ベクトルの長さ（大きさ、ノルム）の2乗を計算する
    // 平方根の計算はコストが高いため、単純な大小比較などではこちらを使うと効率が良い
    float LengthSq() const {
        return x * x + y * y + z * z;
    }

    // ベクトルの長さ（大きさ、ノルム）を計算する
    float Length() const {
        return sqrtf(LengthSq()); // LengthSq()の結果の平方根を計算
    }

    // このベクトル自身を正規化（長さを1に）する
    // 方向ベクトルとして扱いたい場合などに使用する
    void Normalize() {
        float lenSq = LengthSq(); // まず長さの2乗を計算
        // 長さの2乗が非常に小さい値より大きい場合のみ正規化処理を行う (ゼロ除算回避)
        // 1e-12f は 0.000000000001 (10の-12乗)
        if (lenSq > 1e-12f) {
            float len = sqrtf(lenSq); // 長さを計算
            float invLen = 1.0f / len; // 割り算の代わりに逆数を掛ける
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
        else {
            // 長さがほぼゼロの場合は、正規化できないのでゼロベクトルにする
            x = y = z = 0.0f;
        }
    }

    // 正規化された新しいベクトルを生成して返す (元のベクトルは変更しない)
    // 例: Vector3D direction = velocity.Normalized();
    Vector3D Normalized() const {
        Vector3D result = *this; // 自分自身のコピーを作成
        result.Normalize();      // コピーを正規化
        return result;           // 正規化されたコピーを返す
    }
    // (内積 Dot() や 外積 Cross() の関数も必要に応じて追加すると便利)
    // static float Dot(const Vector3D& a, const Vector3D& b) { ... }
    // static Vector3D Cross(const Vector3D& a, const Vector3D& b) { ... }
};

// グローバル関数: 3つのfloat値からVector3Dを作成する (コンストラクタがあるので必須ではない)
// _inline はインライン展開をコンパイラに提案する指示子 (現代のコンパイラでは自動で判断することが多い)
inline Vector3D SetVec3D(float _x, float _y, float _z) { return { _x, _y, _z }; }

// グローバル関数: スカラー値とベクトルの乗算 (scale * vec の順序を可能にする)
inline Vector3D operator*(float _scale, const Vector3D& _vec) {
    return _vec * _scale; // 既存の Vector3D::operator* を呼び出す
}