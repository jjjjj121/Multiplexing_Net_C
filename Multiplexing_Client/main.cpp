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



#pragma region 접속 시 맵,Player 그리기
	char Buffer2[1024] = { 0, };
	int TotalRecvLength2 = 0;

	do
	{
		int RecvLength = recv(ServerSocket, &Buffer2[TotalRecvLength2], sizeof(Buffer2) - TotalRecvLength2, 0);

		if (RecvLength == 0)
		{
			//연결 종료
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

	//디스크립터
	fd_set Original;
	fd_set CopyReads;
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	//초기화
	FD_ZERO(&Original);
	//원하는 소켓으로 셋팅
	FD_SET(ServerSocket, &Original);


	while (bRunning)
	{
		char Buffer[1024] = { 0, };
		int TotalRecvLength = 0;

		//키보드 인풋이 있을 때만 로직 실행
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
		//맨앞 매개변수는 WIndow는 0넣어도됨 ->필요한 소켓 갯수?
		int fd_num = select(0, &CopyReads, 0, 0, &Timeout);


		if (fd_num == 0)
		{
			//another process
			continue;
		}

		//소켓 에러 시 종료
		if (fd_num == SOCKET_ERROR)
		{
			bRunning = false;
			break;
		}

		//이벤트 발생했는 지?
		for (size_t i = 0; i < Original.fd_count; ++i)
		{
			//등록한 소켓 리스트 중에 이벤트가 발생 했음.
			//등록한 소켓이 서버이므로 서버에서 정보를 보냈을 때 이벤트가 발생할 것
			if (FD_ISSET(Original.fd_array[i], &CopyReads))
			{

				//정보가 들어왔으므로 recv
				do
				{
					int RecvLength = recv(ServerSocket, &Buffer[TotalRecvLength], sizeof(Buffer) - TotalRecvLength, 0);

					if (RecvLength == 0)
					{
						//연결 종료
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

				//recv 받은 정보 변환해서 저장
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


		//std::cout << "X 좌표 - " << X << "  Y 좌표 -" << Y << std::endl;



	}
	closesocket(ServerSocket);

	WSACleanup();
	return 0;

}
