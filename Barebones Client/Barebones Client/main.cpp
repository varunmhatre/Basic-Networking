#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	std::string ipAddress = "127.0.0.1";	//Server IP Address
	int port = 54000;						//Listening port number on server

	//Initialize winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsOK = WSAStartup(ver, &data);

	if (wsOK != 0)
	{
		std::cerr << "Can't start winsock! Err #" << wsOK << std::endl;
		return 0;
	}

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	//Fill in hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	
	//Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server! Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}
	
	//Do-while loop to send and receive data
	char buf[4096];
	std::string userInput;
	
	do
	{
		//Prompt user for text
		std::cout << "> ";
		std::getline(std::cin, userInput);

		if (userInput.size() > 0) // Make sure user has typed in something
		{
			//Send text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				//Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					//Echo response to console
					std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}

	} while (userInput.size() > 0);

	//Close down
	closesocket(sock);
	WSACleanup();

	return 0;
}