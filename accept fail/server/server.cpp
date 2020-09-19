#include "framework.h"
#include "server.h"
#include "wsa_server.h"

#define MAX_LOADSTRING 100

HWND hWnd;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

#include <map>

std::map<SOCKET, int> clients;

void doAccept()
{
	SOCKADDR_IN client_addr;
	int addrlen = sizeof(client_addr);
	SOCKET client_socket = accept(accept_socket, (SOCKADDR*)&client_addr, &addrlen);
	// const char* client_ip = inet_ntoa(client_addr.sin_addr);

	if (client_socket == INVALID_SOCKET)
		return;

	WSAAsyncSelect(client_socket, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	clients[client_socket] = 0;
}

void doRead(SOCKET client_socket)
{
	auto it = clients.find(client_socket);

	if (it == clients.end())
	{
		return;
	}

	// recv(client_socket, buf, len, 0);

	auto& value = it->second;
}

void doClose(SOCKET client_socket, bool flag)
{
	auto it = clients.find(client_socket);

	if (it == clients.end())
	{
		return;
	}

	if (flag)
	{
		LINGER linger = { 1, 0 };
		setsockopt(client_socket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));
	}

	closesocket(client_socket);

	clients.erase(it);

	auto& value = it->second;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (!startup_server())
	{
		return FALSE;
	}

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		switch (msg.message)
		{
		case WM_SOCKET:
			switch (WSAGETSELECTEVENT(msg.lParam))
			{
			case FD_ACCEPT:
				doAccept();
				break;
			case FD_READ:
				doRead((SOCKET)msg.wParam);
				break;
			case FD_CLOSE:
				doClose((SOCKET)msg.wParam, false);
				break;
			}
			break;
		default:
			DispatchMessage(&msg);
		}
	}

	cleanup_server();

	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
