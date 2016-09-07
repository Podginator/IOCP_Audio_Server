#pragma once

#include "stdadfx.h"
#include "Socket.h"
#include <atomic>

/**
* The Server Instance that Listens to
* Incoming Connections.
*/
class ServerSocket {
private:

  //The port to connect on.
  int mPort;
  //The address we wish to connect to
  string mAddress;
  //The Socket File Description
  SOCKET mSocketFileDesc;

  // <Method>
  //    setupServerAddressStruct
  // <Summary>
  //    Sets up pthe Sockaddr_in 
  // @param ref The Socket_addr_in to modify.
  //
  void setupServerHints(addrinfo& ref);

public:

  // <Method>
  //    Constructor
  // <Summary>
  //    Constructs the Server Objects
  // @param hostName the host name to connect to (Ie: IP-Port)
  // @param port the port to listen to
  //
  ServerSocket(const string& hostName, int port);

  // Destructor:
  //  Calls WSACleanup(). 
  //  Is called on Exception from Constructor
  //  And 
  ~ServerSocket();

  //Copy constructor.
  // Ensure that we cannot copy this class elsewhere, as it is 
  // a noncopyable resource (A socket.)
  ServerSocket(const ServerSocket& other) = delete;

  //Move Constructor 
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  ServerSocket(ServerSocket&& other) = default;

  //Move Operator.
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  ServerSocket& operator=(ServerSocket&&) & = default;

  // <Method>
  //  accept
  // <Summary>
  //   Accept a connection that is attempting to connection
  //   Return a unique pointer to the Socket that is created.
  // @return Pointer to a Socket.
  //
  unique_ptr<Socket> acceptSocket();

  //
  // <Method>
  //   Begin Listening
  // <Summary>
  //   Starts listening on the Socket.
  //
  bool begin();

  // Return the Socket File Descriptor
  SOCKET getSocketFileDescriptor(void);

  // 
  // <Method>
  //  Close
  // <Summary> 
  //  Close the socket.
  //
  void close();
};