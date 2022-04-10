#ifndef ES_ANNOUNCER_INTERFACE_INCLUDED
#define ES_ANNOUNCER_INTERFACE_INCLUDED

/**
 * Interface for the possible annoucers
*/
class IAnnouncer
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void run() = 0;
};

#endif
