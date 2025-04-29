#pragma once
#include <vector> // ���̃t�@�C���� <vector> ���g���\�������邽�߁A�O�̂��߃C���N���[�h

/*
 * Common.h
 * ����: �v���W�F�N�g�S�̂ŋ��L����萔���`���܂��B�E�B���h�E�T�C�Y�A���w�萔�A
 *       �����_�����O�֘A�̃p�����[�^�Ȃǂ��ꌳ�Ǘ����܂��B
 *
 * �ŏ��̃R�[�h����̕ύX�_�E�R�����g:
 * - �������O��� (`namespace {}`) ���g�p���Ă��܂��B
 *   ����ɂ��A�����Œ�`���ꂽ�萔�͊ecpp�t�@�C�����œ��������P�[�W�������܂��B
 *   �A�N�Z�X���ɖ��O��ԏC���q�͕s�v�ł��i��: `PI` �ƒ��ڋL�q�j�B
 * - `static const` ���� `const` �ɕύX�i�������O��ԓ��ł� `const` �����œ��������P�[�W�ɂȂ邽�߁j�B
 * - `PI` �̒l����萸�x�̍���float�l�ɕύX���܂����B
 * - `SCREEN` �萔�̈Ӗ��i�ˉe���ʂ܂ł̋����j�ɂ��ăR�����g��ǉ����܂����B
 */

namespace // �������O���: �����Œ�`���ꂽ�萔�̓t�@�C���X�R�[�v�i���������P�[�W�j������
{
    // ���w�萔
    const float PI = 3.1415926535f;       // �~���� (float)
    const float ONE_DEGREE = PI / 180.0f; // 1�x�����W�A���ɕϊ�����W��

    // �E�B���h�E�ݒ�
    const float WINDOW_WIDTH = 800.f;     // �E�B���h�E�̕�
    const float WINDOW_HEIGHT = 600.f;    // �E�B���h�E�̍���

    // �J�����E�����_�����O�ݒ�
    const float SCREEN = 500.f;           // �ˉe���� (���z�X�N���[��) �܂ł̋���
    // ���̒l���������e�̋����ɉe�����܂��B
}