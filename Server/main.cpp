#include <iostream>
#include "stdadfx.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "ServerManager.h"
#include "AudioServerHandler.h"
#include "AudioClientManagerFactory.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>

int main(int, char**)
{
  cout << "Server Started \n";
  unique_ptr<ServerSocket> servSocket = make_unique<ServerSocket>("localhost", 29054);
  try {
    ServerManager manager(servSocket);
    AudioClientManagerFactory fact;
    fact.createClientManager<ServerManager>(manager);
    manager.listen();

    while (1) {}
  }
  catch (exception& e) {
    cout << "Cannot start server manager: " << e.what() << endl;
  }
}
