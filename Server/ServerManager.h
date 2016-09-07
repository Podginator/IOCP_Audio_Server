#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include "ClientManagerFactory.h"
#include "ThreadPool.h"
#include <set> 

// The Manager class to manage the Listening Server and Connection
class ServerManager {

public:

  // Constructor 
  //  serPtr : The Server socket we wish to listen on
  //  factory : The factory to create a Client Manager on.
  ServerManager(unique_ptr<ServerSocket>& serPtr);

  //Destructor.
  ~ServerManager();

  // Listen to the server. 
  // Accept connections.ClientManagerFactory
  void listen();

  void addListener(shared_ptr<InputHandler> handler) {
    mHandlers.push_back(handler);
  }

private:

  //The Server Socket we're listening on.
  unique_ptr<ServerSocket> mServerSocket;

  // Accept the Connection.
  void acceptConnection(unique_ptr<Socket> socket);

  // Get any incoming connections. Create a client from it. 
  void getConnections();

  // Works through IOCP Connections
  void workerThread();

  // Send Task.
  //  Send the Packets in the queue.
  void sendTask();

  ThreadPool pool;

  HANDLE mCompletionPort;

  HANDLE mAcceptEvent;

  vector<unique_ptr<Socket>> mSockets;

  vector<shared_ptr<InputHandler>> mHandlers;
  
  // A Send Queue. We drain this queue and send back to the C++.
  shared_ptr<ConcurrentQueue<Packet>> mSendQueue;

  // The mutex
  mutex mMutex;

  //We are running.
  atomic_bool mIsRunning = false;


};