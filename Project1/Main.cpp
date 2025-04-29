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
#include <math.h>  // sinf, cosf ���g�p���邽��

/*
 * Main.cpp
 * ����: �A�v���P�[�V�����̃G���g���[�|�C���g�B
 *
 * �ύX�_:
 * - �n�ʃO���b�h�͈̔͂��g�債�AY���W�𗧕��̂̉��ɕύX�B
 * - ���C���[�t���[���̋��̃I�u�W�F�N�g��ǉ��B
 * - TopAngle �̃R�����g�A�E�g������ (�K�v�ɉ����čēx�R�����g�A�E�g���Ă�������)�B
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

    // --- ���K�[�̏����� ---
    if (!Logger::GetInstance().Open("debug_log.txt")) {
        printfDx("�x��: �f�o�b�O���O�t�@�C�����J���܂���ł����B\n");
    }
    LogDebug("Application Started.");

    // --- �I�u�W�F�N�g�f�[�^�̏��� ---
    std::vector<std::vector<Vector3D>> worldLine;

    // --- �����̂̕Ӄf�[�^�쐬 ---
    {
        float s = 25.0f; // �����̕ӂ̒���
        Vector3D center = { 0.0f, 0.0f, 50.0f }; // �������ɔz�u
        Vector3D p[8] = {
            {center.x - s, center.y - s, center.z - s}, {center.x + s, center.y - s, center.z - s},
            {center.x + s, center.y + s, center.z - s}, {center.x - s, center.y + s, center.z - s},
            {center.x - s, center.y - s, center.z + s}, {center.x + s, center.y - s, center.z + s},
            {center.x + s, center.y + s, center.z + s}, {center.x - s, center.y + s, center.z + s}
        };
        worldLine.push_back({ p[0], p[1] }); worldLine.push_back({ p[1], p[2] });
        worldLine.push_back({ p[2], p[3] }); worldLine.push_back({ p[3], p[0] });
        worldLine.push_back({ p[4], p[5] }); worldLine.push_back({ p[5], p[6] });
        worldLine.push_back({ p[6], p[7] }); worldLine.push_back({ p[7], p[4] });
        worldLine.push_back({ p[0], p[4] }); worldLine.push_back({ p[1], p[5] });
        worldLine.push_back({ p[2], p[6] }); worldLine.push_back({ p[3], p[7] });
    }

    // --- ���n�ʂ̃O���b�h�f�[�^�쐬 (�g���E�ʒu�ύX) ---
    {
        float groundY = -25.0f; // �n�ʂ�Y���W (�����̂̒�ʂƓ�������)
        float groundExtent = 500.0f; // �n�ʂ�`�悷��͈͂��g��
        float gridSize = 10.0f;     // �O���b�h��1�}�X�̃T�C�Y (�����e��)
        int lineCount = static_cast<int>(groundExtent / gridSize);

        // X���ɕ��s�Ȑ� (Z�����ɐ�������)
        for (int i = -lineCount; i <= lineCount; ++i) {
            float currentX = i * gridSize;
            Vector3D startPoint = { currentX, groundY, -groundExtent };
            Vector3D endPoint = { currentX, groundY,  groundExtent };
            worldLine.push_back({ startPoint, endPoint });
        }
        // Z���ɕ��s�Ȑ� (X�����ɐ�������)
        for (int i = -lineCount; i <= lineCount; ++i) {
            float currentZ = i * gridSize;
            Vector3D startPoint = { -groundExtent, groundY, currentZ };
            Vector3D endPoint = { groundExtent, groundY, currentZ };
            worldLine.push_back({ startPoint, endPoint });
        }
    }

    // --- �����̂̃��C���[�t���[���f�[�^�쐬 ---
    {
        Vector3D sphereCenter = { 80.0f, 0.0f, 80.0f }; // ���̒��S���W (�����̂̉E�����)
        float sphereRadius = 30.0f;                   // ���̔��a
        int latitudeDivisions = 12;                   // �ܓx�̕�����
        int longitudeDivisions = 24;                  // �o�x�̕�����

        // �ܐ� (�����ȉ~)
        for (int i = 1; i < latitudeDivisions; ++i) { // i=0 �� i=latitudeDivisions �͋ɂȂ̂ŏ���
            float phi = PI * ((float)i / latitudeDivisions - 0.5f); // -PI/2 �` PI/2 �͈̔�
            float y = sphereCenter.y + sphereRadius * sinf(phi);
            float r = sphereRadius * cosf(phi); // ���̍����ł̔��a

            Vector3D prevPoint;
            for (int j = 0; j <= longitudeDivisions; ++j) {
                float theta = 2.0f * PI * (float)j / longitudeDivisions; // 0 �` 2PI �͈̔�
                float x = sphereCenter.x + r * cosf(theta);
                float z = sphereCenter.z + r * sinf(theta);
                Vector3D currentPoint = { x, y, z };
                if (j > 0) {
                    worldLine.push_back({ prevPoint, currentPoint });
                }
                prevPoint = currentPoint;
            }
        }

        // �o�� (�����Ȕ��~)
        for (int j = 0; j < longitudeDivisions; ++j) {
            float theta = 2.0f * PI * (float)j / longitudeDivisions;
            Vector3D prevPoint;
            for (int i = 0; i <= latitudeDivisions; ++i) {
                float phi = PI * ((float)i / latitudeDivisions - 0.5f);
                float x = sphereCenter.x + sphereRadius * cosf(phi) * cosf(theta);
                float y = sphereCenter.y + sphereRadius * sinf(phi);
                float z = sphereCenter.z + sphereRadius * cosf(phi) * sinf(theta);
                Vector3D currentPoint = { x, y, z };
                if (i > 0) {
                    worldLine.push_back({ prevPoint, currentPoint });
                }
                prevPoint = currentPoint;
            }
        }
    }

    // --- �J�����ƃg�b�v�_�E���r���[�̐��� ---
    Camera* camera = new Camera();
    // !! �C��: TopAngle �̃R�����g�A�E�g������ !!
    TopAngle* topangle = new TopAngle(camera); // �ēx�L����

    // --- ���C�����[�v ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();
        camera->Update();
        LogDebug(camera->GetDetailedDebugInfo()); // �ڍ׏������O��

        camera->Draw(worldLine);
        // !! �C��: TopAngle �̕`����ēx�L���� !!
        topangle->Draw(worldLine); // �ēx�L����

        // �\���}�[�N�`�� (�ȗ�)
        { int cX = (int)(WINDOW_WIDTH / 2), cY = (int)(WINDOW_HEIGHT / 2), sz = 10; DrawLine(cX - sz, cY, cX + sz, cY, GetColor(255, 255, 0)); DrawLine(cX, cY - sz, cX, cY + sz, GetColor(255, 255, 0)); }

        // �f�o�b�O����ʕ\�� (�ȗ�)
        { std::string dt = camera->GetDebugInfo(); DrawFormatString(10, WINDOW_HEIGHT - 40, GetColor(255, 255, 255), dt.c_str()); } // Y���W����

        ScreenFlip();
    }

    // --- �I������ ---
    LogDebug("Application Ended.");
    Logger::GetInstance().Close();
    // !! �C��: topangle �� delete ���ēx�L���� !!
    delete topangle; // �ēx�L����
    delete camera;
    DxLib_End();
    return 0;
}