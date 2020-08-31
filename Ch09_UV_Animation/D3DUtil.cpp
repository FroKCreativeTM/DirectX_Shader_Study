#include "D3DUtil.h"

//----------------------------------------------------------------------
// ��������
//----------------------------------------------------------------------
// D3D ����
LPDIRECT3D9             gpD3D			= NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice		= NULL;				// D3D ��ġ
// ��Ʈ
ID3DXFont*              gpFont			= NULL;
// ��
LPD3DXMESH				g_pTorus		= nullptr;
// ���̴�
LPD3DXEFFECT			g_pUVAnimationShader	= nullptr;
// ������ ��ġ
D3DXVECTOR4				g_WorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// ī�޶� ��ġ
D3DXVECTOR4				g_WorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);
// �ؽ�ó
LPDIRECT3DTEXTURE9		g_pStoneDM = nullptr;
LPDIRECT3DTEXTURE9		g_pStoneSM = nullptr;
// ���� ����
D3DXVECTOR4				g_LightColor(0.7f, 0.7f, 1.0f, 1.0f);

// ȸ���� ���� ȸ����
float g_RotationY = 0.0f;

// �޽��� ó����
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

// Ű���� �Է�ó��
void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		// ESC Ű�� ������ ���α׷��� �����Ѵ�.
	case VK_ESCAPE:
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}

//------------------------------------------------------------
// ���ӷ���
//------------------------------------------------------------
void PlayDemo()
{
	Update();
	RenderFrame();
}

// ���ӷ��� ������Ʈ
void Update()
{
}

//------------------------------------------------------------
// ������
//------------------------------------------------------------

void RenderFrame()
{
	D3DCOLOR bgColour = 0x005289FF;	// ������ - �Ķ�

	gpD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), bgColour, 1.0f, 0);

	gpD3DDevice->BeginScene();
	{
		RenderScene();				// 3D ��ü���� �׸���.
		RenderInfo();				// ����� ���� ���� ����Ѵ�.
	}
	gpD3DDevice->EndScene();

	gpD3DDevice->Present(NULL, NULL, NULL, NULL);
}


// 3D ��ü���� �׸���.
void RenderScene()
{
	// �� ����� �����.
	D3DXMATRIXA16			matView;
	D3DXVECTOR3 vEyePt(g_WorldCameraPosition.x, g_WorldCameraPosition.y, g_WorldCameraPosition.z);			// ī�޶��� ��ġ
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);			// ī�޶� �ٶ󺸴� ����
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);			// �� ������ ����ΰ�. x:0, y:1, z:0
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// ��������� �����.
	D3DXMATRIXA16			matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);


	// ��������� �����.
	D3DXMATRIXA16			matWorld;
	D3DXMatrixIdentity(&matWorld);

	// ȸ���Ѵ�.
	g_RotationY += 0.4f * PI / 180.0f;
	if (g_RotationY > 2 * PI)
	{
		g_RotationY -= 2 * PI;
	}
	D3DXMatrixRotationY(&matWorld, g_RotationY);

	// �ý��� �ð��� ���Ѵ�.
	ULONGLONG tick = GetTickCount64();

	// ���̴� ������������ ����
	g_pUVAnimationShader->SetMatrix("g_WorldMatrix", &matWorld);
	g_pUVAnimationShader->SetMatrix("g_ViewMatrix", &matView);
	g_pUVAnimationShader->SetMatrix("g_ProjectionMatrix", &matProjection);

	g_pUVAnimationShader->SetVector("g_WorldLightPosition", &g_WorldLightPosition);
	g_pUVAnimationShader->SetVector("g_WorldCameraPosition", &g_WorldCameraPosition);

	g_pUVAnimationShader->SetFloat("g_Time", tick / 1000.0f);
	g_pUVAnimationShader->SetFloat("g_WaveHeight", 3.6f);			// �ĵ��� ���̸� �����ϱ� ���� ���� ���� 
	g_pUVAnimationShader->SetFloat("g_Speed", 2.0f);				// �󸶳� �ĵ��� ���� ġ����
	g_pUVAnimationShader->SetFloat("g_WaveFrequency", 10.0f);		// �󸶳� �ĵ��� ���� �Ͼ����
	g_pUVAnimationShader->SetFloat("g_UVSpeed", 0.5f);				// �ؽ�ó�� �����̴� �ӵ�

	// �ؽ�ó�� ������.
	g_pUVAnimationShader->SetVector("g_LightColor", &g_LightColor);
	g_pUVAnimationShader->SetTexture("DiffuseMap_Tex", g_pStoneDM);
	g_pUVAnimationShader->SetTexture("SpecularMap_Tex", g_pStoneSM);


	// ���̴��� �����Ѵ�.
	// ������ �׸� ��� ��ü�� �� ���̴��� ������ ���̶� ��
	UINT numPasses = 0;
	g_pUVAnimationShader->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			g_pUVAnimationShader->BeginPass(i);
			{
				// ��ü�� �׸���.
				g_pTorus->DrawSubset(0);
			}
			g_pUVAnimationShader->EndPass();
		}
	}
	// �� ���̴��� �����ϴ� ���� ������
	g_pUVAnimationShader->End();
}

