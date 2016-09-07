#pragma once

#include "ClientManagerFactory.h"

// An Implementation of a Socket Manager Factory. This creates a Client Manager that 
// Responds to incoming Audio Calls. We pass the Drainable Queue to the Audio Server Handler. 
class AudioClientManagerFactory {
public:

  // Create the Client Manager. Pass in the correct Socket.  
  template<class Listenable>
  void createClientManager(Listenable& listener) {
    //Create an audio handler. 
    shared_ptr<InputHandler> audioHandler = make_shared<AudioServerHandler>();

    //add the listeners.
    listener.addListener(audioHandler);
  }
};
