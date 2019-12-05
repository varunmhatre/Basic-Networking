#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")


int main()
{

	//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		std::cerr << "Can't initialize winsock! Quitting" << std::endl;
		return 0;
	}

	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return 0;
	}

	//Bind an ip address and a port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell winsock socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				//Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add new connection to the set
				FD_SET(client, &master);

				//Welcome message to newly connected client
				std::string welcomeMsg = "Welcome to the chat server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size(), 0);

				for (u_int j = 0; j < master.fd_count; j++)
				{
					SOCKET outSock = master.fd_array[j];
					if (outSock != listening && outSock != client)
					{
						std::ostringstream ss;

						ss << "SOCKET #" << client << " has joined the chat.\r\n";

						std::string notify = ss.str();
						send(outSock, notify.c_str(), notify.size() + 1, 0);
					}
				}
			}
			else
			{
				//Accept a new message
				char buf[4096];
				ZeroMemory(buf, 4096);

				//Receive message
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived <= 0)
				{

					for (u_int j = 0; j < master.fd_count; j++)
					{
						SOCKET outSock = master.fd_array[j];
						if (outSock != listening && outSock != sock)
						{
							std::ostringstream ss;

							ss << "SOCKET #" << sock << " has left the chat.\r\n";

							std::string notify = ss.str();
							send(outSock, notify.c_str(), notify.size() + 1, 0);
						}
					}
					//Drop Client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					//Send message to other CLIENTS and not to SERVER (listening)
					for (u_int j = 0; j < master.fd_count; j++)
					{
						SOCKET outSock = master.fd_array[j];
						if (outSock != listening && outSock != sock)
						{
							std::ostringstream ss;

							ss << "SOCKET #" << sock << ": " << buf << "\r";

							std::string msg = ss.str();
							send(outSock, msg.c_str(), msg.size() + 1, 0);
						}
					}
				}
			}
		}
	}

	//Cleanup winsock
	WSACleanup();

	system("pause");

	return 0;
}