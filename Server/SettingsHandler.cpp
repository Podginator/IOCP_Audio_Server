#include "SettingsHandler.h"

SettingsHandler::SettingsHandler(shared_ptr<Settings> sharedSetting) : 
  InputHandler(Type::SETTINGS) {
  mSettings = sharedSetting;
};


// Reassign the Settings Packet to the one sent via the packet.
void SettingsHandler::handlePacket(const Packet& packet) {
  // Get The Settings.
  Settings settings;
  size_t settingSize = sizeof(Settings);

  //Ensure that the packets contents are the size of a song (as a preliminary check)
  if (packet.size == sizeof(Settings)) {
    //Copy the memory of the song to a song object. 
    memcpy(&settings, packet.packetData, packet.size);
    *mSettings = settings;
  }

}