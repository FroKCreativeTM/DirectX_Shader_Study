#include "D3DUtil.h"

// ������
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    // ������ Ŭ������ ����Ѵ�.
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      gAppName, NULL };
    RegisterClassEx(&wc);

    // ���α׷� â�� �����Ѵ�.
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    HWND hWnd = CreateWindow(gAppName, gAppName,
        style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);

    // Client Rect ũ�Ⱑ WIN_WIDTH, WIN_HEIGHT�� ������ ũ�⸦ �����Ѵ�.
    POINT ptDiff;
    RECT rcClient, rcWindow;

    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    // D3D�� ����� ��� ���� �ʱ�ȭ�Ѵ�.
    if (!InitEverything(hWnd))
        PostQuitMessage(1);

    // �޽��� ����
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else // �޽����� ������ ������ ������Ʈ�ϰ� ����� �׸���
        {
            PlayDemo();
        }
    }

    UnregisterClass(gAppName, wc.hInstance);
    return 0;
}