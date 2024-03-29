#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "Qotd.h"

#pragma comment (lib, "ws2_32.lib")


int main()
{
	CQotd quotes("Resources//wisdom.txt");

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

	//Wait for a connnection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Couldn't establish connection! Quitting" << std::endl;
		return 0;
	}

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connected on

	//ZeroMemory is for windows
	//memset would be used here to make it portable
	ZeroMemory(host, NI_MAXHOST);	//same as memset(host, 0, NI_MAXHOST); (is used for linux or mac)
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	//Close listening socket
	closesocket(listening);

	//While loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		//Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error recieveing client data! Quitting." << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		//Echo response to console
		std::cout << "CLIENT> " << std::string(buf, 0, bytesReceived) << std::endl;

		//Get quote for client
		std::string quote = quotes.GetRandomQuote();

		//Echo message back to client
		send(clientSocket, quote.c_str(), quote.size() + 1, 0);
	}

	//Close the client socket
	closesocket(clientSocket);

	//Cleanup winsock
	WSACleanup();

	system("pause");

	return 0;
}