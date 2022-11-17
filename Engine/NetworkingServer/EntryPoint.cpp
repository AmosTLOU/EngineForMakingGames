#include "cMyNetworkServer.h"

int main(int argc, char* argv[])
{
    Networking::MyNetworkServer::StartServer();
    Networking::MyNetworkServer::EndServer();
}
