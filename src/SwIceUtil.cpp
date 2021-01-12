#include "SwIceUtil.h"

SwConnectionInfo SwIceUtil::GetConnectionInfo(const Ice::ObjectPrx& prx)
{
    SwConnectionInfo swc;
    Ice::ConnectionPtr conn = prx->ice_getConnection();
    if (conn)
    {
        Ice::IPConnectionInfoPtr addrInfo = Ice::IPConnectionInfoPtr::dynamicCast(conn->getInfo());
        if (addrInfo)
        {
            swc.localAddr = addrInfo->localAddress;
            swc.localPort = addrInfo->localPort;
            swc.remoteAddr = addrInfo->remoteAddress;
            swc.remotePort = addrInfo->remotePort;
        }
    }
    return swc;
}

SwConnectionInfo SwIceUtil::GetConnectionInfo(const ::Ice::Current& cur)
{
    SwConnectionInfo swc;
    if (cur.con)
    {
        Ice::IPConnectionInfoPtr addrInfo = Ice::IPConnectionInfoPtr::dynamicCast(cur.con->getInfo());
        if (addrInfo)
        {
            swc.localAddr = addrInfo->localAddress;
            size_t pos = swc.localAddr.find_last_of(':');
            if (pos != std::string::npos && pos + 1 < swc.localAddr.size())
                swc.localAddr = swc.localAddr.substr(pos + 1);
            swc.localPort = addrInfo->localPort;
            swc.remoteAddr = addrInfo->remoteAddress;
            pos = swc.remoteAddr.find_last_of(':');
            if (pos != std::string::npos && pos + 1 < swc.remoteAddr.size())
                swc.remoteAddr = swc.remoteAddr.substr(pos + 1);
            swc.remotePort = addrInfo->remotePort;
        }
    }
    return swc;
}
