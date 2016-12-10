#include "ServerManager.h"



// Constructor: 
//  Pass a server socket to listen on
//  Instantiate the threadpool to be 2 * the Hardware threads. 
//  Create a Queue to send from.
ServerManager::ServerManager(unique_ptr<ServerSocket>& serPtr)
  : pool(thread::hardware_concurrency() * 2), mIsRunning(true),
  mServerSocket(move(serPtr)), mSendQueue(make_shared <ConcurrentQueue<ClientPacket>>()) {}

//Destructor.
// Stop all the threads. 
// Wait for them to join.
ServerManager::~ServerManager() {}


// Listen: 
//  Begin Listening on the Server Socket
//  Create the accept event. 
//  Create an IO Completion Port to listen on the Server Socket. 
//  Create one thread to get connections, the other to send tasks, the rest to do IO Completion port 
void ServerManager::listen() {
  //Begin the server
  if (mServerSocket->begin()) {
    mAcceptEvent = WSACreateEvent();
    if (mAcceptEvent != WSA_INVALID_EVENT) {
      if (WSAEventSelect(mServerSocket->getSocketFileDescriptor(), mAcceptEvent, FD_ACCEPT) != SOCKET_ERROR) {
        mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        pool.EnqueueTask([=] { getConnections(); });
        pool.EnqueueTask([=] { sendTask(); });
        for (size_t i = 2; i < (thread::hardware_concurrency() * 2) - 2; i++) {
          pool.EnqueueTask([=] { workerThread(); });
        }
      }
    }
  }
}


// sendTask
//  While we are running get the latest ClientPacket (pair of client, Packet) in the send queue
//  Send the Packet to the client. 
void ServerManager::sendTask() {
  while (mIsRunning) {
    unique_lock<mutex> drainLock(mSendMutex);
    // Wait for notification that the i/o even has been correctly processed.
    auto clientPacket = mSendQueue->pop();
    auto client = clientPacket.first;
    auto item = clientPacket.second;
    //Send the response to the server.
    if ((item.type != Type::INVALID)) {

      //Convert Packet down to byte array. 
      // There's no need to send the entire packet. 
      size_t packetSize = item.size + sizeof(Type) + sizeof(item.id) + sizeof(item.size);
      byte* data = new byte[packetSize];

      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, &item, packetSize);
      try {
        if (auto clientPtr = client.lock()) {
          //send(clientPtr->getSocket(), reinterpret_cast<char*>(data), (int)packetSize, 0);
          clientPtr->BeginSend(data, packetSize);
          mSendCondition.wait(drainLock);
        }
      }
      catch (exception& e) {
        mIsRunning.store(false);
        cout << "Error when attempting to send on socket. " << e.what() << endl;
      }

      delete[] data;
    }
  }
}

// Accept the Connection.
//  Pass a socket, create a client from it. 
//  Create an IO Completion port from the socket
void ServerManager::acceptConnection(SOCKET& socket) {
  //Create a new ClientContext for this newly accepted client
  shared_ptr<Client> client = make_shared<Client>(socket, sizeof(Packet));
  //Associate with Completion Port. 
  HANDLE handle = CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), mCompletionPort, reinterpret_cast<ULONG_PTR>(client.get()), 0);
  // If the handle is not null  then we insert the pair to the map.
  if (handle) {
    // Insert to Sockets Map. 
    unique_lock<mutex> lk(mClientQueue);
    mSockets.insert(std::pair<PULONG_PTR, shared_ptr<Client>>((PULONG_PTR)client.get(), client));

    // If we have a client factory, we modify the client to add the specific handlers. 
    if (mClientFactory != nullptr) {
      mClientFactory->addHandlerToClient(client);
    }

    lk.unlock();
    // Then start receiving.
    client->BeginRead();
  }
}

// Remove Connection
//  Remove the connection from the Map.
void ServerManager::removeConnection(const shared_ptr<Client>& client) {
  PULONG_PTR key = reinterpret_cast<PULONG_PTR>(client.get());
  unique_lock<mutex> lk(mClientQueue);
  if (key && mSockets.find(key) != mSockets.end()) {
    mSockets.erase(key);
  }
  lk.unlock();
}

// Set the Client factory. 
//  When we create a client this factory creates the Send and Receive Handlers.
void ServerManager::setClientFactory(shared_ptr<IClientHandlerFactory> factory) {
  mClientFactory = factory;
}

// Get the Send Queue
shared_ptr<ConcurrentQueue<ClientPacket>> ServerManager::getSendQueue() {
  return mSendQueue;
}


// Get Connections
//  Listen For an Asynchronous Accept Call. When we have one, accept the connection. 
void ServerManager::getConnections() {
  SOCKET serverSocketDesc = mServerSocket->getSocketFileDescriptor();
  WSANETWORKEVENTS events;

  while (mIsRunning.load()) {
    if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &mAcceptEvent, FALSE, 100, FALSE))
    {
      WSAEnumNetworkEvents(serverSocketDesc, mAcceptEvent, &events);
      if ((events.lNetworkEvents & FD_ACCEPT) && (0 == events.iErrorCode[FD_ACCEPT_BIT])) {
        try {
          SOCKET socket = mServerSocket->acceptSocket();
          acceptConnection(socket);
        }
        catch (...) {
          cout << "err" << endl;
        }
      }
    }
  }
}

// Check a client is connected. 
bool ServerManager::isClientConnected(shared_ptr<Client> client) {
  PULONG_PTR key = reinterpret_cast<PULONG_PTR>(client.get());
  return isPtrToClientValid(key);
}

bool ServerManager::isPtrToClientValid(const PULONG_PTR& key) {
  bool connected = false; 
  unique_lock<mutex> lk(mClientQueue);
  if (key && mSockets.find(key) != mSockets.end()) {
    connected = true;
  }
  lk.unlock();

  return connected;
}

// Worker Thread: 
//  This is where the I/O Completion ports are done. 
//  We wait for a queued completion, then get the pointer of the client. 
//  Then we retrieve the client, and, if we have one perform the appropriate action.
void ServerManager::workerThread() {
  PULONG_PTR ptrClient = NULL;
  OVERLAPPED *ptrOverlapped = NULL;
  DWORD bytesTransferred = 0;
  BOOL Status = 0;

  while (mIsRunning.load()) {
    // We have either processed the previous result. 
    Status = GetQueuedCompletionStatus(mCompletionPort, &bytesTransferred,
      (PULONG_PTR)&ptrClient, &ptrOverlapped, INFINITE);

    if (isPtrToClientValid(ptrClient)) {
      auto client = mSockets.at(ptrClient);
      if ((!Status) || (Status && (bytesTransferred == 0))) {
        removeConnection(client);
        continue;
      } else {
        try {
          client->BeginIOEvent(bytesTransferred);
          client->BeginRead();
          mSendCondition.notify_all();
        } catch (runtime_error& err) {
          cout << err.what() << ": Removing Connection " << endl; 
          removeConnection(client);
        }
      }
    }
  }

}
