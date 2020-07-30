// ProxyChain.cpp : Defines the entry point for the application.
//

#include "ProxyChain.h"
#include "ProxyChainHelper.h"

int main(int argc, char ** argv)
{
	NETWORK_STARTUP();

	std::cout << "Hello CMake." << std::endl;
	OnSocks5();

	NETWORK_CLEANUP();

	return 0;
}
