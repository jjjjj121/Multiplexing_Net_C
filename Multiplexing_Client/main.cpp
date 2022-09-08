#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

void MyMemSet(char* p, int Value, size_t Size)
{
	for (size_t i = 0; i < Size; ++i)
	{
		p[i] = Value;
	}
}

void MyMemCopy(char* Destination, char* Source, size_t Size)
{
	for (size_t i = 0; i < Size; ++i)
	{
		Destination[i] = Source[i];
	}
}

char Map[10][10] = {
	{'0','0','0','0','0','0','0','0','0','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','0','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','0','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','0','0','0','0','0','0','0','0','0'},
};


int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerAddrIn;
	//memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	MyMemSet((char*)&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN));

	int X = 1;
	int Y = 1;
	bool bRunning = true;



#pragma region ���� �� ��,Player �׸���
	char Buffer2[1024] = { 0, };
	int TotalRecvLength2 = 0;

	do
	{
		int RecvLength = recv(ServerSocket, &Buffer2[TotalRecvLength2], sizeof(Buffer2) - TotalRecvLength2, 0);

		if (RecvLength == 0)
		{
			//���� ����
			std::cout << "Disconnected Client." << std::endl;
			bRunning = false;
			break;
		}
		else if (RecvLength < 0)
		{
			//Error
			std::cout << "Disconnected Client By Error : " << GetLastError() << std::endl;
			bRunning = false;
			break;
		}
		TotalRecvLength2 += RecvLength;
	} while (TotalRecvLength2 < 8);

	MyMemCopy((char*)&X, &Buffer2[0], 4);
	MyMemCopy((char*)&Y, &Buffer2[4], 4);

	//draw
	system("cls");
	for (int MapY = 0; MapY < 10; ++MapY)
	{
		for (int MapX = 0; MapX < 10; ++MapX)
		{
			if (Map[MapY][MapX] == '0')
			{
				std::cout << "#" << " ";
			}
			else if (MapX == X && MapY == Y)
			{
				std::cout << "P" << " ";
			}
			else
			{
				std::cout << " " << " ";
			}
		}

		std::cout << std::endl;
	}
#pragma endregion

	//��ũ����
	fd_set Original;
	fd_set CopyReads;
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	//�ʱ�ȭ
	FD_ZERO(&Original);
	//���ϴ� �������� ����
	FD_SET(ServerSocket, &Original);


	while (bRunning)
	{
		char Buffer[1024] = { 0, };
		int TotalRecvLength = 0;

		//Ű���� ��ǲ�� ���� ���� ���� ����
		if (_kbhit())
		{
			int KeyCode = _getch();

			switch (KeyCode)
			{
			case 'w':
			case 's':
			case 'a':
			case 'd':
			{
				char Code = (char)(KeyCode);


				int SentLength = send(ServerSocket, &Code, 1, 0);
			}
			break;
			case 'q':

				bRunning = false;
				break;

			}

		}



		CopyReads = Original;

		//polling
		//�Ǿ� �Ű������� WIndow�� 0�־�� ->�ʿ��� ���� ����?
		int fd_num = select(0, &CopyReads, 0, 0, &Timeout);


		if (fd_num == 0)
		{
			//another process
			continue;
		}

		//���� ���� �� ����
		if (fd_num == SOCKET_ERROR)
		{
			bRunning = false;
			break;
		}

		//�̺�Ʈ �߻��ߴ� ��?
		for (size_t i = 0; i < Original.fd_count; ++i)
		{
			//����� ���� ����Ʈ �߿� �̺�Ʈ�� �߻� ����.
			//����� ������ �����̹Ƿ� �������� ������ ������ �� �̺�Ʈ�� �߻��� ��
			if (FD_ISSET(Original.fd_array[i], &CopyReads))
			{

				//������ �������Ƿ� recv
				do
				{
					int RecvLength = recv(ServerSocket, &Buffer[TotalRecvLength], sizeof(Buffer) - TotalRecvLength, 0);

					if (RecvLength == 0)
					{
						//���� ����
						std::cout << "Disconnected Client." << std::endl;
						bRunning = false;
						break;
					}
					else if (RecvLength < 0)
					{
						//Error
						std::cout << "Disconnected Client By Error : " << GetLastError() << std::endl;
						bRunning = false;
						break;
					}
					TotalRecvLength += RecvLength;
				} while (TotalRecvLength < 8);

				//recv ���� ���� ��ȯ�ؼ� ����
				MyMemCopy((char*)&X, &Buffer[0], 4);
				MyMemCopy((char*)&Y, &Buffer[4], 4);

				//draw
				system("cls");
				for (int MapY = 0; MapY < 10; ++MapY)
				{
					for (int MapX = 0; MapX < 10; ++MapX)
					{
						if (Map[MapY][MapX] == '0')
						{
							std::cout << "#" << " ";
						}
						else if (MapX == X && MapY == Y)
						{
							std::cout << "P" << " ";
						}
						else
						{
							std::cout << " " << " ";
						}
					}

					std::cout << std::endl;
				}
			}

		}


		//std::cout << "X ��ǥ - " << X << "  Y ��ǥ -" << Y << std::endl;



	}
	closesocket(ServerSocket);

	WSACleanup();
	return 0;

}
