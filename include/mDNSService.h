#ifndef ES_MDNS_SERVICE_INCLUDED
#define ES_MDNS_SERVICE_INCLUDED

#include <Poco/Net/NameValueCollection.h>
#include <Poco/Net/NetworkInterface.h>
#include <avahi-common/strlst.h>


/// The Properties of a resolved service contains the
/// contents of the associated TXT record.
///
/// The TXT record contains key-value pairs in
/// the form <key>=<value>. Each pair is preceded
/// by a length byte giving the total length
/// of the pair. Thus the total length of key
/// plus value must not exceed 254 bytes.
/// Keys are case insensitive and must consists
/// entirely of printable US-ASCII characters.
/// The length of a key should not exceed nine bytes,
/// to keep packet sizes small. Values can contain
/// arbitrary bytes. A value can also be empty; in
/// this case the '=' character can be omitted in
/// the record.
typedef Poco::Net::NameValueCollection Properties;

class MDNSService
{
public:
    MDNSService() = delete;
    MDNSService(const std::string& name, Poco::Int32 net_if, Poco::UInt16 port, const std::string& type);

    void addTXTRecord(const std::string& name, const std::string& value);
    AvahiStringList* getTXTStringList() const;

    const std::string& getServiceName() const;
    const std::string& getServiceType() const;
    Poco::Int32 getNetworkInterface() const;
    Poco::UInt16 getPort() const;
    const Properties& getProperties() const;

private:
    std::string m_name;
    Poco::Int32 m_net_if;
    Poco::UInt16 m_port;
    std::string m_type;
    Properties m_properties;
};

inline MDNSService::MDNSService(const std::string& name, Poco::Int32 net_if, Poco::UInt16 port, const std::string& type) :
    m_name(name),
    m_net_if(net_if),
    m_port(port),
    m_type(type),
    m_properties()
{
}

inline void MDNSService::addTXTRecord(const std::string& name, const std::string& value)
{
    m_properties.add(name, value);
}

inline AvahiStringList* MDNSService::getTXTStringList() const
{
    AvahiStringList* list = 0;
    auto iter = m_properties.begin();
    for (; iter != m_properties.end(); ++iter)
    {
        list = avahi_string_list_add_pair_arbitrary(
            list,
            iter->first.c_str(),
            reinterpret_cast<const uint8_t*>(iter->second.c_str()),
            iter->second.size()
        );
    }
    return list;
}

inline const std::string& MDNSService::getServiceName() const
{
    return m_name;
}

inline const std::string& MDNSService::getServiceType() const
{
    return m_type;
}

inline const Properties& MDNSService::getProperties() const
{
    return m_properties;
}

inline Poco::Int32 MDNSService::getNetworkInterface() const
{
    return m_net_if;
}

inline Poco::UInt16 MDNSService::getPort() const
{
    return m_port;
}

#endif
