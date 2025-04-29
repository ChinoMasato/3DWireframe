// !! NOMINMAX �̓t�@�C���̐擪 !!
#define NOMINMAX
#include "Common.h"
#include "DxLib.h"
#include "Camera.h"
#include "TopAngle.h"   // �C���N���[�h�͎c�����A�g�p���Ȃ�
#include "Vector.h"
#include <vector>
#include <string>
#include "Logger.h" // ���K�[�N���X���C���N���[�h

/*
 * Main.cpp
 * ����: �A�v���P�[�V�����̃G���g���[�|�C���g�B
 *
 * �ύX�_:
 * - Logger �����������A���t���[���ڍׂȃf�o�b�O�����t�@�C���ɏ������ށB
 * - TopAngle �̕`����R�����g�A�E�g�B
 * - �f�o�b�O�\����Y���W������ɒ����B
 */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib �������t�F�[�Y ---
    ChangeWindowMode(TRUE);
    SetWindowSizeChangeEnableFlag(FALSE);
    SetGraphMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), 32);
    if (DxLib_Init() < 0) return -1;
    SetMouseDispFlag(FALSE);
    SetDrawScreen(DX_SCREEN_BACK);

    // --- �����K�[�̏����� ---
    if (!Logger::GetInstance().Open("debug_log.txt")) {
        // �t�@�C���I�[�v�����s���̏����i�����ł͒P���ɑ�����j
        printfDx("�x��: �f�o�b�O���O�t�@�C�����J���܂���ł����B\n");
    }
    LogDebug("Application Started."); // �J�n���O

    // --- �I�u�W�F�N�g�f�[�^�̏��� ---
    std::vector<std::vector<Vector3D>> worldLine;
    // �����̂ƒn�ʃO���b�h�f�[�^ (�ȗ�)
    { float s = 25.0f; Vector3D p[8] = { {-s,-s,-s},{s,-s,-s},{s,s,-s},{-s,s,-s},{-s,-s,s},{s,-s,s},{s,s,s},{-s,s,s} }; worldLine.push_back({ p[0], p[1] }); worldLine.push_back({ p[1], p[2] }); worldLine.push_back({ p[2], p[3] }); worldLine.push_back({ p[3], p[0] }); worldLine.push_back({ p[4], p[5] }); worldLine.push_back({ p[5], p[6] }); worldLine.push_back({ p[6], p[7] }); worldLine.push_back({ p[7], p[4] }); worldLine.push_back({ p[0], p[4] }); worldLine.push_back({ p[1], p[5] }); worldLine.push_back({ p[2], p[6] }); worldLine.push_back({ p[3], p[7] }); }
    { float groundExtent = 100.0f; float gridSize = 5.0f; int lineCount = static_cast<int>(groundExtent / gridSize); for (int i = -lineCount; i <= lineCount; ++i) { float cX = i * gridSize; worldLine.push_back({ {cX,0.0f,-groundExtent},{cX,0.0f,groundExtent} }); } for (int i = -lineCount; i <= lineCount; ++i) { float cZ = i * gridSize; worldLine.push_back({ {-groundExtent,0.0f,cZ},{groundExtent,0.0f,cZ} }); } }


    // --- �J�����̐��� ---
    Camera* camera = new Camera();
    // !! �C��: TopAngle �͐����E�g�p���Ȃ� !!
    // TopAngle* topangle = new TopAngle(camera);

    // --- ���C�����[�v ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();
        camera->Update();

        // --- ���f�o�b�O���O�t�@�C���ւ̏������� ---
        LogDebug(camera->GetDetailedDebugInfo()); // �ڍ׏������O��

        camera->Draw(worldLine);
        // topangle->Draw(worldLine); // �R�����g�A�E�g

        // --- �\���}�[�N�̕`�� ---
        { int cX = (int)(WINDOW_WIDTH / 2), cY = (int)(WINDOW_HEIGHT / 2), sz = 10; DrawLine(cX - sz, cY, cX + sz, cY, GetColor(255, 255, 0)); DrawLine(cX, cY - sz, cX, cY + sz, GetColor(255, 255, 0)); }

        // --- �f�o�b�O���̉�ʕ\�� ---
        {
            std::string debugText = camera->GetDebugInfo(); // ��ʗp�͒Z�����
            // !! �C��: Y���W������ɒ��� !!
            DrawFormatString(10, WINDOW_HEIGHT - 60, GetColor(255, 255, 255), debugText.c_str()); // Y��60�ɕύX
        }

        ScreenFlip();
    }

    // --- �I������ ---
    LogDebug("Application Ended."); // �I�����O
    Logger::GetInstance().Close(); // �����I�ɕ���i�f�X�g���N�^�ł����邪�j
    // delete topangle; // �������Ȃ��̂ŕs�v
    delete camera;
    DxLib_End();

    return 0;
}