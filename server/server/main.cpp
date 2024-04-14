#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>

#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_LISTEN_NUM 5
#define RECV_BUF_SIZE 8192
#define LISTEN_PORT 2010

int wmain(int argc, WCHAR* argv[])
{
	SOCKET fd = 0;
	SOCKET fd1 = 0;
	struct sockaddr_in sai;
	int socklen = sizeof(sai);
	char recvbuf[RECV_BUF_SIZE];
	int retlen;
	int port = 2024;

	if (argc > 1)
	{
		port = _wtoi(argv[1]);
	}

	WSADATA wsad;

	if (WSAStartup(MAKEWORD(2, 2), &wsad) == 0)
	{
		memset((void*)&sai, 0, sizeof(sai));

		sai.sin_family = AF_INET;
		sai.sin_port = htons(port);
		sai.sin_addr.s_addr = htonl(INADDR_ANY);

		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd != INVALID_SOCKET)
		{
			if (bind(fd, (const struct sockaddr*)&sai, sizeof(sai)) == 0)
			{
				if (listen(fd, MAX_LISTEN_NUM) == 0)
				{
					while (1)
					{
						fd1 = accept(fd, (struct sockaddr*)&sai, &socklen);
						if (fd1 != INVALID_SOCKET)
						{
							printf("accept ok!\r\n");
							Sleep(5000);

							int t = 0;
							for (int i = 0; i < 10; i++)
							{
								char temp[256];
								sprintf(temp, "%d, 1234567890", t++);
								retlen = send(fd1, temp, strlen(temp) + 1, 0);
								printf("send %d\r\n", retlen);
							}
							Sleep(10000);
							while (1)
							{
								char temp[256];
								sprintf(temp, "%d, 1234567890", t++);
								retlen = send(fd1, temp, strlen(temp) + 1, 0);
								printf("send %d\r\n", retlen);
							}

							printf("Press a key to quit socket\r\n");

							if (getchar())
							{
							}

							closesocket(fd1);
						}
					}
				}
			}

			closesocket(fd);
		}

		WSACleanup();
	}

	return 0;
}
