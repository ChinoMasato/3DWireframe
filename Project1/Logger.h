#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

/*
 * Logger.h
 * 役割: デバッグ情報をファイルに書き込むための簡易ロガークラス。
 */

class Logger {
public:
    // シングルトンインスタンスを取得
    static Logger& GetInstance() {
        static Logger instance; // シングルトンインスタンス
        return instance;
    }

    // ログファイルを開く
    bool Open(const std::string& filename = "debug_log.txt") {
        // 既に開いている場合は閉じる
        if (logFile.is_open()) {
            logFile.close();
        }
        // 追記モードでファイルを開く
        logFile.open(filename, std::ios::out | std::ios::trunc); // 起動時にファイルをクリア
        if (!logFile.is_open()) {
            // DxLibが使えるならエラー表示
            // printfDx("エラー: ログファイル '%s' を開けませんでした。\n", filename.c_str());
            return false;
        }
        // ログ開始時刻などを書き込む（任意）
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        // std::put_time は C++11 では使えない可能性があるため、ctime_s または localtime_s を使う
        // または簡単な形式で出力
        // logFile << "--- Log Start: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " ---\n";
        logFile << "--- Log Start ---\n";

        return true;
    }

    // ログファイルに書き込む
    void Write(const std::string& message) {
        if (logFile.is_open()) {
            logFile << message << std::endl; // メッセージと改行を書き込む
        }
    }

    // ログファイルを閉じる (デストラクタで自動的に呼ばれる)
    void Close() {
        if (logFile.is_open()) {
            logFile << "--- Log End ---\n";
            logFile.close();
        }
    }

    // デストラクタ
    ~Logger() {
        Close();
    }

private:
    std::ofstream logFile; // ファイルストリーム

    // プライベートコンストラクタ（シングルトン用）
    Logger() = default;
    // コピーとムーブを禁止
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

// グローバルなログ関数 (任意)
inline void LogDebug(const std::string& message) {
    Logger::GetInstance().Write(message);
}