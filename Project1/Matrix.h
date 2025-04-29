#pragma once
#include <cmath>      // sinf, cosf, tanf など (<math.h> より推奨)
#include <stdexcept> // 例外処理 (std::invalid_argument) のために必要
#include <cstring>   // memset など (もし使う場合)

/*
 * Matrix.h
 * 役割:
 *   このヘッダーファイルは、3Dグラフィックスで非常に重要な役割を果たす
 *   4x4行列 (`Matrix` 構造体) の定義と、それに関連する基本的な関数を提供します。
 *   主な機能は以下の通りです。
 *   - `Matrix` 構造体: 4x4の浮動小数点数(float)を格納するデータ構造。
 *   - `Matrix::Identity()`: 何も変換しない「単位行列」を作成する静的メソッド。
 *   - 各軸周りの回転行列を生成する関数 (`GetMatrixAxisXLH`, `GetMatrixAxisYLH`, `GetMatrixAxisZLH`)。
 *     これらは「左手座標系 (Left-Handed, LH)」に基づいています。
 *   - `MatrixMultiply`: 2つの行列を掛け合わせる関数。変換の合成に使います。
 *   - `PerspectiveFovLH`: 透視投影（遠近感）を与えるプロジェクション行列を生成する関数。
 *
 * このバージョンでの変更点 (元の Matrix.h から):
 * - 回転行列生成関数: 元の `GetMatrixAxisX`, `GetMatrixAxisY` が左手座標系(LH)の
 *   定義に基づいていることが明確になるように、関数名に `LH` が追加され、
 *   内部の計算式も左手座標系の標準的な定義に合わせて修正されています。
 *   Z軸周りの回転行列 `GetMatrixAxisZLH` も参考として追加されています。
 * - 機能追加: 行列の乗算 (`MatrixMultiply`)、単位行列 (`Identity`)、
 *   透視投影行列 (`PerspectiveFovLH`) を生成する関数が追加されています。
 *   これらは現代的な3Dカメラシステムの実装に不可欠な要素です。
 * - エラーチェック: `PerspectiveFovLH` に関数の引数が不正な場合に例外を投げる
 *   基本的なエラーチェックが追加されました。
 *
 * 使い方:
 *   - このヘッダーファイルをインクルード (`#include "Matrix.h"`) します。
 *   - `Matrix` 型の変数を作成して行列データを扱います。
 *   - `Matrix::Identity()` で単位行列を作成したり、`GetMatrixAxisYLH(angle)` などで
 *     回転行列を作成したりします。
 *   - `MatrixMultiply(matA, matB)` で行列Aと行列Bを掛け合わせた結果を得ます。
 *   - `PerspectiveFovLH(...)` でプロジェクション行列を作成します。
 *
 * 注意点:
 * - 行列の要素アクセス: `m[行][列]` の形式（Row-major order）でアクセスします。
 * - 左手座標系(LH): このヘッダー内の回転行列やプロジェクション行列は、
 *   DirectXなどで標準的に使われる左手座標系を基準にしています。
 *   OpenGLなどで使われる右手座標系とは一部の符号などが異なります。
 * - 行列の乗算順序: `MatrixMultiply(A, B)` は A * B を計算しますが、
 *   座標変換を適用する際は、変換を適用したい順序と逆の順序で
 *   行列を掛けるのが一般的です（例: 移動 * 回転 なら `matRot * matTrans`）。
 */

 // 4x4 行列を表す構造体
struct Matrix
{
    float m[4][4]; // 4x4 の float 型要素

    // デフォルトコンストラクタ: 全要素を 0.0f で初期化
    Matrix() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = 0.0f;
            }
        }
        // あるいは <cstring> をインクルードして: std::memset(m, 0, sizeof(float) * 16);
    }

    // 静的メソッド: 単位行列 (対角成分が1で他が0の行列) を作成して返す
    static Matrix Identity() {
        Matrix result; // ゼロ行列で初期化される
        result.m[0][0] = 1.0f;
        result.m[1][1] = 1.0f;
        result.m[2][2] = 1.0f;
        result.m[3][3] = 1.0f;
        return result;
    }
};

