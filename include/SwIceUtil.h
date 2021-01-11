#pragma once
#include <Ice/Ice.h>
struct SwConnectionInfo
{
    std::string localAddr;
    std::string localPort;
    std::string remoteAddr;
    std::string remotePort;

    inline operator std::string() const { return remoteAddr; }
};

class SwIceUtil
{
public:
    static SwConnectionInfo GetConnectionInfo(const Ice::ObjectPrx& prx);
    static SwConnectionInfo GetConnectionInfo(const ::Ice::Current& cur);
};
