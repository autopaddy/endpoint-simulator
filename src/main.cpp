#include <iostream>

#include "mDNSService.h"
#include "mDNSAnnouncer.h"

int main()
{
    std::cout << "Hello - Starting the application :)\n";

    // Create a mock chromecast mDNS-SD service
    MDNSService service("Fake-Chromecast-00001", 1, 8009, "_googlecast._tcp");
    service.addTXTRecord("id", "00000000000000000000000000000000");   // unique-id for the chromecast without hyphens
    service.addTXTRecord("cd", "00000000000000000000000000000000");   // not sure what this is??
    service.addTXTRecord("md", "Chromecast");                         // model
    service.addTXTRecord("rm", "");                                   // not sure what this is??
    service.addTXTRecord("ve", "05");                                 // version
    service.addTXTRecord("ic", "/setup/icon.png");                    // path to icon associated with the device
    service.addTXTRecord("fn", "Fake Chromecast");                    // friendly name associated with the chromecast
    service.addTXTRecord("ca", "4101");                               // certificate authority.  This gets used later when a secure connection is established with the chreomcast
    service.addTXTRecord("st", "0");                                  // not sure what this is??
    service.addTXTRecord("bs", "000000000000");                       // not sure what this is??
    service.addTXTRecord("nf", "1");                                  // not sure what this is??
    service.addTXTRecord("rs", "");

    std::cout << "Hellooooo,\n";
    std::cout << "Hellooooo,\n";
    std::cout << "Hellooooo,\n";
    std::cout << "Hellooooo,\n";

    MDNSAnnouncer announcer;
    announcer.start();
    announcer.registerService(service);

    // Simple hold to prevent
    std::cin.get();
}
