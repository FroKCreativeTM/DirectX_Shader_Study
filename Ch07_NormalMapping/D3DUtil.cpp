#include "D3DUtil.h"

//----------------------------------------------------------------------
// 전역변수
//----------------------------------------------------------------------
// D3D 관련
LPDIRECT3D9             gpD3D			= NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice		= NULL;				// D3D 장치
// 폰트
ID3DXFont*              gpFont			= NULL;
// 모델
LPD3DXMESH				gpSphere		= NULL;
// 쉐이더
LPD3DXEFFECT			g_pNormalMappingShader	= nullptr;
// 조명의 위치
D3DXVECTOR4				g_WorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// 카메라 위치
D3DXVECTOR4				g_WorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);
// 텍스처
LPDIRECT3DTEXTURE9		g_pStoneDM = nullptr;		// 디퓨즈맵
LPDIRECT3DTEXTURE9		g_pStoneSM = nullptr;		// 스페큘러맵
LPDIRECT3DTEXTURE9		g_pStoneNM = nullptr;		// 노멀맵
// 빛의 색상
D3DXVECTOR4				g_LightColor(0.7f, 0.7f, 1.0f, 1.0f);

// 회전을 위한 회전값
float g_RotationY = 0.0f;

// 메시지 처리기
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		ProcessInput(hWnd, wParam);
		break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 키보드 입력처리
void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		// ESC 키가 눌리면 프로그램을 종료한다.
	case VK_ESCAPE:
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}

//------------------------------------------------------------
// 게임루프
//------------------------------------------------------------
void PlayDemo()
{
	Update();
	RenderFrame();
}

// 게임로직 업데이트
void Update()
{
}

//------------------------------------------------------------
// 렌더링
//------------------------------------------------------------

void RenderFrame()
{
	D3DCOLOR bgColour = 0x005289FF;	// 배경색상 - 파랑

	gpD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), bgColour, 1.0f, 0);

	gpD3DDevice->BeginScene();
	{
		RenderScene();				// 3D 물체등을 그린다.
		RenderInfo();				// 디버그 정보 등을 출력한다.
	}
	gpD3DDevice->EndScene();

	gpD3DDevice->Present(NULL, NULL, NULL, NULL);
}


// 3D 물체등을 그린다.
void RenderScene()
{
	// 뷰 행렬을 만든다.
	D3DXMATRIXA16			matView;
	D3DXVECTOR3 vEyePt(g_WorldCameraPosition.x, g_WorldCameraPosition.y, g_WorldCameraPosition.z);			// 카메라의 위치
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);			// 카메라가 바라보는 방향
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);			// 윗 방향이 어디인가. x:0, y:1, z:0
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// 투영행렬을 만든다.
	D3DXMATRIXA16			matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);

	// 회전한다.
	g_RotationY += 0.4f * PI / 180.0f;
	if (g_RotationY > 2 * PI)
	{
		g_RotationY -= 2 * PI;
	}

	// 월드행렬을 만든다.
	D3DXMATRIXA16			matWorld;
	D3DXMatrixRotationY(&matWorld, g_RotationY);

	// 월드/뷰/투영행렬을 미리 곱한다.
	D3DXMATRIXA16 matWorldView;
	D3DXMATRIXA16 matWorldViewProjection;
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProjection, &matWorldView, &matProjection);

	// 쉐이더 전역변수들을 설정
	g_pNormalMappingShader->SetMatrix("g_WorldMatrix", &matWorld);
	g_pNormalMappingShader->SetMatrix("g_WorldViewProjectionMatrix", &matWorldViewProjection);

	g_pNormalMappingShader->SetVector("g_WorldLightPosition", &g_WorldLightPosition);
	g_pNormalMappingShader->SetVector("g_WorldCameraPosition", &g_WorldCameraPosition);

	// 텍스처를 입힌다.
	g_pNormalMappingShader->SetVector("g_LightColor", &g_LightColor);
	g_pNormalMappingShader->SetTexture("DiffuseMap_Tex", g_pStoneDM);
	g_pNormalMappingShader->SetTexture("SpecularMap_Tex", g_pStoneSM);
	g_pNormalMappingShader->SetTexture("NormalMap_Tex", g_pStoneNM);


	// 쉐이더를 시작한다.
	// 앞으로 그릴 모든 물체에 이 쉐이더를 적용할 것이란 뜻
	UINT numPasses = 0;
	g_pNormalMappingShader->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			g_pNormalMappingShader->BeginPass(i);
			{
				// 구체를 그린다.
				gpSphere->DrawSubset(0);
			}
			g_pNormalMappingShader->EndPass();
		}
	}
	// 이 쉐이더를 적용하는 것을 끝내라
	g_pNormalMappingShader->End();
}

