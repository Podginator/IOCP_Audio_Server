#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include "ClientManagerFactory.h"
#include <unordered_map>
#include "ThreadPool.h"
#include "Client.h"
#include <map>

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
  void acceptConnection(SOCKET& socket);

  void removeConnection(const shared_ptr<Client>& client);

  // Get any incoming connections. Create a client from it. 
  void getConnections();

  // Works through IOCP Connections
  void workerThread();

  // Send Task.
  //  Send the Packets in the queue.
  void sendTask();

  // Thread Pool to pool threads for Connection. 
  ThreadPool pool;

  // The Completion Port
  HANDLE mCompletionPort;

  // The Acceptance Event
  HANDLE mAcceptEvent;

  // A vector of connected Clients.
  unordered_map<PULONG_PTR, shared_ptr<Client>> mSockets;

  // The Input Handlers
  vector<shared_ptr<InputHandler>> mHandlers;
  
  // A Send Queue. We drain this queue and send back to the C++.
  shared_ptr<ConcurrentQueue<Packet>> mSendQueue;

  // The Send Queue mutex
  mutex mSendMutex;

  // The Client Queue Mutex.
  mutex mClientQueue;

  //We are running.
  atomic_bool mIsRunning = false;
};