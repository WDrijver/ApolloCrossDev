// Minimal dll example

#include "dll.h"
#include <iostream>
#include "server.h"

class Client
{
public:
	Client()
	{
		std::cout << "Client constructor" << std::endl;
	};
	
	~Client()
	{
		std::cout << "Client destructor" << std::endl;
	};
};


Client testclient;

Server::Server()
{
	std::cout << "Server constructor" << std::endl;
}

Server::~Server()
{
	std::cout << "Server destructor" << std::endl;
}

int Server::Test(int a, int b)
{
	return a + b;
}


Server *CreateServer()
{
	return new Server;
};

dll_tExportSymbol DLL_ExportSymbols[] =
{
	{(void *)CreateServer, (char *)"CreateServer"},
	
	{NULL, NULL}
};

dll_tImportSymbol DLL_ImportSymbols[] =
{
	{NULL, NULL, NULL, NULL}
};

int DLL_Init(void)
{
	return 1;
}

void DLL_DeInit(void)
{
}
