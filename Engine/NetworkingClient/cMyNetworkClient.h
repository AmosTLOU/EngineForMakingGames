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
	class MyNetworkClient
	{

	public:
		static int StartClient();
		static int Sync(std::string i_sendMsg);
		static int EndClient();		

	protected:	
	private:
		//void sigchld_handler(int s);		
		static void* get_in_addr(struct sockaddr* sa);
		static int RunClient(std::string i_sendMsg);
		
		
	public:
		static std::string RecvMsg;
		
	protected:
	private:
		static int sockfd_client;
		static struct addrinfo* servinfo_client;
		static char s_client[INET6_ADDRSTRLEN];
		static std::vector<std::thread*> vtThread;		
	};

}
