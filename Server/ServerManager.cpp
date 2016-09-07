#include "ServerManager.h"
#include "EventHandler.h"
#include "Event.h"
#include <exception>
#include "Client.h"


//
// <Method>
//    Constructor
// <Summary>
//    Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
//
ServerManager::ServerManager(unique_ptr<ServerSocket>& serPtr) : pool(thread::hardware_concurrency() * 2), mIsRunning(true), mServerSocket(move(serPtr)){}

//Destructor.
ServerManager::~ServerManager() {
}


//
// <Method>
//    listen
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::listen() {
  //Begin the server
  if (mServerSocket->begin()) {
    mAcceptEvent = WSACreateEvent();

    if (mAcceptEvent != WSA_INVALID_EVENT) {

      if (WSAEventSelect(mServerSocket->getSocketFileDescriptor(), mAcceptEvent, FD_ACCEPT) != SOCKET_ERROR) {
        mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        pool.EnqueueTask([=] { getConnections(); });
        pool.EnqueueTask([=] {  });
        for (size_t i = 2; i < (thread::hardware_concurrency() * 2) - 2; i++) {
          pool.EnqueueTask([=] { workerThread(); });
        }
      }
    }
  }
}


void ServerManager::sendTask() {
  while (mIsRunning) {
    unique_lock<mutex> drainLock(mSendMutex);
    auto item = mSendQueue->pop();
    //Send the response to the server.
    if ((item.type != Type::INVALID)) {

      //Convert Packet down to byte array. 
      // There's no need to send the entire
      size_t packetSize = item.size + sizeof(Type) + sizeof(item.size);
      byte* data = new byte[packetSize];

      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, &item, packetSize);

      try {
        //mSocket->send(data, packetSize);
      }
      catch (exception& e) {
        mIsRunning.store(false);
        cout << "Error when attempting to send on socket. " << e.what() << endl;
      }

      delete[] data;
    }
  }
}

//
// <Method>
//    acceptConnection
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::acceptConnection(SOCKET& socket) {
  //Associate the socket with IOCP
  //Create a new ClientContext for this newly accepted client
  shared_ptr<Client> client = make_shared<Client>(socket);
  HANDLE hTemp = CreateIoCompletionPort((HANDLE)socket, mCompletionPort, (ULONG_PTR) client.get(), 0);

  if (hTemp) {
    unique_lock<mutex> lk(mClientQueue);
    mSockets.insert(std::pair<PULONG_PTR, shared_ptr<Client>>((PULONG_PTR)client.get(), client));
    lk.unlock();
    client->BeginRead();
  }
}

//
// <Method>
//    acceptConnection
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::removeConnection(const shared_ptr<Client>& client) {

  PULONG_PTR key = reinterpret_cast<PULONG_PTR>(client.get());

  if (key && mSockets.find(key) != mSockets.end()) {
    mSockets.erase(key);
  }
}

//
// <Method>
//    getConnections
// <Summary>
//   Listen to the Server Socket, accepting any 
// @return Pointer to a Socket.
//
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

//
// <Method>
//    getConnections
// <Summary>
//   Listen to the Server Socket, accepting any 
// @return Pointer to a Socket.
//
void ServerManager::workerThread() {
  PULONG_PTR ptrClient = NULL;
  OVERLAPPED *ptrOverlapped = NULL;
  DWORD bytesTransferred = 0;
  BOOL Status = 0;

  while (mIsRunning.load()) {
    Status = GetQueuedCompletionStatus(mCompletionPort, &bytesTransferred,
      (PULONG_PTR)&ptrClient, &ptrOverlapped, INFINITE);

    if (ptrClient) {
      auto client = mSockets.at(ptrClient);
      if ((!Status) || (Status && (bytesTransferred == 0))) {
        removeConnection(client);
        continue;
      }
    } else {
      cout << "Error" << endl;
      break;
    } 

  }

}
