#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define WM_SOCKET WM_USER+1

extern HWND hWnd;
SOCKET client_socket;

BOOL startup_client()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return FALSE;
	}

	client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client_socket == INVALID_SOCKET)
	{
		return FALSE;
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	ZeroMemory(server_addr.sin_zero, sizeof(server_addr.sin_zero));
	
	if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	WSAAsyncSelect(client_socket, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

	return TRUE;
}

void cleanup_client()
{
	closesocket(client_socket);

	WSACleanup();
}
