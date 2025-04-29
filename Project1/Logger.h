#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

/*
 * Logger.h
 * ����: �f�o�b�O�����t�@�C���ɏ������ނ��߂̊ȈՃ��K�[�N���X�B
 */

class Logger {
public:
    // �V���O���g���C���X�^���X���擾
    static Logger& GetInstance() {
        static Logger instance; // �V���O���g���C���X�^���X
        return instance;
    }

    // ���O�t�@�C�����J��
    bool Open(const std::string& filename = "debug_log.txt") {
        // ���ɊJ���Ă���ꍇ�͕���
        if (logFile.is_open()) {
            logFile.close();
        }
        // �ǋL���[�h�Ńt�@�C�����J��
        logFile.open(filename, std::ios::out | std::ios::trunc); // �N�����Ƀt�@�C�����N���A
        if (!logFile.is_open()) {
            // DxLib���g����Ȃ�G���[�\��
            // printfDx("�G���[: ���O�t�@�C�� '%s' ���J���܂���ł����B\n", filename.c_str());
            return false;
        }
        // ���O�J�n�����Ȃǂ��������ށi�C�Ӂj
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        // std::put_time �� C++11 �ł͎g���Ȃ��\�������邽�߁Actime_s �܂��� localtime_s ���g��
        // �܂��͊ȒP�Ȍ`���ŏo��
        // logFile << "--- Log Start: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " ---\n";
        logFile << "--- Log Start ---\n";

        return true;
    }

    // ���O�t�@�C���ɏ�������
    void Write(const std::string& message) {
        if (logFile.is_open()) {
            logFile << message << std::endl; // ���b�Z�[�W�Ɖ��s����������
        }
    }

    // ���O�t�@�C������� (�f�X�g���N�^�Ŏ����I�ɌĂ΂��)
    void Close() {
        if (logFile.is_open()) {
            logFile << "--- Log End ---\n";
            logFile.close();
        }
    }

    // �f�X�g���N�^
    ~Logger() {
        Close();
    }

private:
    std::ofstream logFile; // �t�@�C���X�g���[��

    // �v���C�x�[�g�R���X�g���N�^�i�V���O���g���p�j
    Logger() = default;
    // �R�s�[�ƃ��[�u���֎~
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

// �O���[�o���ȃ��O�֐� (�C��)
inline void LogDebug(const std::string& message) {
    Logger::GetInstance().Write(message);
}