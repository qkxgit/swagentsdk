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
            swc.localPort = addrInfo->localPort;
            swc.remoteAddr = addrInfo->remoteAddress;
            swc.remotePort = addrInfo->remotePort;
        }
    }
    return swc;
}
