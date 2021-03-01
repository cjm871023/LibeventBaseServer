//
// Created by chenjianmin on 2020/11/12.
//

#include "CTcpServer.h"
#include <event2/thread.h>
#include "glog/logging.h"
CTcpServer::CTcpServer()
{
    m_pListenThread = NULL;
    m_pCommunThread = NULL;
    m_uClientID = 0;
    evthread_use_pthreads();
}

CTcpServer::~CTcpServer()
{

}
void CTcpServer::StartServer( struct sockaddr_in& serAddr )
{
    m_pListenThread = new CLibeventThread;
    m_pListenThread->Start();
    m_pCommunThread = new CLibeventThread;
    m_pCommunThread->Start();

    m_pListen = new CTcpListen(m_pListenThread);
    m_pListen->SetNewconnCallback( boost::bind(&CTcpServer::OnNewConnection, this, _1, _2, _3 ));
    m_pListen->StartListen( serAddr );
}
void CTcpServer::StopServer()
{

}


void CTcpServer::OnNewConnection( evutil_socket_t sockfd, sockaddr* pAddress, int addLen )
{
    boost::shared_ptr<CTcpSession> session(new CTcpSession(sockfd, m_pCommunThread));
    session->SetConnectCallback(boost::bind(&CTcpServer::OnSessionConnect, this, _1 ));
    session->SetDisconnectCallback(boost::bind(&CTcpServer::OnSessionDisconn, this, _1 ));
    session->SetMessageCallback(boost::bind(&CTcpServer::OnSessionMessageCallback, this, _1, _2 ));
    session->Establish();
}
void CTcpServer::OnSessionConnect(boost::shared_ptr<CTcpSession> psession )
{
    uint64_t uSessionID = m_uClientID++;
    psession->SetSessionID( uSessionID );
    m_mAllConns[uSessionID] = psession;


}
void CTcpServer::OnSessionDisconn(boost::shared_ptr<CTcpSession> psession)
{
    uint64_t uSessionID = psession->GetSessionID();
    auto itor =m_mAllConns.find(uSessionID);
    if( itor != m_mAllConns.end() )
    {
        m_mAllConns.erase( itor );
    }
}
void CTcpServer::OnSessionMessageCallback(boost::shared_ptr<CTcpSession> session ,struct evbuffer* buffer )
{
    //默認直接清除緩存
    int iLen = evbuffer_get_length(buffer);
    evbuffer_drain( buffer, iLen );
}
