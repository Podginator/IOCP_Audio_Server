#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "InputHandler.h"
#include "ClientManagerFactory.h"
#include <unordered_map>
#include "ThreadPool.h"
#include "Client.h"
#include "IClientHandlerFactory.h"
#include <exception>
#include <fstream>
#include "Client.h"
#include <map>

// The Manager class to manage the Listening Server and Connection
class ServerManager {

public:

  // Constructor 
  //  serPtr : The Server socket we wish to listen on
  ServerManager(unique_ptr<ServerSocket>& serPtr);

  //Destructor.
  ~ServerManager();

  // Listen to the server. 
  // Accept connections.ClientManagerFactory
  void listen();

  // Set the Client Factory.
  void setClientFactory(shared_ptr<IClientHandlerFactory> factory);

  // Get the Queue used in the send 
  shared_ptr<ConcurrentQueue<ClientPacket>> getSendQueue();

  bool isClientConnected(shared_ptr<Client> client);

private:

  //The Server Socket we're listening on.
  unique_ptr<ServerSocket> mServerSocket;

  // Accept the Connection.
  void acceptConnection(SOCKET& socket);

  // Remove the connection
  void removeConnection(const shared_ptr<Client>& client);

  // Get any incoming connections. Create a client from it. 
  void getConnections();

  // Works through IOCP Connections
  void workerThread();

  bool isPtrToClientValid(const PULONG_PTR& ptr);

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

  // The factory to add handlers to the Client.
  shared_ptr<IClientHandlerFactory> mClientFactory;

  // A Send Queue. We drain this queue and send back to the C++.
  shared_ptr<ConcurrentQueue<ClientPacket>> mSendQueue;

  // The Send Queue mutex
  mutex mSendMutex;

  // Condition Variable to ensure thread synchronicity between Send/Recv; 
  condition_variable mSendCondition;

  // The Client Queue Mutex.
  mutex mClientQueue;

  //We are running.
  atomic_bool mIsRunning = false;
};