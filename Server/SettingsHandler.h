#pragma once
#include "Settings.h"
#include "InputHandler.h"


using namespace std;

/**
* The Settings Handler
*/
class SettingsHandler : public InputHandler {
public:

  //Constructor
  // Shared Pointer to the settings.
  SettingsHandler(shared_ptr<Settings> settings);

  //Handle the Packet we have sent 
  //  sentMessage: The Packet we wish to process.  
  void handlePacket(const Packet& packet);
private:

  // The Client we wish to send to.
  shared_ptr<Settings> mSettings;
};