// 디버그 정보 등을 출력.
void RenderInfo()
{
	// 텍스트 색상
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	// 텍스트를 출력할 위치
	RECT rct;
	rct.left = 5;
	rct.right = WIN_WIDTH / 3;
	rct.top = 5;
	rct.bottom = WIN_HEIGHT / 3;

	// 키 입력 정보를 출력
	gpFont->DrawText(NULL, "데모 프레임워크\n\nESC: 데모종료", -1, &rct, 0, fontColor);
}

//------------------------------------------------------------
// 초기화 코드
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D를 초기화
	if (!InitD3D(hWnd))
	{
		return false;
	}

	// 모델, 쉐이더, 텍스처등을 로딩
	if (!LoadAssets())
	{
		return false;
	}

	// 폰트를 로딩
	if (FAILED(D3DXCreateFont(gpD3DDevice, 20, 10, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_DONTCARE),
		"Arial", &gpFont)))
	{
		return false;
	}

	return true;
}

// D3D 객체 및 장치 초기화
bool InitD3D(HWND hWnd)
{
	// D3D 객체
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!gpD3D)
	{
		return false;
	}

	// D3D장치를 생성하는데 필요한 구조체를 채워넣는다.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = WIN_WIDTH;
	d3dpp.BackBufferHeight = WIN_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// D3D장치를 생성한다.
	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
	}

	return true;
}

bool LoadAssets()
{
	// 쉐이더 로딩
	g_pNormalMappingShader = LoadShader("NormalMapping.fx");
	if (!g_pNormalMappingShader)
	{
		return false;
	}

	// 모델 로딩
	gpSphere = LoadModel("SphereWithTangent.x");
	if (!gpSphere)
	{
		return false;
	}

	// 텍스처 로딩
	g_pStoneSM = LoadTexture("fieldstone_SM.tga");
	if (!g_pStoneSM)
	{
		return false;
	}

	g_pStoneDM = LoadTexture("Fieldstone_DM.tga");
	if (!g_pStoneDM)
	{
		return false;
	}

	g_pStoneNM = LoadTexture("fieldstone_NM.tga");
	if (!g_pStoneNM)
	{
		return false;
	}

	return true;
}

// 쉐이더 로딩
LPD3DXEFFECT LoadShader(const char* filename)
{
	LPD3DXEFFECT ret = NULL;

	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	D3DXCreateEffectFromFile(gpD3DDevice, filename,
		NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

	// 쉐이더 로딩에 실패한 경우 output창에 쉐이더
	// 컴파일 에러를 출력한다.
	if (!ret && pError)
	{
		int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
			sprintf(str, (const char*)ack, size);
			OutputDebugString(str);
			delete[] str;
		}
	}

	return ret;
}

// 모델 로딩
LPD3DXMESH LoadModel(const char* filename)
{
	LPD3DXMESH ret = NULL;
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, gpD3DDevice, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString("모델 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	};

	return ret;
}

// 텍스처 로딩
LPDIRECT3DTEXTURE9 LoadTexture(const char* filename)
{
	LPDIRECT3DTEXTURE9 ret = NULL;
	if (FAILED(D3DXCreateTextureFromFile(gpD3DDevice, filename, &ret)))
	{
		OutputDebugString("텍스처 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	}

	return ret;
}
//------------------------------------------------------------
// 뒷정리 코드.
//------------------------------------------------------------

void Cleanup()
{
	// 폰트를 release 한다.
	if (gpFont)
	{
		gpFont->Release();
		gpFont = NULL;
	}

	// 모델을 release 한다.
	if (gpSphere)
	{
		gpSphere->Release();
		gpSphere = NULL;
	}

	// 쉐이더를 release 한다.
	if (g_pNormalMappingShader)
	{
		g_pNormalMappingShader->Release();
		g_pNormalMappingShader = nullptr;
	}

	// 텍스처를 release 한다.
	if (g_pStoneNM)
	{
		g_pStoneNM->Release();
		g_pStoneNM = nullptr;
	}

	if (g_pStoneDM)
	{
		g_pStoneDM->Release();
		g_pStoneDM = nullptr;
	}

	if (g_pStoneSM)
	{
		g_pStoneSM->Release();
		g_pStoneSM = nullptr;
	}

	// D3D를 release 한다.
	if (gpD3DDevice)
	{
		gpD3DDevice->Release();
		gpD3DDevice = NULL;
	}

	if (gpD3D)
	{
		gpD3D->Release();
		gpD3D = NULL;
	}
}

