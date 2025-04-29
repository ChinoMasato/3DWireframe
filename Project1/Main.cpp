// !! NOMINMAX �̓t�@�C���̐擪 !!
#define NOMINMAX
#include "Common.h"     // ���ʒ萔
#include "DxLib.h"      // DxLib ���C�u����
#include "Camera.h"     // Camera �N���X
#include "TopAngle.h"   // TopAngle �N���X
#include "Vector.h"     // Vector3D �\����
#include <vector>       // std::vector
#include <string>       // std::string
#include "Logger.h"     // Logger �N���X (���O�o�͗p)
#include <cmath>        // sinf, cosf (<math.h> ��萄��)

/*
 * Main.cpp
 * ����:
 *   ���̃t�@�C���̓v���O�����S�̂̊J�n�_�ƂȂ� WinMain�֐� ���܂�ł��܂��B
 *   �A�v���P�[�V�����̋N������I���܂ł̗�����Ǘ����܂��B
 *   ��ȏ������e�͈ȉ��̒ʂ�ł��B
 *   - DxLib���C�u�����̏������ƃE�B���h�E�ݒ�
 *   - ���O�o�͋@�\(Logger)�̏���
 *   - 3D��Ԃɕ\������I�u�W�F�N�g�i�����́A�n�ʃO���b�h�A���́j�̐��f�[�^�쐬
 *   - �J����(`Camera`)�ƃg�b�v�_�E���r���[(`TopAngle`)�̃I�u�W�F�N�g����
 *   - ���C�����[�v�F�v���O�������I������܂ŌJ��Ԃ����s����镔��
 *     - ���͏��� (DxLib��ProcessMessage)
 *     - �J�����ƃI�u�W�F�N�g�̏�ԍX�V (camera->Update)
 *     - ���O�o�� (LogDebug)
 *     - ��ʂւ̕`�� (camera->Draw, topangle->Draw, �f�o�b�O�\��)
 *     - ��ʂ̍X�V (ScreenFlip)
 *   - �v���O�����I�����̌�Еt�� (�I�u�W�F�N�g�̉���ADxLib�I������)
 *
 * ���̃o�[�W�����ł̎�ȕύX�_�E�ǉ��_ (���� Main.cpp ����̕ύX�_):
 * 1. ���O�o�͋@�\�̗��p (Logger�N���X):
 *    - �v���O�����̓���󋵂��t�@�C��("debug_log.txt")�ɋL�^���� Logger �N���X�𗘗p���܂��B
 *    - �v���O�����̊J�n�E�I�����b�Z�[�W��A���t���[���̃J�����̏ڍׂȏ�ԂȂǂ��L�^���邱�ƂŁA
 *      ����m�F���蔭�����̌��������ɖ𗧂��܂��B
 *
 * 2. �`�悷��3D�I�u�W�F�N�g�̒ǉ��E�ύX:
 *    - �n�ʃO���b�h: �\���͈͂��L���AY���W�𒲐����đ��̃I�u�W�F�N�g�Ƃ̈ʒu�֌W�𕪂���₷�����܂����B
 *    - ���̃I�u�W�F�N�g: �V���Ƀ��C���[�t���[���̋��̂�ǉ����܂����B����ɂ��A�J�������猩���Ƃ���
 *                    �Ȗʂ̕\����A��蕡�G�ȃV�[���ł̓�����m�F�ł��܂��B
 *
 * 3. �g�b�v�_�E���r���[�\���̗L���� (TopAngle�N���X):
 *    - `TopAngle` �N���X�̃I�u�W�F�N�g�𐶐����A���t���[���`�悷��悤�ɂ��܂����B
 *    - ����ɂ��A��ʍ���Ƀ��[���h���ォ�猩��2D�}�b�v���\������A
 *      ���C���J�����̈ʒu������A�I�u�W�F�N�g�̔z�u���q�ϓI�ɔc���ł��܂��B
 *
 * 4. �f�o�b�O�p�\���̋���:
 *    - ��ʒ����ɏ\���}�[�N��\�����A�J�����̒��S�i�����_�j�𕪂���₷�����܂����B
 *    - ��ʍ����ɃJ�����̊�{�I�ȏ��i���W�A���͏�ԂȂǁj�����A���^�C���ŕ\������悤�ɂ��܂����B
 *      ����ɂ��A���삪���������f����Ă��邩�Ȃǂ�f�����m�F�ł��܂��B
 *
 * �����̕ύX�́A��ɃJ�����@�\�̊J����e�X�g�A�f�o�b�O���s���₷�����邱�Ƃ�ړI�Ƃ��Ă��܂��B
 *
 * ���ӓ_:
 * - ���̃v���O���������s����ɂ́A`Camera`, `TopAngle`, `Logger`, `Vector`, `Matrix`, `Quaternion`,
 *   `CameraMath`, `Common` �Ƃ������A�֘A����S�ẴN���X��w�b�_�[�t�@�C����
 *   ��������������A�v���W�F�N�g�Ɋ܂܂�Ă���K�v������܂��B
 */

 // ���C���[�t���[���̗����̂̒��_�f�[�^�𐶐�����w���p�[�֐�
