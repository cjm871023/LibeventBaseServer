//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_CTCPSERVER_H
#define LR_DATARECEIVE_CTCPSERVER_H
#include "CTcpListen.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include "CLibeventThread.h"
class CTcpServer
{
public:
    CTcpServer();
    ~CTcpServer();
    void StartServer( struct sockaddr_in& serAddr );
    void StopServer();
    void OnNewConnection( evutil_socket_t sockfd, sockaddr* pAddress, int addLen );

protected:
    virtual void  OnSessionConnect(boost::shared_ptr<CTcpSession> );
    virtual void  OnSessionDisconn(boost::shared_ptr<CTcpSession> );
    virtual void  OnSessionMessageCallback(boost::shared_ptr<CTcpSession> ,struct evbuffer* buffer );
private:
    CTcpListen*                                         m_pListen;
    std::map<uint64_t, boost::shared_ptr<CTcpSession>>  m_mAllConns;
    CLibeventThread*                                    m_pListenThread;
    CLibeventThread*                                    m_pCommunThread;
    uint64_t                                            m_uClientID;
};


#endif //LR_DATARECEIVE_CTCPSERVER_H
