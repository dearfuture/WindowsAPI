#include<winsock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<tchar.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT	"10000"
#define MAX_REQUEST		1024
#define BUF_SIZE		4096

DWORD WINAPI CommunicationThread(LPVOID lpParameter)
{
	DWORD dwTid = GetCurrentThreadId();
	SOCKET socket = (SOCKET)lpParameter;
	LPSTR  szRequest = HeapAlloc(GetProcessHeap(), 0, MAX_REQUEST);
	int iResult;
	int bytesSent;

	iResult = recv(socket, szRequest, MAX_REQUEST, 0);
	if (0 == iResult)
	{
		printf("Connection closing...\n");
		HeapFree(GetProcessHeap(), 0, szRequest);
		closesocket(socket);
		return -1;
	}
	else if (iResult == SOCKET_ERROR)
	{
		_tprintf(_T("recv failed: %d\n"), WSAGetLastError());
		HeapFree(GetProcessHeap(), 0, szRequest);
		closesocket(socket);
		return 1;
	}
	else if (iResult > 0)
	{
		printf("\tCommunicationThread(%d)\tBytes received: %d\n", dwTid, iResult);
		printf("\tCommunicationThread(%d)request string is(%s)\n", dwTid, szRequest);

		bytesSent = send(socket, "this is information", strlen("this is information") + 1, 0);
		if (SOCKET_ERROR == bytesSent)
		{
			_tprintf(_T("\tCommunicationThread\tsend error %d\n"), WSAGetLastError());
			closesocket(socket);
			return -1;
		}
		_tprintf(_T("\tCommunicationThread(%d)\tsend %d bytes\n"), dwTid, bytesSent);
	}

	HeapFree(GetProcessHeap(), 0, szRequest);
	closesocket(socket);

	return 0;
}

int main(void)
{
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	printf("bind\n");
	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	printf("start listen......\n");
	while (1)
	{
		printf("ready to accept\n");
		ClientSocket = accept(ListenSocket, NULL, NULL);

		printf("accept a connection\n");
		CreateThread(NULL, 0, CommunicationThread, (LPVOID)ClientSocket, 0, NULL);
	}

	WSACleanup();
	return 0;
}