#pragma once
#include <fstream> // ファイル入出力 (ofstream) のために必要
#include <string>  // 文字列 (std::string) のために必要
#include <chrono>  // 現在時刻を取得するために必要 (ログのタイムスタンプ等)
#include <iomanip> // 時刻のフォーマット出力 (std::put_time) のために必要 (ただし互換性に注意)
#include <sstream> // 文字列ストリーム (今回は直接は使っていないが、ログ整形でありえる)

/*
 * Logger.h
 * 役割:
 *   このファイルは、プログラムの動作状況やデバッグ情報をファイルに書き出すための
 *   シンプルな「ロガークラス」を定義します。
 *   プログラム実行中に特定の情報をファイルに記録しておくことで、後から動作を確認したり、
 *   問題が発生した場合の原因調査に役立てることができます。
 *
 * 主な機能:
 *   - シングルトンパターン: Logger クラスのインスタンスがプログラム全体で一つだけ
 *     存在するように保証します (`GetInstance()` で取得)。これにより、どこからでも
 *     同じログファイルに書き込むことができます。
 *   - ファイル操作: 指定されたファイル名でログファイルを開き (`Open`)、
 *     メッセージを書き込み (`Write`)、プログラム終了時に自動的に閉じる (`Close`) 機能を提供します。
 *   - グローバル関数: `LogDebug` のような簡単な関数を用意し、どこからでも手軽に
 *     ログを書き込めるようにします。
 *
 * 元のコードからの変更点・コメント (推定):
 *   - この Logger クラスは、元の初期コードにはなく、デバッグ機能の強化のために
 *     後から追加されたものであると考えられます。
 *   - シングルトンパターンを採用し、コピーやムーブを禁止することで、意図しない
 *     複数のロガーインスタンスが作られるのを防いでいます。
 *   - `Open` 関数で、プログラム起動時にログファイルをクリア（上書き）するように
 *     設定されています (`std::ios::trunc`)。もし追記したい場合は `std::ios::app` に変更します。
 *   - ログの開始/終了時刻の書き込み部分は、より高度なタイムスタンプ機能を追加する際の
 *     参考としてコメントアウトされています (`std::put_time` は環境によって使えない可能性があります)。
 *
 * 使い方:
 *   1. `#include "Logger.h"` をインクルードします。
 *   2. プログラムの初期化部分 (例: `WinMain` の開始直後) で `Logger::GetInstance().Open("ファイル名.txt");`
 *      を呼び出してログファイルを開きます。
 *   3. ログを書き込みたい場所で `LogDebug("メッセージ");` のように呼び出します。
 *   4. プログラム終了時に自動的にファイルが閉じられますが、明示的に閉じたい場合は
 *      `Logger::GetInstance().Close();` を呼び出すこともできます。
 */

class Logger {
public:
    // シングルトンインスタンスを取得するための静的メソッド
    // プログラム中で Logger::GetInstance() と呼び出すことで、
    // 常に同じ Logger オブジェクトへの参照を取得できます。
    static Logger& GetInstance() {
        // static ローカル変数としてインスタンスを生成することで、
        // 最初に呼び出されたときに一度だけ初期化され、以降は同じインスタンスを返す。
        static Logger instance; // シングルトンインスタンス
        return instance;
    }

    // ログファイルを開くメソッド
    // filename: ログを書き込むファイルの名前 (デフォルトは "debug_log.txt")
    // 戻り値: ファイルを開くのに成功したら true、失敗したら false
    bool Open(const std::string& filename = "debug_log.txt") {
        // もし既にファイルが開かれていたら、一度閉じる
        if (logFile.is_open()) {
            logFile.close();
        }
        // ファイルを出力モード(std::ios::out)で開く。
        // std::ios::trunc を指定すると、ファイルが既に存在する場合、中身を空にしてから開く（上書き）。
        // もし追記したい場合は std::ios::app を代わりに指定する。
        logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!logFile.is_open()) { // ファイルを開けなかった場合
            // DxLibが初期化済みなら、画面にエラーメッセージを表示することも可能
            // printfDx("エラー: ログファイル '%s' を開けませんでした。\n", filename.c_str());
            return false; // 失敗したことを呼び出し元に伝える
        }
        // ファイルを開いたときに、開始時刻などを記録する（任意）
        // auto now = std::chrono::system_clock::now();
        // auto now_c = std::chrono::system_clock::to_time_t(now);
        // std::put_time は C++11 標準ではないため、環境によっては使えない。
        // 代替案: #include <ctime> して localtime_s や strftime を使う。
        // logFile << "--- Log Start: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " ---\n";
        logFile << "--- Log Start ---" << std::endl; // より簡単な開始マーカー

        return true; // 成功したことを伝える
    }

    // ログファイルにメッセージを書き込むメソッド
    // message: 書き込みたい文字列
    void Write(const std::string& message) {
        // ファイルが正常に開かれている場合のみ書き込む
        if (logFile.is_open()) {
            logFile << message << std::endl; // メッセージをファイルに書き込み、最後に改行を追加
        }
    }

    // ログファイルを閉じるメソッド
    // 通常はデストラクタで自動的に呼ばれるので、明示的に呼ぶ必要は少ない。
    void Close() {
        if (logFile.is_open()) { // ファイルが開いている場合
            logFile << "--- Log End ---" << std::endl; // 終了マーカーを書き込む
            logFile.close(); // ファイルを閉じる
        }
    }

    // デストラクタ: Loggerオブジェクトが破棄されるときに自動的に呼び出される
    // ここでファイルを確実に閉じることで、書き込み内容が失われるのを防ぐ。
    ~Logger() {
        Close(); // ファイルを閉じる処理を呼ぶ
    }

private: // クラス内部からのみアクセス可能なメンバ
    // ファイル出力ストリームオブジェクト。実際のファイル書き込みを行う。
    std::ofstream logFile;

    // プライベートコンストラクタ: シングルトンパターンを実現するため、外部から直接
    // Logger オブジェクトを作成できないようにする。GetInstance() を通してのみ取得可能。
    Logger() = default; // デフォルトコンストラクタを有効にする

    // コピーコンストラクタとコピー代入演算子を禁止 (delete指定)
    // これにより、Logger オブジェクトが誤ってコピーされるのを防ぎ、
    // シングルトン（唯一のインスタンス）であることを保証する。
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ムーブコンストラクタとムーブ代入演算子も禁止 (delete指定)
    // 同様に、意図しない所有権の移動を防ぐ。
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

// グローバルなログ関数 (簡易アクセス用)
// これを使うことで、Logger::GetInstance().Write(...) と書く代わりに
// LogDebug(...) と簡潔に書ける。
inline void LogDebug(const std::string& message) {
    // シングルトンインスタンスを取得し、Writeメソッドを呼び出す
    Logger::GetInstance().Write(message);
}

// 必要に応じて、他のログレベル用のグローバル関数も定義できる
// inline void LogInfo(const std::string& message) { /* ... */ }
// inline void LogWarning(const std::string& message) { /* ... */ }
// inline void LogError(const std::string& message) { /* ... */ }