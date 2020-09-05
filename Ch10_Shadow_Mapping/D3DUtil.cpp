#include "D3DUtil.h"

#pragma warning(disable:4996)

//----------------------------------------------------------------------
// 전역변수
//----------------------------------------------------------------------
// D3D 관련
LPDIRECT3D9             gpD3D			= NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice		= NULL;				// D3D 장치
// 폰트
ID3DXFont*              gpFont			= NULL;
// 모델
LPD3DXMESH				g_pTorus = nullptr;
LPD3DXMESH				g_pDisc		= nullptr;
// 쉐이더
LPD3DXEFFECT			g_pApplyShadowShader	= nullptr;
LPD3DXEFFECT			g_pCreateShadowShader	= nullptr;
// 조명의 위치
D3DXVECTOR4				g_WorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// 카메라 위치
D3DXVECTOR4				g_WorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);
// 그림자맵 렌더타겟(D3D의 텍스처)
LPDIRECT3DTEXTURE9		g_pShadowRenderTarget	= nullptr;
LPDIRECT3DSURFACE9		g_pShadowDepthStencil	= nullptr;
// 물체의 색상
D3DXVECTOR4				g_TorusColor(1.0f, 1.0f, 0.0f, 1.0f);
D3DXVECTOR4				g_DiscColor(0.0f, 1.0f, 1.0f, 1.0f);
// 빛의 색상
D3DXVECTOR4				g_LightColor(1.0f, 1.0f, 1.0f, 1.0f);

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
	// 광원-뷰행렬
	D3DXMATRIXA16			matLightView;
	{
		// 뷰 행렬을 만든다.
		D3DXVECTOR3 vEyePt(g_WorldLightPosition.x, 
			g_WorldLightPosition.y, 
			g_WorldLightPosition.z);			// 카메라의 위치
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);			// 카메라가 바라보는 방향
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);			// 윗 방향이 어디인가. x:0, y:1, z:0
		D3DXMatrixLookAtLH(&matLightView, &vEyePt, &vLookatPt, &vUpVec);
	}

	// 광원-투영 생성
	D3DXMATRIXA16			matLightProjection;
	{
		D3DXMatrixPerspectiveFovLH(&matLightProjection, D3DX_PI / 4.0f, 1, 1, 1000);
	}

	// 뷰-투영행렬
	D3DXMATRIXA16			matViewProjection;
	{
		// 뷰 행렬을 만든다.
		D3DXMATRIXA16		matView;
		D3DXVECTOR3 vEyePt(g_WorldCameraPosition.x, g_WorldCameraPosition.y, g_WorldCameraPosition.z);			// 카메라의 위치
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);			// 카메라가 바라보는 방향
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);			// 윗 방향이 어디인가. x:0, y:1, z:0
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

		D3DXMATRIXA16 matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);

		D3DXMatrixMultiply(&matViewProjection, &matView, &matProjection);
	}

	// 원환체의 월드 행렬 생성
	D3DXMATRIXA16			matTorusWorld;
	{
		// 회전한다.
		g_RotationY += 0.4f * PI / 180.0f;
		{
			if (g_RotationY > 2 * PI)
			{
				g_RotationY -= 2 * PI;
			}
			D3DXMatrixRotationY(&matTorusWorld, g_RotationY);
		}
	}

	// 디스크의 월드 행렬을 만든다.
	D3DXMATRIXA16			matDiscWorld;
	{
		D3DXMATRIXA16 matScale;
		D3DXMatrixScaling(&matScale, 2, 2, 2);

		D3DXMATRIXA16 matTrans;
		D3DXMatrixTranslation(&matTrans, 0, -40, 0);

		D3DXMatrixMultiply(&matDiscWorld, &matScale, &matTrans);
	}

	// 현재 하드웨어 백버퍼와 깊이버퍼
	LPDIRECT3DSURFACE9 pHWBackBuffer = nullptr;
	LPDIRECT3DSURFACE9 pHWDepthStencilBuffer = nullptr;
	gpD3DDevice->GetRenderTarget(0, &pHWBackBuffer);
	gpD3DDevice->GetDepthStencilSurface(&pHWDepthStencilBuffer);

	////////////////////////////
	//    1. 그림자 만들기	  //
	////////////////////////////

	// 그림자맵의 렌더타깃과 깊이 버퍼를 사용한다.
	LPDIRECT3DSURFACE9 pShadowSurface = nullptr;
	if (SUCCEEDED(g_pShadowRenderTarget->GetSurfaceLevel(0, &pShadowSurface)))
	{
		gpD3DDevice->SetRenderTarget(0, pShadowSurface);
		pShadowSurface->Release();
		pShadowSurface = nullptr;
	}
	gpD3DDevice->SetDepthStencilSurface(g_pShadowDepthStencil);

	gpD3DDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), 0xFFFFFFFF, 1.0f, 0);

	g_pCreateShadowShader->SetMatrix("g_WorldMatrix", &matTorusWorld);
	g_pCreateShadowShader->SetMatrix("g_LightViewMatrix", &matLightView);
	g_pCreateShadowShader->SetMatrix("g_LightProjectionMatrix", &matLightProjection);

	// 그림자 만들기 셰이더 시작
	{
		// 쉐이더를 시작한다.
		// 앞으로 그릴 모든 물체에 이 쉐이더를 적용할 것이란 뜻
		UINT numPasses = 0;
		g_pCreateShadowShader->Begin(&numPasses, NULL);
		{
			for (UINT i = 0; i < numPasses; ++i)
			{
				g_pCreateShadowShader->BeginPass(i);
				{
					// 구체를 그린다.
					g_pTorus->DrawSubset(0);
				}
				g_pCreateShadowShader->EndPass();
			}
		}
		// 이 쉐이더를 적용하는 것을 끝내라
		g_pCreateShadowShader->End();
	}

	////////////////////////////
	//    2. 그림자 입히기	  //
	////////////////////////////

	// 하드웨어 백버퍼/깊이버퍼를 사용한다.
	gpD3DDevice->SetRenderTarget(0, pHWBackBuffer);
	gpD3DDevice->SetDepthStencilSurface(pHWDepthStencilBuffer);

	// GPU 메모리 누수 방지
	// D3D 자원들은 사용자가 직접 메모리를 할당/제거를 하지 않는다
	// 그 대신 참조 카운트가 0으로 떨어진다면 D3D 내부에서 알아서 해제한다. (unique_ptr같은 것)
	pHWBackBuffer->Release();
	pHWBackBuffer = nullptr;
	pHWDepthStencilBuffer->Release();
	pHWDepthStencilBuffer = nullptr;

	// 그림자 입히기 셰이더 전역변수들을 설정
	g_pApplyShadowShader->SetMatrix("g_WorldMatrix", &matTorusWorld);
	g_pApplyShadowShader->SetMatrix("g_ViewProjectionMatrix", &matViewProjection);
	g_pApplyShadowShader->SetMatrix("g_LightViewMatrix", &matLightView);
	g_pApplyShadowShader->SetMatrix("g_LightProjectionMatrix", & matLightProjection);

	// 난반사용 전역변수 설정
	g_pApplyShadowShader->SetVector("g_WorldLightPosition", &g_WorldLightPosition);
	g_pApplyShadowShader->SetVector("g_ObjectColor", &g_TorusColor);

	// 그림자를 입히기 위한 그림자맵
	g_pApplyShadowShader->SetTexture("ShadowMap_Tex", g_pShadowRenderTarget);

	{
		// 쉐이더를 시작한다.
		// 앞으로 그릴 모든 물체에 이 쉐이더를 적용할 것이란 뜻
		UINT numPasses = 0;
		g_pApplyShadowShader->Begin(&numPasses, NULL);
		{
			for (UINT i = 0; i < numPasses; ++i)
			{
				g_pApplyShadowShader->BeginPass(i);
				{
					// 구체를 그린다.
					g_pTorus->DrawSubset(0);

					// 디스크를 그린다.
					g_pApplyShadowShader->SetMatrix("g_WorldMatrix", &matDiscWorld);
					g_pApplyShadowShader->SetVector("g_ObjectColor", &g_DiscColor);

					// BeginPass와 EndPass에서 전역변수를 변화시키려면 CommitChanges라는 함수를 호출해야한다.
					g_pApplyShadowShader->CommitChanges();
					g_pDisc->DrawSubset(0);
				}
				g_pApplyShadowShader->EndPass();
			}
		}
		// 이 쉐이더를 적용하는 것을 끝내라
		g_pApplyShadowShader->End();
	}
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

	// 렌더 타깃 생성
	const int shadowMapSize = 2048;

	// 렌더 타깃 텍스처 생성
	if (FAILED(gpD3DDevice->CreateTexture(shadowMapSize, shadowMapSize,
		1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
		D3DPOOL_DEFAULT, &g_pShadowRenderTarget, nullptr)))
	{
		return false;
	}

	// 그림자맵과 동일한 크기의 깊이 버퍼를 만들어줘야 한다.
	// D3DFMT_D24X8 : 깊이 버퍼로 24비트, 나머지 8비트는 버리기
	// D3DMULTISAMPLE_NONE : 안티앨리어싱 사용 안 함
	if (FAILED(gpD3DDevice->CreateDepthStencilSurface(shadowMapSize, shadowMapSize,
		D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE,
		&g_pShadowDepthStencil, nullptr)))
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
	g_pCreateShadowShader = LoadShader("CreateShadow.fx");
	if (!g_pCreateShadowShader)
	{
		return false;
	}

	g_pApplyShadowShader = LoadShader("ApplyShadow.fx");
	if (!g_pApplyShadowShader)
	{
		return false;
	}

	// 모델 로딩
	g_pDisc = LoadModel("Disc.x");
	if (!g_pDisc)
	{
		return false;
	}

	g_pTorus = LoadModel("Torus.x");
	if (!g_pTorus)
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
	if (g_pDisc)
	{
		g_pDisc->Release();
		g_pDisc = nullptr;
	}

	if (g_pTorus)
	{
		g_pTorus->Release();
		g_pTorus = nullptr;
	}

	// 쉐이더를 release 한다.
	if (g_pCreateShadowShader)
	{
		g_pCreateShadowShader->Release();
		g_pCreateShadowShader = nullptr;
	}

	if (g_pApplyShadowShader)
	{
		g_pApplyShadowShader->Release();
		g_pApplyShadowShader = nullptr;
	}

	// 텍스처를 release 한다.
	if (g_pShadowRenderTarget)
	{
		g_pShadowRenderTarget->Release();
		g_pShadowRenderTarget = nullptr;
	}

	if (g_pShadowDepthStencil)
	{
		g_pShadowDepthStencil->Release();
		g_pShadowDepthStencil = nullptr;
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