std::vector<std::vector<Vector3D>> CreateCubeLines(float size, const Vector3D& center) {
    std::vector<std::vector<Vector3D>> lines;
    float halfSize = size * 0.5f;
    Vector3D vertices[8] = {
        center + Vector3D{-halfSize, -halfSize, -halfSize}, center + Vector3D{ halfSize, -halfSize, -halfSize},
        center + Vector3D{ halfSize,  halfSize, -halfSize}, center + Vector3D{-halfSize,  halfSize, -halfSize},
        center + Vector3D{-halfSize, -halfSize,  halfSize}, center + Vector3D{ halfSize, -halfSize,  halfSize},
        center + Vector3D{ halfSize,  halfSize,  halfSize}, center + Vector3D{-halfSize,  halfSize,  halfSize}
    };
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    for (const auto& edge : edges) {
        lines.push_back({ vertices[edge[0]], vertices[edge[1]] });
    }
    return lines;
}

// ���̂̃��C���[�t���[���f�[�^�𐶐�����w���p�[�֐�
std::vector<std::vector<Vector3D>> CreateSphereLines(const Vector3D& center, float radius, int latDivs, int lonDivs) {
    std::vector<std::vector<Vector3D>> lines;
    // �ܐ�
    for (int i = 1; i < latDivs; ++i) {
        float phi = PI * ((float)i / latDivs - 0.5f);
        float y = center.y + radius * sinf(phi);
        float r = radius * cosf(phi);
        Vector3D prevPoint;
        for (int j = 0; j <= lonDivs; ++j) {
            float theta = 2.0f * PI * (float)j / lonDivs;
            float x = center.x + r * cosf(theta);
            float z = center.z + r * sinf(theta);
            Vector3D currentPoint = { x, y, z };
            if (j > 0) { lines.push_back({ prevPoint, currentPoint }); }
            prevPoint = currentPoint;
        }
    }
    // �o��
    for (int j = 0; j < lonDivs; ++j) {
        float theta = 2.0f * PI * (float)j / lonDivs;
        Vector3D prevPoint;
        for (int i = 0; i <= latDivs; ++i) {
            float phi = PI * ((float)i / latDivs - 0.5f);
            float x = center.x + radius * cosf(phi) * cosf(theta);
            float y = center.y + radius * sinf(phi);
            float z = center.z + radius * cosf(phi) * sinf(theta);
            Vector3D currentPoint = { x, y, z };
            if (i > 0) { lines.push_back({ prevPoint, currentPoint }); }
            prevPoint = currentPoint;
        }
    }
    return lines;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib �������t�F�[�Y ---
    ChangeWindowMode(TRUE); // �E�B���h�E���[�h
    SetWindowSizeChangeEnableFlag(FALSE); // �T�C�Y�ύX�s��
    SetGraphMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), 32); // ��ʃ��[�h�ݒ�
    if (DxLib_Init() < 0) { // DxLib������
        MessageBox(NULL, TEXT("DxLib �̏������Ɏ��s���܂����B"), TEXT("�G���["), MB_OK); // �G���[�\��
        return -1;
    }
    SetMouseDispFlag(FALSE);       // �}�E�X�J�[�\����\��
    SetDrawScreen(DX_SCREEN_BACK); // �`���𗠉�ʂɐݒ�

    // --- ���K�[�̏����� ---
    if (!Logger::GetInstance().Open("debug_log.txt")) { // ���O�t�@�C�����J��
        printfDx("�x��: �f�o�b�O���O�t�@�C�����J���܂���ł����B\n"); // ���s������x��
    }
    LogDebug("�A�v���P�[�V�������J�n���܂����B"); // �J�n���O���o��

    // --- �I�u�W�F�N�g�f�[�^�̏��� ---
    std::vector<std::vector<Vector3D>> worldLine; // �`�悷��������i�[���郊�X�g

    // --- �����̂̕Ӄf�[�^�쐬 ---
    auto cubeLines = CreateCubeLines(50.0f, { 0.0f, 0.0f, 50.0f }); // �T�C�Y50, ���S(0,0,50)
    worldLine.insert(worldLine.end(), cubeLines.begin(), cubeLines.end());

    // --- �n�ʃO���b�h�f�[�^�쐬 ---
    {
        float groundY = -25.0f; // �n�ʂ�Y���W
        float groundExtent = 500.0f;
        float gridSize = 10.0f;
        int lineCount = static_cast<int>(groundExtent / gridSize);
        for (int i = -lineCount; i <= lineCount; ++i) { // X���s��
            worldLine.push_back({ { (float)i * gridSize, groundY, -groundExtent }, { (float)i * gridSize, groundY,  groundExtent } });
        }
        for (int i = -lineCount; i <= lineCount; ++i) { // Z���s��
            worldLine.push_back({ { -groundExtent, groundY, (float)i * gridSize }, {  groundExtent, groundY, (float)i * gridSize } });
        }
    }

    // --- ���̂̃��C���[�t���[���f�[�^�쐬 ---
    {
        auto sphereLines = CreateSphereLines({ 80.0f, 0.0f, 80.0f }, 30.0f, 12, 24); // ���S(80,0,80), ���a30
        worldLine.insert(worldLine.end(), sphereLines.begin(), sphereLines.end());
    }

    // --- �J�����ƃg�b�v�_�E���r���[�̐��� ---
    Camera* camera = new Camera(); // Camera�I�u�W�F�N�g����
    TopAngle* topangle = new TopAngle(camera); // TopAngle�I�u�W�F�N�g����

    // --- ���C�����[�v ---
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) // �E�B���h�E�������邩ESC���������܂�
    {
        // 1. ��ʃN���A
        ClearDrawScreen();

        // 2. �X�V����
        camera->Update(); // �J�����̏�ԍX�V
        LogDebug(camera->GetDetailedDebugInfo()); // �J�����ڍ׏������O��

        // 3. �`�揈��
        camera->Draw(worldLine);   // ���C���J�������_�`��
        topangle->Draw(worldLine); // �g�b�v�_�E���r���[�`��

        // 4. UI�E�f�o�b�O�\���`��
        // ��ʒ����ɏ\���}�[�N�`��
        {
            int cX = static_cast<int>(WINDOW_WIDTH / 2), cY = static_cast<int>(WINDOW_HEIGHT / 2), sz = 10;
            unsigned int color = GetColor(255, 255, 0); // ���F
            DrawLine(cX - sz, cY, cX + sz, cY, color);
            DrawLine(cX, cY - sz, cX, cY + sz, color);
        }

        // ��ʍ����ɃJ�����̊ȈՃf�o�b�O����\��
        {
            std::string dt = camera->GetDebugInfo(); // �J�������擾
            DrawFormatString(10, static_cast<int>(WINDOW_HEIGHT) - 40, GetColor(255, 255, 255), dt.c_str());
        }

        // 5. ��ʍX�V
        ScreenFlip(); // ����ʂ�\��ʂɕ\��
    }

    // --- �I������ ---
    LogDebug("�A�v���P�[�V�������I�����܂��B");
    Logger::GetInstance().Close(); // ���K�[�I��

    // new �Ŋm�ۂ��������������
    delete topangle;
    delete camera;

    DxLib_End(); // DxLib�I��
    return 0;
}