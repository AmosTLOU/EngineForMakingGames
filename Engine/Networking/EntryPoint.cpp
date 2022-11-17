#include <thread>
#include "cMyNetwork.h"

int main(int argc, char* argv[])
{
    Networking::MyNetwork::StartServer();
    Networking::MyNetwork::EndServer();

    /*Networking::MyNetwork::StartClient();
    while (1)
    {
        Networking::MyNetwork::RunClient("h");
        Sleep(1000);
    }    
    Networking::MyNetwork::EndClient();*/
}