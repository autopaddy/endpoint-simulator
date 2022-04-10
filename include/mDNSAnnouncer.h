#ifndef ES_MDNS_ANNOUNCER_INCLUDED
#define ES_MDNS_ANNOUNCER_INCLUDED

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <avahi-common/error.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>

#include "interface/Announcer.h"
#include "mDNSService.h"

/**
 * Interface for the possible annoucers
*/
class MDNSAnnouncer : public IAnnouncer
{
public:
    MDNSAnnouncer();
    ~MDNSAnnouncer();

    void registerService(const MDNSService& service);

    void start() final;
    void stop() final;
    void run() final;

private:

    bool m_running;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_thread;
    AvahiSimplePoll* m_poll;
    AvahiClient* m_client;
    int m_error;

    static int onPoll(struct pollfd* ufds, unsigned int nfds, int timeout, void* userdata);
    static void onClientStateChange(AvahiClient* client, AvahiClientState state, void* userdata);
    static void onGroupStateChange(AvahiEntryGroup* avahiGroup, AvahiEntryGroupState state, void* userdata);

    void createSimplePoll();
    void createClient();
    AvahiEntryGroup* createGroup();
    void commitGroup(AvahiEntryGroup* group);
};
#endif
