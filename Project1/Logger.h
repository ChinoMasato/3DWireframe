#pragma once
#include <fstream> // �t�@�C�����o�� (ofstream) �̂��߂ɕK�v
#include <string>  // ������ (std::string) �̂��߂ɕK�v
#include <chrono>  // ���ݎ������擾���邽�߂ɕK�v (���O�̃^�C���X�^���v��)
#include <iomanip> // �����̃t�H�[�}�b�g�o�� (std::put_time) �̂��߂ɕK�v (�������݊����ɒ���)
#include <sstream> // ������X�g���[�� (����͒��ڂ͎g���Ă��Ȃ����A���O���`�ł��肦��)

/*
 * Logger.h
 * ����:
 *   ���̃t�@�C���́A�v���O�����̓���󋵂�f�o�b�O�����t�@�C���ɏ����o�����߂�
 *   �V���v���ȁu���K�[�N���X�v���`���܂��B
 *   �v���O�������s���ɓ���̏����t�@�C���ɋL�^���Ă������ƂŁA�ォ�瓮����m�F������A
 *   ��肪���������ꍇ�̌��������ɖ𗧂Ă邱�Ƃ��ł��܂��B
 *
 * ��ȋ@�\:
 *   - �V���O���g���p�^�[��: Logger �N���X�̃C���X�^���X���v���O�����S�̂ň����
 *     ���݂���悤�ɕۏ؂��܂� (`GetInstance()` �Ŏ擾)�B����ɂ��A�ǂ�����ł�
 *     �������O�t�@�C���ɏ������ނ��Ƃ��ł��܂��B
 *   - �t�@�C������: �w�肳�ꂽ�t�@�C�����Ń��O�t�@�C�����J�� (`Open`)�A
 *     ���b�Z�[�W���������� (`Write`)�A�v���O�����I�����Ɏ����I�ɕ��� (`Close`) �@�\��񋟂��܂��B
 *   - �O���[�o���֐�: `LogDebug` �̂悤�ȊȒP�Ȋ֐���p�ӂ��A�ǂ�����ł���y��
 *     ���O���������߂�悤�ɂ��܂��B
 *
 * ���̃R�[�h����̕ύX�_�E�R�����g (����):
 *   - ���� Logger �N���X�́A���̏����R�[�h�ɂ͂Ȃ��A�f�o�b�O�@�\�̋����̂��߂�
 *     �ォ��ǉ����ꂽ���̂ł���ƍl�����܂��B
 *   - �V���O���g���p�^�[�����̗p���A�R�s�[�⃀�[�u���֎~���邱�ƂŁA�Ӑ}���Ȃ�
 *     �����̃��K�[�C���X�^���X�������̂�h���ł��܂��B
 *   - `Open` �֐��ŁA�v���O�����N�����Ƀ��O�t�@�C�����N���A�i�㏑���j����悤��
 *     �ݒ肳��Ă��܂� (`std::ios::trunc`)�B�����ǋL�������ꍇ�� `std::ios::app` �ɕύX���܂��B
 *   - ���O�̊J�n/�I�������̏������ݕ����́A��荂�x�ȃ^�C���X�^���v�@�\��ǉ�����ۂ�
 *     �Q�l�Ƃ��ăR�����g�A�E�g����Ă��܂� (`std::put_time` �͊��ɂ���Ďg���Ȃ��\��������܂�)�B
 *
 * �g����:
 *   1. `#include "Logger.h"` ���C���N���[�h���܂��B
 *   2. �v���O�����̏��������� (��: `WinMain` �̊J�n����) �� `Logger::GetInstance().Open("�t�@�C����.txt");`
 *      ���Ăяo���ă��O�t�@�C�����J���܂��B
 *   3. ���O���������݂����ꏊ�� `LogDebug("���b�Z�[�W");` �̂悤�ɌĂяo���܂��B
 *   4. �v���O�����I�����Ɏ����I�Ƀt�@�C���������܂����A�����I�ɕ������ꍇ��
 *      `Logger::GetInstance().Close();` ���Ăяo�����Ƃ��ł��܂��B
 */

class Logger {
public:
    // �V���O���g���C���X�^���X���擾���邽�߂̐ÓI���\�b�h
    // �v���O�������� Logger::GetInstance() �ƌĂяo�����ƂŁA
    // ��ɓ��� Logger �I�u�W�F�N�g�ւ̎Q�Ƃ��擾�ł��܂��B
    static Logger& GetInstance() {
        // static ���[�J���ϐ��Ƃ��ăC���X�^���X�𐶐����邱�ƂŁA
        // �ŏ��ɌĂяo���ꂽ�Ƃ��Ɉ�x��������������A�ȍ~�͓����C���X�^���X��Ԃ��B
        static Logger instance; // �V���O���g���C���X�^���X
        return instance;
    }

