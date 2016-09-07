#include "ServerManager.h"
#include "EventHandler.h"
#include "Event.h"
#include <exception>
//
// <Method>
//    Constructor
// <Summary>
//    Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
//
ServerManager::ServerManager(unique_ptr<ServerSocket>& serPtr) : pool(thread::hardware_concurrency() * 2) {
  mServerSocket = move(serPtr);

}

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
    unique_lock<mutex> drainLock(mMutex);
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
void ServerManager::acceptConnection(unique_ptr<Socket> socket) {
  //Associate the socket with IOCP
  HANDLE hTemp = CreateIoCompletionPort((HANDLE)socket->getFileDescriptor(), mCompletionPort, (DWORD)socket.get(), 0);

  if (hTemp) {
    socket->beginReceive();
    mSockets.push_back(move(socket));
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

  while (true) {
    if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &mAcceptEvent, FALSE, 100, FALSE))
    {
      WSAEnumNetworkEvents(serverSocketDesc, mAcceptEvent, &events);
      if ((events.lNetworkEvents & FD_ACCEPT) && (0 == events.iErrorCode[FD_ACCEPT_BIT]))
      {
        unique_ptr<Socket> socket = mServerSocket->acceptSocket();
        if (socket != nullptr) {
          acceptConnection(move(socket));
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
  void *lpContext = NULL;
  OVERLAPPED       *pOverlapped = NULL;
  DWORD            dwBytesTransfered = 0;
  int nBytesRecv = 0;
  int nBytesSent = 0;
  DWORD             dwBytes = 0, dwFlags = 0;

  //Worker thread will be around to process requests,
  //until a Shutdown event is not Signaled.
  while (true)
  {
    BOOL bReturn = GetQueuedCompletionStatus(
      mCompletionPort,
      &dwBytesTransfered,
      (PULONG_PTR)&lpContext,
      &pOverlapped,
      INFINITE);

    if (NULL == lpContext)
    {
      //We are shutting down
      break;
    }
  }

}
