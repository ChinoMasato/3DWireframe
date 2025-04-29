#pragma once
#include <cmath>     // fabsf, sinf, cosf など (<math.h> より推奨)
#include "Vector.h" // Vector3D を使用
#include "Matrix.h" // Matrix を使用
#include <sstream> // デバッグログ用 (stringstream)
#include <iomanip> // デバッグログ用 (setprecision)
#include "Logger.h" // デバッグログ用 (LogDebug) (必要ならインクルード)

/*
 * CameraMath.h
 * 役割:
 *   このヘッダーファイルは、カメラ関連、特にベクトルと行列を用いた
 *   座標変換計算など、3Dグラフィックスにおける数学的な処理を行うための
 *   ヘルパー関数や構造体を定義します。
 *   以前のバージョンから整理・変更されています。
 *
 * このバージョンでの変更点 (以前の CameraMath.h から):
 * - Vector4D 構造体: 4次元ベクトル（同次座標）を扱うための構造体が定義されています。
 *                    これは3D座標変換（特に透視投影）で重要な役割を果たします。
 * - VEC4Transform 関数: 4次元ベクトルを行列で変換する関数が実装されています。
 *                      コメントアウトされていますが、デバッグログ出力の機能も含まれています。
 * - TransformCoord 関数: 3次元の「座標点」を行列で変換し、パースペクティブ除算（wで割る）を
 *                       行って再び3次元座標に戻す関数が実装されています。
 *                       これは、ビュー・プロジェクション変換などで使われます。
 * - VEC3Transform 関数: 3次元ベクトルを行列の左上3x3部分（回転・スケーリング）で変換する関数です。
 *                      これは主に「方向ベクトル」の変換に使われます（平行移動の影響は受けません）。
 *                      （元の `CameraMath.h` にも類似の関数があったかもしれません）
 *
 * 使い方:
 *   - このファイルをインクルード (`#include "CameraMath.h"`) します。
 *   - 必要に応じて `Vector4D` 構造体を使用します。
 *   - ベクトルや座標を行列で変換したい場合に、`VEC3Transform`, `VEC4Transform`, `TransformCoord`
 *     といった関数を呼び出して使用します。
 *
 * 注意点:
 * - このファイル内の関数は、`Vector.h` や `Matrix.h` で定義された構造体や、
 *   場合によっては `Logger.h` に依存しています。これらのファイルが正しく
 *   プロジェクトに含まれている必要があります。
 * - `VEC3Transform` は方向ベクトル用、`TransformCoord` は座標点用と、
 *   用途に応じて関数を使い分けることが重要です。
 * - パースペクティブ除算 (`TransformCoord` 内) では、w成分がゼロに近い場合に
 *   ゼロ除算が発生しないように注意が必要です（このコードではチェックされています）。
 */

 // 4Dベクトル(同次座標)を扱うための構造体
struct Vector4D {
    float x, y, z, w; // 4つの成分

    // デフォルトコンストラクタ (初期値を設定)
    Vector4D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
        : x(_x), y(_y), z(_z), w(_w) {
    }
    // 必要であれば、Vector3D からの変換コンストラクタなども追加できます
    // Vector4D(const Vector3D& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
};


// 3Dベクトルを行列で変換する関数 (主に方向ベクトル用)
// 行列の回転・スケーリング成分（左上3x3）のみを適用し、平行移動成分は無視します。
inline Vector3D VEC3Transform(const Vector3D& _vec, const Matrix& _mat)
{
    // デバッグログ出力例 (必要ならコメントアウト解除して使用)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC3Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << ")";
    // // 必要なら行列の内容もログに出力
    // LogDebug(ss_log.str());

    Vector3D result;
    // 行列の左上3x3部分とベクトルの乗算
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2];

    // ss_log.str(""); // ストリームをクリア
    // ss_log << "[VEC3Trans] OutVec(" << result.x << "," << result.y << "," << result.z << ")";
    // LogDebug(ss_log.str());

    // 注意: 方向ベクトルとして使う場合、変換後に正規化(Normalize)が必要な場合があります
    return result;
}


