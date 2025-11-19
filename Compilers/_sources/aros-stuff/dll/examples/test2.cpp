#include "dll.h"
#include <iostream>
#include "server.h"

Server *server;

int main()
{
	void *hinst;
	Server *(*CreateServer)();


	hinst = dllLoadLibrary("server.dll", 0);

	CreateServer = (Server * (*)()) dllGetProcAddress(hinst, "CreateServer");

	server = CreateServer();

	std::cout << "2 + 3 = " << server->Test(2, 3) << std::endl;

	delete server;

	dllFreeLibrary(hinst);
}
