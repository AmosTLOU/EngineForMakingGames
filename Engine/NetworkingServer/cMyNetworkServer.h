#include <vector>
#include <string>
#include <thread>
#include <signal.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define IP "192.168.0.240"
#define PORT "3490" // the port clients will be connnecting to
#define BACKLOG 10 // how many connections are waiting
#define MAXDATASIZE 100 // the largest number of bytes we can get for one time


namespace Networking
{
	class MyNetworkServer
	{

	public:
		static void StartServer();
		static int EndServer();
	protected:	

	private:
		//void sigchld_handler(int s);		
		static void* get_in_addr(struct sockaddr* sa);
		static int RunServer();
		
		
	public:
		static std::string RecvMsg;
		
	protected:
	private:		
	};

}