// X軸周りの回転行列を生成する関数 (左手座標系 / LH)
// XAxisRotation: 回転角度 (ラジアン単位)。正の値で反時計回り (Y軸からZ軸へ向かう向き)。
// (注意: DxLib のデフォルトとは回転方向が逆の可能性があります)
static Matrix GetMatrixAxisXLH(float XAxisRotation)
{
    float sin_val = sinf(XAxisRotation);
    float cos_val = cosf(XAxisRotation);
    Matrix result = Matrix::Identity(); // 単位行列から始める
    // X軸回転の成分を設定 (左手座標系の定義に基づく)
    result.m[1][1] = cos_val;  result.m[1][2] = sin_val;  // 修正: LHでは (1,2) が sin
    result.m[2][1] = -sin_val; result.m[2][2] = cos_val;  // 修正: LHでは (2,1) が -sin
    return result;
}

// Y軸周りの回転行列を生成する関数 (左手座標系 / LH)
// YAxisRotation: 回転角度 (ラジアン単位)。正の値で反時計回り (Z軸からX軸へ向かう向き)。
static Matrix GetMatrixAxisYLH(float YAxisRotation)
{
    float sin_val = sinf(YAxisRotation);
    float cos_val = cosf(YAxisRotation);
    Matrix result = Matrix::Identity();
    // Y軸回転の成分を設定 (左手座標系の定義に基づく)
    result.m[0][0] = cos_val;  result.m[0][2] = -sin_val; // 修正: LHでは (0,2) が -sin
    result.m[2][0] = sin_val;  result.m[2][2] = cos_val;  // 修正: LHでは (2,0) が sin
    return result;
}

// Z軸周りの回転行列を生成する関数 (左手座標系 / LH) - 参考として追加
// ZAxisRotation: 回転角度 (ラジアン単位)。正の値で反時計回り (X軸からY軸へ向かう向き)。
static Matrix GetMatrixAxisZLH(float ZAxisRotation)
{
    float sin_val = sinf(ZAxisRotation);
    float cos_val = cosf(ZAxisRotation);
    Matrix result = Matrix::Identity();
    // Z軸回転の成分を設定 (左手座標系の定義に基づく)
    result.m[0][0] = cos_val;  result.m[0][1] = sin_val;  // 修正: LHでは (0,1) が sin
    result.m[1][0] = -sin_val; result.m[1][1] = cos_val;  // 修正: LHでは (1,0) が -sin
    return result;
}


// 行列の乗算 C = A * B を計算する関数
static Matrix MatrixMultiply(const Matrix& a, const Matrix& b)
{
    Matrix c; // 結果を格納する行列 (コンストラクタでゼロ初期化される)
    // 行列乗算の定義に従って計算
    for (int i = 0; i < 4; i++) {       // 結果の行インデックス
        for (int j = 0; j < 4; j++) {   // 結果の列インデックス
            // c.m[i][j] = 0; // コンストラクタで初期化済みなので不要
            for (int k = 0; k < 4; k++) { // 内側のループ (和を計算)
                c.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return c;
}


// 左手座標系、視野角ベースの透視投影行列 (Perspective Projection Matrix) を作成する関数
// fovY: 垂直方向の視野角 (ラジアン単位)
// aspectRatio: スクリーンのアスペクト比 (幅 / 高さ)
// zn: ニアクリップ面までの距離 (Near Plane Distance) > 0
// zf: ファークリップ面までの距離 (Far Plane Distance) > zn
static Matrix PerspectiveFovLH(float fovY, float aspectRatio, float zn, float zf)
{
    // 引数の妥当性をチェック
    if (aspectRatio <= 0.0f || zn <= 0.0f || zf <= zn) {
        // 不正な引数が渡された場合は、エラーを示す例外を投げる
        throw std::invalid_argument("PerspectiveFovLH に不正な引数が渡されました。");
        // または、エラーメッセージを表示して単位行列を返すなどの代替処理も考えられる
        // DxLib が初期化済みなら printfDx(...) などでエラー表示
        // return Matrix::Identity();
    }

    Matrix result; // ゼロ行列で初期化される
    // Y方向のスケール (視野角から計算)
    float yScale = 1.0f / tanf(fovY / 2.0f);
    // X方向のスケール (Yスケールとアスペクト比から計算)
    float xScale = yScale / aspectRatio;
    // Z方向の変換係数 (左手座標系の定義)
    float zRange = zf / (zf - zn); // LH

    // 行列の要素を設定
    result.m[0][0] = xScale;
    result.m[1][1] = yScale;
    result.m[2][2] = zRange;        // Z値を [0, 1] の範囲に近づけるためのスケール (LH)
    result.m[2][3] = 1.0f;          // W' に元のZ値をコピーするための設定 (LH)
    result.m[3][2] = -zn * zRange;  // Z値のオフセット (LH)
    // result.m[3][3] は 0.0f のまま

    return result;
}