#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "cMyNetworkClient.h"
#include <Engine/NetworkingServer/cMyNetworkServer.h>

using namespace std;

int Networking::MyNetworkClient::sockfd_client;
struct addrinfo* Networking::MyNetworkClient::servinfo_client;
char Networking::MyNetworkClient::s_client[INET6_ADDRSTRLEN];
std::string Networking::MyNetworkClient::RecvMsg = "";
std::vector<std::thread*> Networking::MyNetworkClient::vtThread;


//void Networking::MyNetwork::sigchld_handler(int s)
//{
//    while (waitpid(-1, NULL, WNOHANG) > 0);
//}


// Get IPv4 or IPv6 sockaddr
void* Networking::MyNetworkClient::get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int Networking::MyNetworkClient::StartClient()
{
    WSADATA wsaData; 
    // if the above doesn't work, try the following instead
    //WSAData wsaData;

    // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    struct addrinfo hints;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(IP, PORT, &hints, &servinfo_client)) != 0) 
    {
        const char* errorMsg = reinterpret_cast<char*>(gai_strerror(rv));
        fprintf(stderr, "getaddrinfo: %s\n", errorMsg);
        return 1;
    }

    return 0;
}


int Networking::MyNetworkClient::Sync(std::string i_sendMsg)
{
    //Networking::MyNetwork::RunClient(i_sendMsg);

    //vtThread.push_back(new std::thread(Networking::MyNetwork::RunClient, i_sendMsg));
    new std::thread(Networking::MyNetworkClient::RunClient, i_sendMsg);

    return 0;
}


int Networking::MyNetworkClient::RunClient(std::string i_sendMsg)
{
    struct addrinfo* p;
    int numbytes;
    char buf[MAXDATASIZE];

    // Iterate all the results and connect to the first one that is available
    for (p = servinfo_client; p != NULL; p = p->ai_next) {
        if ((sockfd_client = static_cast<int>(socket(p->ai_family, p->ai_socktype,
            p->ai_protocol))) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd_client, p->ai_addr, static_cast<int>(p->ai_addrlen)) == -1) {
            closesocket(sockfd_client);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s_client, sizeof s_client);
    printf("client: connecting to %s\n", s_client);

    //send
    const char* sendMsg = i_sendMsg.c_str();
    if (send(sockfd_client, sendMsg, static_cast<int>(strlen(sendMsg)), 0) == -1)
        perror("send");

    // receive
    if ((numbytes = recv(sockfd_client, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n", buf);

    return 0;
}


int Networking::MyNetworkClient::EndClient()
{
    freeaddrinfo(servinfo_client);
    closesocket(sockfd_client);
    return 0;
}