// ����� ���� ���� ���.
void RenderInfo()
{
	// �ؽ�Ʈ ����
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	// �ؽ�Ʈ�� ����� ��ġ
	RECT rct;
	rct.left = 5;
	rct.right = WIN_WIDTH / 3;
	rct.top = 5;
	rct.bottom = WIN_HEIGHT / 3;

	// Ű �Է� ������ ���
	gpFont->DrawText(NULL, "���� �����ӿ�ũ\n\nESC: ��������", -1, &rct, 0, fontColor);
}

//------------------------------------------------------------
// �ʱ�ȭ �ڵ�
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D�� �ʱ�ȭ
	if (!InitD3D(hWnd))
	{
		return false;
	}

	// ��, ���̴�, �ؽ�ó���� �ε�
	if (!LoadAssets())
	{
		return false;
	}

	// ��Ʈ�� �ε�
	if (FAILED(D3DXCreateFont(gpD3DDevice, 20, 10, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_DONTCARE),
		"Arial", &gpFont)))
	{
		return false;
	}

	return true;
}

// D3D ��ü �� ��ġ �ʱ�ȭ
bool InitD3D(HWND hWnd)
{
	// D3D ��ü
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!gpD3D)
	{
		return false;
	}

	// D3D��ġ�� �����ϴµ� �ʿ��� ����ü�� ä���ִ´�.
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

	// D3D��ġ�� �����Ѵ�.
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
	// ���̴� �ε�
	g_pUVAnimationShader = LoadShader("UV_Animation.fx");
	if (!g_pUVAnimationShader)
	{
		return false;
	}

	// �� �ε�
	g_pTorus = LoadModel("Torus.x");
	if (!g_pTorus)
	{
		return false;
	}

	// �ؽ�ó �ε�
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


	return true;
}

// ���̴� �ε�
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

	// ���̴� �ε��� ������ ��� outputâ�� ���̴�
	// ������ ������ ����Ѵ�.
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

// �� �ε�
LPD3DXMESH LoadModel(const char* filename)
{
	LPD3DXMESH ret = NULL;
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, gpD3DDevice, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString("�� �ε� ����: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	};

	return ret;
}

// �ؽ�ó �ε�
LPDIRECT3DTEXTURE9 LoadTexture(const char* filename)
{
	LPDIRECT3DTEXTURE9 ret = NULL;
	if (FAILED(D3DXCreateTextureFromFile(gpD3DDevice, filename, &ret)))
	{
		OutputDebugString("�ؽ�ó �ε� ����: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	}

	return ret;
}
//------------------------------------------------------------
// ������ �ڵ�.
//------------------------------------------------------------

void Cleanup()
{
	// ��Ʈ�� release �Ѵ�.
	if (gpFont)
	{
		gpFont->Release();
		gpFont = NULL;
	}

	// ���� release �Ѵ�.
	if (g_pTorus)
	{
		g_pTorus->Release();
		g_pTorus = nullptr;
	}

	// ���̴��� release �Ѵ�.
	if (g_pUVAnimationShader)
	{
		g_pUVAnimationShader->Release();
		g_pUVAnimationShader = nullptr;
	}

	// �ؽ�ó�� release �Ѵ�.
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

	// D3D�� release �Ѵ�.
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

