#pragma once

// ---------- ��� ���� ------------------------------------
const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

const float   PI = 3.14159265f;
const float   FOV = (PI / 4.0f);								// �þ߰�
const float   ASPECT_RATIO = (WIN_WIDTH / (float)WIN_HEIGHT);	// ȭ���� ��Ⱦ��
const float   NEAR_PLANE = 1;									// ���� ���
const float   FAR_PLANE = 10000;								// ���Ÿ� ���

static const char* gAppName = "�ʰ��� ���̴� ���� �����ӿ�ũ";	// ���α׷� �̸