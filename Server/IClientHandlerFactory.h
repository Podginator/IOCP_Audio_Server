#pragma once

#include "Client.h"
#include "stdadfx.h"

class IClientHandlerFactory {
public:
  virtual void addHandlerToClient(weak_ptr<Client> client) = 0;
};
