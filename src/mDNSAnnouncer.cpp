#include "mDNSAnnouncer.h"

MDNSAnnouncer::MDNSAnnouncer()
{
    createSimplePoll();
    createClient();
}

MDNSAnnouncer::~MDNSAnnouncer()
{
    try
    {
        stop();
        avahi_client_free(m_client);
        avahi_simple_poll_free(m_poll);
    }
    catch (...)
    {
        poco_unexpected();
    }
}

void MDNSAnnouncer::registerService(const MDNSService& service)
{
    std::scoped_lock lock(m_mutex);
    auto group = createGroup();
    try
    {
        const char* service_domain = 0;
        const char* service_host = 0;
        int error = avahi_entry_group_add_service_strlst(
            group,
            service.getNetworkInterface(),
            AVAHI_PROTO_INET,
            AVAHI_PUBLISH_USE_MULTICAST,
            service.getServiceName().c_str(),
            service.getServiceType().c_str(),
            service_domain,
            service_host,
            service.getPort(),
            service.getTXTStringList()
        );
        if (error < 0) throw("Failed to add service to entry group");

        commitGroup(group);
    }
    catch (...)
    {
        std::cerr << "Error: Unexpected error when registering new service\n";
        avahi_entry_group_free(group);
    }
}

void MDNSAnnouncer::start()
{
    if (!m_running)
    {
        if (!m_thread.joinable())
        {
            m_thread = std::thread([this] { run(); });

            // Precaution to ensure client is actually running before continuing callers thread.
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait_for(lock, std::chrono::seconds(1));
            m_running = true;
        }
    }
}

void MDNSAnnouncer::stop()
{
    if (m_running)
    {
        {
            std::scoped_lock lock(m_mutex);
            avahi_simple_poll_quit(m_poll);
        }
        m_thread.join();
        m_running = false;
    }
}

void MDNSAnnouncer::run()
{
    std::scoped_lock lock(m_mutex);
    avahi_simple_poll_loop(m_poll);
}

void MDNSAnnouncer::createSimplePoll()
{
    m_poll = avahi_simple_poll_new();
    if (!m_poll)
    {
        throw("Failed to create Avahi poll object");
    }
    avahi_simple_poll_set_func(m_poll, this->onPoll, &m_mutex);
}

void MDNSAnnouncer::createClient()
{
    m_client = avahi_client_new(avahi_simple_poll_get(m_poll), AVAHI_CLIENT_NO_FAIL, this->onClientStateChange, this, &m_error);
}

AvahiEntryGroup* MDNSAnnouncer::createGroup()
{
    AvahiEntryGroup* group = avahi_entry_group_new(m_client, this->onGroupStateChange, this);
    if (!group)
    {
        throw("Failed to create Avahi entry group object");
    }
    return group;
}

void MDNSAnnouncer::commitGroup(AvahiEntryGroup* group)
{
    int ret;
    if ((ret = avahi_entry_group_commit(group)) < 0)
    {
        std::cerr << "Failed to commit entry group: " << avahi_strerror(ret) << '\n';
    }
}

int MDNSAnnouncer::onPoll(struct pollfd* ufds, unsigned int nfds, int timeout, void* userdata)
{
    std::mutex* pMutex = reinterpret_cast<std::mutex*>(userdata);
    pMutex->unlock();
    int ret = poll(ufds, nfds, timeout);
    pMutex->lock();
    return ret;
}

void MDNSAnnouncer::onClientStateChange(AvahiClient* client, AvahiClientState state, void* userdata)
{
    try
    {
        MDNSAnnouncer* pAnnouncer = reinterpret_cast<MDNSAnnouncer*>(userdata);
        switch (state)
        {
        case AVAHI_CLIENT_S_RUNNING:
            std::cout << "AVAHI_CLIENT_S_RUNNING\n";
            pAnnouncer->m_cv.notify_all();
            break;
        case AVAHI_CLIENT_S_REGISTERING:
            std::cout << "AVAHI_CLIENT_S_REGISTERING\n";
            break;
        case AVAHI_CLIENT_S_COLLISION:
            std::cout << "AVAHI_CLIENT_S_COLLISION \n";
            break;
        case AVAHI_CLIENT_FAILURE:
            std::cout << "AVAHI_CLIENT_FAILURE \n";
            break;
        case AVAHI_CLIENT_CONNECTING:
            std::cout << "AVAHI_CLIENT_CONNECTING \n";
            break;
        }
    }
    catch (...)
    {
        std::cerr << "Unexpected error onClientStateChange\n";
    }
}

void MDNSAnnouncer::onGroupStateChange(AvahiEntryGroup* avahiGroup, AvahiEntryGroupState state, void* userdata)
{
    try
    {
        MDNSAnnouncer* pAnnouncer = reinterpret_cast<MDNSAnnouncer*>(userdata);

        switch (state)
        {
        case AVAHI_ENTRY_GROUP_UNCOMMITED:
            std::cout << "GROUP Registering\n";
            break;
        case AVAHI_ENTRY_GROUP_REGISTERING:
            std::cout << "GROUP Registering\n";
            break;
        case AVAHI_ENTRY_GROUP_ESTABLISHED:
            std::cout << "GROUP Established\n";
            break;
        case AVAHI_ENTRY_GROUP_COLLISION:
            std::cerr << "Name collision detected.  Service with the same name running on the network!\n";
            break;
        case AVAHI_ENTRY_GROUP_FAILURE:
            int error = avahi_client_errno(pAnnouncer->m_client);
            std::cerr << "Avahi Client has failed with the following errno: " << error << '\n';
            break;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
