#include <vector>
#include <string>
#include <thread>
#include <signal.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define IP "192.168.0.240"
// the port clients will be connnecting to
#define PORT "3490" 
// how many connections are waiting
#define BACKLOG 10 
// the largest number of bytes we can get for one time
#define MAXDATASIZE 100 


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
		static std::string RecvMsg[2];
		
	protected:
	private:		
	};

}
