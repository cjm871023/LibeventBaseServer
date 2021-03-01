//
// Created by chenjianmin on 2020/11/12.
//

#include "CTcpListen.h"
#include "glog/logging.h"
CTcpListen::CTcpListen( CLibeventThread* pThread  )
{
    m_pListener = NULL;
    m_pThread = pThread;
    m_NewConnCallback = NULL;
}
void    CTcpListen::SetNewconnCallback( OnNewconnCallback newCallback )
{
    m_NewConnCallback = newCallback;
}
CTcpListen::~CTcpListen()
{

}
void CTcpListen::StartListen( struct sockaddr_in& serAddr )
{
    unsigned flags =  LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE;
    while ( m_pListener == NULL )
    {
        m_pListener = evconnlistener_new_bind( m_pThread->m_pBase, CTcpListen::OnNewConnection, (void*)this, flags, SOMAXCONN,
                                              (struct sockaddr*)&serAddr, sizeof(serAddr) );
        if( m_pListener )
            break;
        boost::this_thread::sleep(boost::posix_time::seconds(2));
        LOG(ERROR)<< "Listen error,Wait 2 secs" ;
    }
    LOG(INFO)<< "Listen Sucess" ;
    evconnlistener_set_error_cb( m_pListener, CTcpListen::OnListenError );
}
void CTcpListen::OnNewConnection( evconnlistener* pListener, evutil_socket_t sockfd, sockaddr* pAddress, int addLen, void* data )
{
    CTcpListen* pServer = (CTcpListen*)data;
    if( pServer && pServer->m_NewConnCallback )
        pServer->m_NewConnCallback( sockfd, pAddress, addLen );

}
void CTcpListen::OnListenError( evconnlistener* pListener, void* data )
{
    printf( "on listen error" );
}
