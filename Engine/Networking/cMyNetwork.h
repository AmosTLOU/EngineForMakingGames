#include <vector>
#include <string>
#include <thread>
#include <signal.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define IP "192.168.0.240"
#define PORT "3490" // 使用者將連線的 port
#define BACKLOG 10 // 在佇列中可以有多少個連線在等待
#define MAXDATASIZE 100 // 我們一次可以收到的最大位原組數（number of bytes）


namespace Networking
{
	class MyNetwork
	{

	public:
		static void StartServer();
		static int EndServer();

		static int StartClient();
		static int Sync(std::string i_sendMsg);
		static int EndClient();		

		static int RunServer();
		static int RunClient(std::string i_sendMsg);

	protected:	
	private:
		void sigchld_handler(int s);		
		static void* get_in_addr(struct sockaddr* sa);
		
		
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