// 4Dベクトル(同次座標)を行列で変換する関数
// ベクトルの4成分すべてを行列と計算します。
inline Vector4D VEC4Transform(const Vector4D& _vec, const Matrix& _mat)
{
    // デバッグログ出力例 (必要ならコメントアウト解除して使用)
    // std::stringstream ss_log;
    // ss_log << std::fixed << std::setprecision(4);
    // ss_log << "[VEC4Trans] InVec(" << _vec.x << "," << _vec.y << "," << _vec.z << "," << _vec.w << ")";
    // LogDebug(ss_log.str()); // 行列内容もログに出力すると長くなるので注意

    Vector4D result;
    // 4x4行列と4Dベクトルの乗算
    result.x = _vec.x * _mat.m[0][0] + _vec.y * _mat.m[1][0] + _vec.z * _mat.m[2][0] + _vec.w * _mat.m[3][0];
    result.y = _vec.x * _mat.m[0][1] + _vec.y * _mat.m[1][1] + _vec.z * _mat.m[2][1] + _vec.w * _mat.m[3][1];
    result.z = _vec.x * _mat.m[0][2] + _vec.y * _mat.m[1][2] + _vec.z * _mat.m[2][2] + _vec.w * _mat.m[3][2];
    result.w = _vec.x * _mat.m[0][3] + _vec.y * _mat.m[1][3] + _vec.z * _mat.m[2][3] + _vec.w * _mat.m[3][3];

    // ss_log.str(""); // ストリームをクリア
    // ss_log << "[VEC4Trans] OutVec(" << result.x << "," << result.y << "," << result.z << "," << result.w << ")";
    // LogDebug(ss_log.str());

    return result;
}

// 3D座標点を行列で変換し、パースペクティブ除算を行う関数 (主に座標点用)
// 内部的には (x, y, z, 1) の4Dベクトルとして変換し、結果のw成分で x, y, z を割ります。
// これにより、平行移動や透視投影の効果が適用された3D座標が得られます。
inline Vector3D TransformCoord(const Vector3D& _coord, const Matrix& _mat)
{
    Vector4D temp; // (x, y, z, 1) として計算するための一時変数

    // 4x4行列と (x, y, z, 1) ベクトルの乗算を実行
    temp.x = _coord.x * _mat.m[0][0] + _coord.y * _mat.m[1][0] + _coord.z * _mat.m[2][0] + /* 1.0f * */ _mat.m[3][0]; // w=1 を掛ける
    temp.y = _coord.x * _mat.m[0][1] + _coord.y * _mat.m[1][1] + _coord.z * _mat.m[2][1] + /* 1.0f * */ _mat.m[3][1];
    temp.z = _coord.x * _mat.m[0][2] + _coord.y * _mat.m[1][2] + _coord.z * _mat.m[2][2] + /* 1.0f * */ _mat.m[3][2];
    temp.w = _coord.x * _mat.m[0][3] + _coord.y * _mat.m[1][3] + _coord.z * _mat.m[2][3] + /* 1.0f * */ _mat.m[3][3];

    Vector3D result = { 0.0f, 0.0f, 0.0f }; // 結果の3Dベクトルを初期化

    // パースペクティブ除算: 結果の w 成分で x, y, z を割る
    // ただし、w がゼロに近い場合はゼロ除算エラーになるためチェックを行う
    if (std::fabsf(temp.w) > 1e-6f) { // w の絶対値が非常に小さい値より大きい場合のみ計算
        float invW = 1.0f / temp.w; // 割り算の代わりに逆数を掛ける（計算効率のため）
        result.x = temp.x * invW;
        result.y = temp.y * invW;
        result.z = temp.z * invW;
    }
    else {
        // w がゼロに近い場合は、エラーとして扱うか、あるいは特定の値を返す。
        // ここではゼロベクトルを返している。
        // LogWarning("TransformCoord: w component is near zero during perspective divide."); // 必要なら警告ログ
    }
    return result;
}

// 元の CameraMath.h にあった CameraTransform 関数 (オイラー角ベースの古い変換) は
// 新しいカメラ実装 (クォータニオン + 行列) では不要になったため削除されています。
/*
static Vector3D CameraTransform(Vector3D& _location, Vector3D& _rotation)
{
    Matrix rotV = GetMatrixAxisX(_rotation.x);
    Matrix rotH = GetMatrixAxisY(_rotation.y);
    Vector3D result = _location;
    result = VECTransform(result, rotV);
    result = VECTransform(result, rotH);
    return result;
}
*/