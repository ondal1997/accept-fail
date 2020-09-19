#pragma once

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define WM_SOCKET WM_USER+1

extern HWND hWnd;
SOCKET server_socket;

BOOL startup_server()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return FALSE;
	}

	server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_socket == INVALID_SOCKET)
	{
		return FALSE;
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(8000);
	ZeroMemory(server_addr.sin_zero, sizeof(server_addr.sin_zero));

	if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	if (listen(server_socket, 1) == SOCKET_ERROR)
	{
		return FALSE;
	}

	WSAAsyncSelect(server_socket, hWnd, WM_SOCKET, FD_ACCEPT);

	return TRUE;
}

void cleanup_server()
{
	closesocket(server_socket);

	WSACleanup();
}
