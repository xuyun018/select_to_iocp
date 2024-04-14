// client.cpp : Defines the entry point for the console application.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_LISTEN_NUM 5
#define SEND_BUF_SIZE 8192
#define LISTEN_PORT 2024

DWORD WINAPI client_proc(LPVOID param)
{
	SOCKET fd = (SOCKET)param;
	fd_set readfds;
	fd_set writefds;
	fd_set* preadfds;
	fd_set* pwritefds;
	char buffer[8192];
	struct timeval tv;
	int r;
	int connected = 0;

	while (1)
	{
		if (connected)
		{
			FD_ZERO(&readfds);
			FD_SET(fd, &readfds);

			preadfds = &readfds;
			pwritefds = NULL;
		}
		else
		{
			FD_ZERO(&writefds);
			FD_SET(fd, &writefds);

			preadfds = NULL;
			pwritefds = &writefds;
		}

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		// IOCP 等待, 有数据返回, 取出数据, WSARecv/recv 32 投递(多)

		r = select(fd + 1, preadfds, pwritefds, NULL, &tv);
		switch (r)
		{
		case -1:
			break;
		case 0:
			break;
		default:
			if (preadfds && FD_ISSET(fd, preadfds))
			{
				while (1)
				{
					r = recv(fd, buffer, sizeof(buffer), 0);
					if (r > 0)
					{
						printf("receive %d\r\n", r);
					}
					else if (r == 0)
					{
						break;
					}
					else
					{
						int errorcode = WSAGetLastError();
						if (errorcode == WSAEWOULDBLOCK || errorcode == WSAEINTR)
						{
							errorcode = 0;
						}
					}
				}
			}
			else if (pwritefds && FD_ISSET(fd, pwritefds))
			{
				int optval = 0;
				int optlen;

				optlen = sizeof(int);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) == 0)
				{
					//
				}
				else
				{
					optval = 1;
				}

				if (optval == 0)
				{
					connected = 1;
				}
			}
			break;
		}
	}

	return(0);
}

int wmain(int argc, WCHAR* argv[])
{
	SOCKET fd = 0;
	char cp[256];
	int i;
	int j;
	struct sockaddr_in ser_addr;
	int port = 2024;

	if (argc > 1)
	{
		i = 0;
		j = 0;
		while (argv[1][i] != '\0' && i + 1 < sizeof(cp))
		{
			cp[i] = argv[1][i];
			if (cp[i] == ':')
			{
				j = i + 1;
			}
			i++;
		}
		cp[i] = '\0';

		//strcpy(cp, "192.168.236.154");

		printf("%s\r\n", cp);

		if (j)
		{
			cp[j - 1] = '\0';
			port = atoi(cp + j);
		}

		//i = 0;
		//while (string[i] != '\0' && i + 1 < sizeof(cp))
		//{
		//	cp[i] = string[i];
		//	i++;
		//}
		//cp[i] = '\0';

		// no release

		WSADATA wsad;

		if (WSAStartup(MAKEWORD(2, 2), &wsad) == 0)
		{
			memset(&ser_addr, 0, sizeof(ser_addr));
			ser_addr.sin_family = AF_INET;
			ser_addr.sin_addr.S_un.S_addr = inet_addr(cp);
			ser_addr.sin_port = htons(port);
			fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd != INVALID_SOCKET)
			{
				u_long argp = 1;
				ioctlsocket(fd, FIONBIO, (u_long*)&argp);

				// 
				if (connect(fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)) == 0 || WSAGetLastError() == WSAEWOULDBLOCK)
				{
					printf("connected\r\n");

					if (0)
					{
						char recvbuf[8192];
						int retlen;

						while (1)
						{
							retlen = recv(fd, recvbuf, sizeof(recvbuf), 0);
							if (retlen > 0)
							{
								printf("receive %d\r\n", retlen);
							}
							else if (retlen == 0)
							{
								printf("receive error\r\n");
								break;
							}
							else
							{
								int errorcode = WSAGetLastError();
								if (errorcode == WSAEWOULDBLOCK || errorcode == WSAEINTR)
								{
									;
								}
								else
								{
									break;
								}
							}
						}
					}
					else
					{
						HANDLE hthread;
						hthread = CreateThread(NULL, 0, client_proc, (LPVOID)fd, 0, NULL);
						if (hthread)
						{
							CloseHandle(hthread);
						}
					}
				}
				else
				{
					printf("connect WSAGetLastError %d\r\n", WSAGetLastError());
				}

				printf("press enter key\r\n");

				if (getchar())
				{
				}

				closesocket(fd);
			}

			WSACleanup();
		}
	}

	printf("exit\r\n");

	return 0;
}

