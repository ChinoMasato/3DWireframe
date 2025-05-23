#pragma once
#include <vector> // 他のファイルで <vector> を使う可能性があるため、念のためインクルード

/*
 * Common.h
 * 役割:
 *   このヘッダーファイルは、プロジェクト全体を通して共通で使われる
 *   定数をまとめて定義する場所です。
 *   例えば、ウィンドウのサイズ、円周率(PI)のような数学的な定数、
 *   角度の単位変換係数、カメラや描画に関する設定値などを一箇所で管理します。
 *   これにより、値の変更が必要になった場合に、このファイルだけを修正すればよくなり、
 *   コードの保守性（メンテナンスのしやすさ）が向上します。
 *
 * このバージョンでの変更点 (元の Common.h から):
 * - 匿名名前空間 (`namespace {}`) を使用しています。
 *   - ここで定義された定数は、このファイルをインクルードした各ソースファイル(.cpp)内でのみ
 *     有効になります（これを「内部リンケージを持つ」と言います）。
 *   - これにより、他のファイルで偶然同じ名前の定数が使われていても、名前の衝突による
 *     リンクエラーを防ぐことができます。
 *   - 匿名名前空間の中では、`static` をつけなくても `const` だけで内部リンケージになるため、
 *     `static const` から `const` に変更されています。
 * - 円周率 `PI` の値を、より桁数の多い float 型の値に変更しました。
 * - 定数 `SCREEN` について、これが射影平面（カメラから見た像を映し出す仮想的なスクリーン）
 *   までの距離を表す値である旨のコメントが追加されています。
 *   （ただし、新しい Camera クラスではこの `SCREEN` 定数は直接使用されていません。）
 *
 * 使い方:
 *   このヘッダーファイルをインクルード (`#include "Common.h"`) したファイル内では、
 *   ここで定義された定数を直接名前で使うことができます (例: `WINDOW_WIDTH`, `PI` など)。
 */

namespace // 匿名名前空間の開始
{
    // --- 数学関連の定数 ---
    const float PI = 3.1415926535f;       // 円周率 (float型)
    const float ONE_DEGREE = PI / 180.0f; // 1度をラジアン単位に変換するための係数

    // --- ウィンドウ設定 ---
    const float WINDOW_WIDTH = 800.0f;     // ゲームウィンドウの幅 (ピクセル単位)
    const float WINDOW_HEIGHT = 600.0f;    // ゲームウィンドウの高さ (ピクセル単位)

    // --- カメラ・レンダリング関連の設定 (元のコード由来) ---
    // (注意: 以下の SCREEN 定数は、最初の単純なカメラ実装で使われていた可能性があり、
    //  新しい Camera クラス (行列ベース) では直接は使用されていません。)
    const float SCREEN = 500.0f;           // 射影平面 (仮想スクリーン) までの距離を示す値。
    // 元のカメラ実装における透視投影の計算で使われていました。
} // 匿名名前空間の終わり