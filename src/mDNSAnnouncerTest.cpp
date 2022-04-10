#include <catch2/catch.hpp>

#include "mDNSAnnouncer.h"

TEST_CASE("Start the mDNS runnable object")
{
    MDNSAnnouncer announcer;
    announcer.start();
}

TEST_CASE("Stop the mDNS runnable object")
{
    MDNSAnnouncer announcer;
    announcer.start();
    announcer.stop();
}

TEST_CASE("Cannot run more than one client running at a time")
{
    MDNSAnnouncer announcer;
    announcer.start();
    announcer.start();
}
