#pragma once

// ---------- 상수 선언 ------------------------------------
const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

const float   PI = 3.14159265f;
const float   FOV = (PI / 4.0f);								// 시야각
const float   ASPECT_RATIO = (WIN_WIDTH / (float)WIN_HEIGHT);	// 화면의 종횡비
const float   NEAR_PLANE = 1;									// 근접 평면
const float   FAR_PLANE = 10000;								// 원거리 평면

static const char* gAppName = "초간단 쉐이더 데모 프레임워크";	// 프로그램 이름