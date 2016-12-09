#include "AudioClientHandlerFactory.h"


// Constructor. Take a packet queue to push to.
AudioClientHandlerFactory::AudioClientHandlerFactory(shared_ptr<ConcurrentQueue<ClientPacket>> packetQueue) : mPacketQueue(packetQueue) {}

// Add to the client.
void AudioClientHandlerFactory::addHandlerToClient(weak_ptr<Client> client) {
	shared_ptr<Settings> settingsPtr = make_shared<Settings>();
	vector<shared_ptr<InputHandler>> handlers;
	shared_ptr<SettingsHandler> settingsHandle = make_shared<SettingsHandler>(settingsPtr);
	shared_ptr<AudioServerHandler> audioHandler = make_shared<AudioServerHandler>(client, mPacketQueue);
	handlers.push_back(audioHandler);
	handlers.push_back(settingsHandle);

	if (auto clientPtr = client.lock()) {
		clientPtr->SetReceivedHandler([=](Buffer& buffer, size_t size) {
			try {
				Packet packet;
				packet.Deserialize(buffer.getBuffer(), size);
				for (auto handler : handlers) {
					if (handler->listensFor(packet.type)) {
						handler->handlePacket(packet);
					}
				}
			}
			catch (exception& e) {
				cout << "Error Converting Buffer to Packet: " << e.what() << endl;
			}
		});
	}
}