    // ���O�t�@�C�����J�����\�b�h
    // filename: ���O���������ރt�@�C���̖��O (�f�t�H���g�� "debug_log.txt")
    // �߂�l: �t�@�C�����J���̂ɐ��������� true�A���s������ false
    bool Open(const std::string& filename = "debug_log.txt") {
        // �������Ƀt�@�C�����J����Ă�����A��x����
        if (logFile.is_open()) {
            logFile.close();
        }
        // �t�@�C�����o�̓��[�h(std::ios::out)�ŊJ���B
        // std::ios::trunc ���w�肷��ƁA�t�@�C�������ɑ��݂���ꍇ�A���g����ɂ��Ă���J���i�㏑���j�B
        // �����ǋL�������ꍇ�� std::ios::app �����Ɏw�肷��B
        logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!logFile.is_open()) { // �t�@�C�����J���Ȃ������ꍇ
            // DxLib���������ς݂Ȃ�A��ʂɃG���[���b�Z�[�W��\�����邱�Ƃ��\
            // printfDx("�G���[: ���O�t�@�C�� '%s' ���J���܂���ł����B\n", filename.c_str());
            return false; // ���s�������Ƃ��Ăяo�����ɓ`����
        }
        // �t�@�C�����J�����Ƃ��ɁA�J�n�����Ȃǂ��L�^����i�C�Ӂj
        // auto now = std::chrono::system_clock::now();
        // auto now_c = std::chrono::system_clock::to_time_t(now);
        // std::put_time �� C++11 �W���ł͂Ȃ����߁A���ɂ���Ă͎g���Ȃ��B
        // ��ֈ�: #include <ctime> ���� localtime_s �� strftime ���g���B
        // logFile << "--- Log Start: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " ---\n";
        logFile << "--- Log Start ---" << std::endl; // ���ȒP�ȊJ�n�}�[�J�[

        return true; // �����������Ƃ�`����
    }

    // ���O�t�@�C���Ƀ��b�Z�[�W���������ރ��\�b�h
    // message: �������݂���������
    void Write(const std::string& message) {
        // �t�@�C��������ɊJ����Ă���ꍇ�̂ݏ�������
        if (logFile.is_open()) {
            logFile << message << std::endl; // ���b�Z�[�W���t�@�C���ɏ������݁A�Ō�ɉ��s��ǉ�
        }
    }

    // ���O�t�@�C������郁�\�b�h
    // �ʏ�̓f�X�g���N�^�Ŏ����I�ɌĂ΂��̂ŁA�����I�ɌĂԕK�v�͏��Ȃ��B
    void Close() {
        if (logFile.is_open()) { // �t�@�C�����J���Ă���ꍇ
            logFile << "--- Log End ---" << std::endl; // �I���}�[�J�[����������
            logFile.close(); // �t�@�C�������
        }
    }

    // �f�X�g���N�^: Logger�I�u�W�F�N�g���j�������Ƃ��Ɏ����I�ɌĂяo�����
    // �����Ńt�@�C�����m���ɕ��邱�ƂŁA�������ݓ��e��������̂�h���B
    ~Logger() {
        Close(); // �t�@�C������鏈�����Ă�
    }

private: // �N���X��������̂݃A�N�Z�X�\�ȃ����o
    // �t�@�C���o�̓X�g���[���I�u�W�F�N�g�B���ۂ̃t�@�C���������݂��s���B
    std::ofstream logFile;

    // �v���C�x�[�g�R���X�g���N�^: �V���O���g���p�^�[�����������邽�߁A�O�����璼��
    // Logger �I�u�W�F�N�g���쐬�ł��Ȃ��悤�ɂ���BGetInstance() ��ʂ��Ă̂ݎ擾�\�B
    Logger() = default; // �f�t�H���g�R���X�g���N�^��L���ɂ���

    // �R�s�[�R���X�g���N�^�ƃR�s�[������Z�q���֎~ (delete�w��)
    // ����ɂ��ALogger �I�u�W�F�N�g������ăR�s�[�����̂�h���A
    // �V���O���g���i�B��̃C���X�^���X�j�ł��邱�Ƃ�ۏ؂���B
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ���[�u�R���X�g���N�^�ƃ��[�u������Z�q���֎~ (delete�w��)
    // ���l�ɁA�Ӑ}���Ȃ����L���̈ړ���h���B
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

// �O���[�o���ȃ��O�֐� (�ȈՃA�N�Z�X�p)
// ������g�����ƂŁALogger::GetInstance().Write(...) �Ə��������
// LogDebug(...) �ƊȌ��ɏ�����B
inline void LogDebug(const std::string& message) {
    // �V���O���g���C���X�^���X���擾���AWrite���\�b�h���Ăяo��
    Logger::GetInstance().Write(message);
}

// �K�v�ɉ����āA���̃��O���x���p�̃O���[�o���֐�����`�ł���
// inline void LogInfo(const std::string& message) { /* ... */ }
// inline void LogWarning(const std::string& message) { /* ... */ }
// inline void LogError(const std::string& message) { /* ... */ }