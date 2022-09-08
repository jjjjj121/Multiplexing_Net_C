#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")


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
	int X = 1;
	int Y = 1;
	bool bRunning = true;

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	//memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	memset((char*)&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = INADDR_ANY;


	//ctrl + RR 하면 선택한 키워드 싹다 한번에 바꾸기 가능!
	fd_set Original;
	fd_set CopyReads;
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	//초기화
	FD_ZERO(&Original);
	//원하는 소켓으로 셋팅
	FD_SET(ServerSocket, &Original);




	bind(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN));

	listen(ServerSocket, 0);

	while (bRunning)
	{

		CopyReads = Original;
		//std::cout << Timeout.tv_sec << Timeout.tv_usec << std::endl;

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


		for (size_t i = 0; i < Original.fd_count; ++i)
		{
			//등록한 소켓 리스트 중에 이벤트가 발생 했음.
			if (FD_ISSET(Original.fd_array[i], &CopyReads))
			{
				if (Original.fd_array[i] == ServerSocket)
				{
					//서버 소켓에 이벤트가 발생함, 연결 요청
					SOCKADDR_IN ClientSockAddrIn;
					memset((char*)&ClientSockAddrIn, 0, sizeof(SOCKADDR_IN));
					int ClientSockAddrInSize = sizeof(SOCKADDR_IN);
					SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddrIn, &ClientSockAddrInSize);

					FD_SET(ClientSocket, &Original);
					std::cout << "Connected Client : " << ClientSocket << std::endl;
				}
				else
				{

					SOCKET ClientSocket = Original.fd_array[i];
					//Client 소켓, Recv, Send
					char Buffer[1024] = { 0, };

					//blocking, non blocking
					//packet 설계, protocol 
					int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);

					if (RecvLength == 0)
					{
						//연결 종료
						std::cout << "Disconnected Client." << std::endl;
						FD_CLR(ClientSocket, &Original);
						closesocket(ClientSocket);
						continue;

					}
					else if (RecvLength < 0)
					{
						//Error
						std::cout << "Disconnected Client By Error : " << GetLastError() << std::endl;
						FD_CLR(ClientSocket, &Original);
						closesocket(ClientSocket);
						continue;

					}
					else
					{
						//자료가 있으면 처리
						//Packet Parse
						char KeyCode = Buffer[0];
						switch (KeyCode)
						{
						case 'w':
							if (Map[Y - 1][X] == '1')
							{
								Y--;
							}
							break;
						case 's':
							if (Map[Y + 1][X] == '1')
							{
								Y++;
							}
							break;
						case 'a':
							if (Map[Y][X - 1] == '1')
							{
								X--;
							}

							break;
						case 'd':
							if (Map[Y][X + 1] == '1')
							{
								X++;
							}
							break;

						}

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

						memcpy(&Buffer[0], (char*)&X, 4);
						memcpy(&Buffer[4], (char*)&Y, 4);


						if (Original.fd_array[i] != ServerSocket)
						{
							int SentLength = send(Original.fd_array[i], Buffer, 8, 0);
						}



					}

				}

			}

		}

	}

	//closesocket(ClientSocket);
	closesocket(ServerSocket);

	WSACleanup();
	return 0;
}
