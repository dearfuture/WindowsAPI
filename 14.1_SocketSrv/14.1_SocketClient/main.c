#include<winsock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<tchar.h>

#pragma comment(lib, "ws2_32.lib")

#define RECV_BUFFER_SIZE 8192

int main(int argc, char* argv[])
{
	SOCKADDR_IN clientService;
	SOCKET ConnectSocket;
	WSADATA wsaData;
	LPVOID recvBuf;
	int bytesSent;
	int bytesRecv = 0;
	char sendBuf[32] = "get information";

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(10000);

	connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	bytesSent = send(ConnectSocket, sendBuf, strlen(sendBuf) + 1, 0);
	printf("Bytes send: %ld\n", bytesSent);
	
	recvBuf = HeapAlloc(GetProcessHeap(), 0, RECV_BUFFER_SIZE);
	while (bytesRecv != SOCKET_ERROR)
	{
		bytesRecv = recv(ConnectSocket, recvBuf, RECV_BUFFER_SIZE, 0);

		if (0 == bytesRecv)
		{
			printf("Connection closed.\n");
			break;
		}
		else
		{
			printf("Bytes Recv: %ld: %s\n", bytesRecv, recvBuf);
		}
		
	}

	HeapFree(GetProcessHeap(), 0, recvBuf);
	WSACleanup();

	return 0;
}