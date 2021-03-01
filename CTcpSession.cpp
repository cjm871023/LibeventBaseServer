//
// Created by chenjianmin on 2020/11/12.
//

#include "CTcpSession.h"
#include "CFunctor.h"
void defaultConncalback(boost::shared_ptr<CTcpSession> )
{
}
void defaultDisconnCallback(boost::shared_ptr<CTcpSession> )
{
}
void defaultMessageCallback(boost::shared_ptr<CTcpSession> ,struct evbuffer* buffer )
{
    evbuffer_drain( buffer, evbuffer_get_length( buffer ) );
}
void defaultWriteComCallback(boost::shared_ptr<CTcpSession> ,struct evbuffer*)
{

}
CTcpSession::CTcpSession(int iSocket, CLibeventThread* p)
{
    m_pEventThread = p;
    m_iSocketID = iSocket;
    m_uSessionID = 0;
    m_pEventHeart = NULL;
    m_iHeartTime = 0;

    m_pBufEvent = bufferevent_socket_new( m_pEventThread->m_pBase, m_iSocketID, 0 );
    m_pBufferRead = bufferevent_get_input( m_pBufEvent);
    m_pBufferWrite = bufferevent_get_output( m_pBufEvent );
    evbuffer_enable_locking(m_pBufferWrite,NULL);

    m_pConnCallback = defaultConncalback;
    m_pDisConnCallback = defaultDisconnCallback;
    m_pMessageCallback = defaultMessageCallback;
    m_pWriteCompleCallback = defaultWriteComCallback;
    SetSockState( SS_CLOSED );
}
CTcpSession::~CTcpSession()
{
    HandleClose();
}
void  CTcpSession::Establish()
{
    //in same thread execute
    CFunctor *pFunctor = new CFunctor(boost::bind(&CTcpSession::InThreadEstabclish, shared_from_this()));
    event_base_once( m_pEventThread->m_pBase, -1, EV_TIMEOUT, &CFunctor::OnFunc, (void*)pFunctor, NULL );
}
void  CTcpSession::ShutDown()
{
    //in same thread execute
    CFunctor *pFunctor = new CFunctor(boost::bind(&CTcpSession::HandleClose, shared_from_this()));
    event_base_once( m_pEventThread->m_pBase, -1, EV_TIMEOUT, &CFunctor::OnFunc, (void*)pFunctor, NULL );
}
void  CTcpSession::SetHeartCheckTime( int iHeartTime )
{
    m_iHeartTime = iHeartTime;
    if(m_pEventHeart)
    {
        event_del(m_pEventHeart);
        event_free(m_pEventHeart);
        m_pEventHeart = NULL;
    }
    m_pEventHeart = event_new( m_pEventThread->m_pBase, -1, EV_TIMEOUT, HeartTimeout, (void*)(&m_WeakPtr) );
    StartHeartCheck();
}
void CTcpSession::StartHeartCheck()
{
    if( m_pEventHeart )
    {
        event_del( m_pEventHeart );
        timeval  timehear = { m_iHeartTime, 0 };
        event_add( m_pEventHeart, &timehear );
    }

}
void  CTcpSession::SendMsg( char* pMsg, int iLen )
{
    //in same thread execute
    char* pData = new char[iLen];
    memcpy( pData, pMsg, iLen );
    CFunctor *pFunctor = new CFunctor(boost::bind(&CTcpSession::InThreadSend, shared_from_this(), pData, iLen ));
    event_base_once( m_pEventThread->m_pBase, -1, EV_TIMEOUT, &CFunctor::OnFunc, (void*)pFunctor, NULL );
}

void  CTcpSession::InThreadSend( char* pMsg, int iLen )
{
    if( IsConnected() )
    {
        evbuffer_add( m_pBufferWrite, pMsg, iLen );
        delete[] pMsg;
    }
    else
    {
        int ii = 0;
        ii++;
    }
}
void  CTcpSession::InThreadEstabclish()
{
    SetSockState( SS_CONNECTED );
    m_WeakPtr = shared_from_this();
    bufferevent_setcb( m_pBufEvent, OnDataReceived, OnWriteComplete, OnException, (void*)(&m_WeakPtr));
    bufferevent_enable(m_pBufEvent,EV_READ);

    m_pConnCallback(shared_from_this());
}
void  CTcpSession::SetConnectCallback( SessionConnectCallback connCallback )
{
    m_pConnCallback = connCallback;
}
void  CTcpSession::SetDisconnectCallback( SessionDisConnectCallback disConnCallback)
{
    m_pDisConnCallback = disConnCallback;
}
void  CTcpSession::SetMessageCallback( SessionMessageCallback messageCallback)
{
    m_pMessageCallback = messageCallback;
}
void  CTcpSession::SetWriteCOmpleteCallback( SessionWriteCompleteCallback writeComCallback )
{
    m_pWriteCompleCallback = writeComCallback;
}
void  CTcpSession::SetSessionID( uint64_t uSessionID )
{
    m_uSessionID = uSessionID;
}
uint64_t CTcpSession::GetSessionID(  )
{
    return m_uSessionID;
}
void  CTcpSession::OnDataReceived( struct bufferevent* bev, void* data )
{
    boost::weak_ptr<CTcpSession>* p = (boost::weak_ptr<CTcpSession>*)(data);
    boost::shared_ptr<CTcpSession> pconn = p->lock();
    if( pconn )
    {

        //printf( "%x            %x\n", pconn->m_pBufferRead, bufferevent_get_input(bev));
        pconn->StartHeartCheck();
        pconn->m_pMessageCallback( pconn, pconn->m_pBufferRead );
    }
}
void  CTcpSession::OnWriteComplete(struct bufferevent* bev,void* data)
{
    boost::weak_ptr<CTcpSession>* p = (boost::weak_ptr<CTcpSession>*)(data);
    boost::shared_ptr<CTcpSession> pconn = p->lock();
    if( pconn )
    {
        pconn->m_pWriteCompleCallback( pconn, pconn->m_pBufferWrite );
    }
}
void  CTcpSession::OnException(struct bufferevent* bev,short what,void* data)
{
    boost::weak_ptr<CTcpSession>* p = (boost::weak_ptr<CTcpSession>*)(data);
    boost::shared_ptr<CTcpSession> pconn = p->lock();
    if( what & BEV_EVENT_EOF || what & BEV_EVENT_TIMEOUT )
    {
        pconn->HandleClose();
    }
    if( what & BEV_EVENT_ERROR || what & BEV_EVENT_READING || what & BEV_EVENT_WRITING )
    {
        pconn->HandleError();
    }
}

void    CTcpSession::HandleClose()
{
    if( IsConnected() == false )
        return;
    SetSockState( SS_DISCONNECTED );
    evutil_closesocket(m_iSocketID);
    bufferevent_disable( m_pBufEvent, EV_READ | EV_WRITE );
    m_pDisConnCallback(shared_from_this());

    bufferevent_free(m_pBufEvent);
    m_pBufEvent = NULL;

    if( m_pEventHeart)
    {
        event_free(m_pEventHeart);
        m_pEventHeart = NULL;
    }


}
void    CTcpSession::HandleError()
{
    switch( errno )
    {
        case EPIPE:
        case ECONNRESET:
        {
            HandleClose();
            break;
        }
    }
}
void   CTcpSession::SetSockState( SOCKSTATE  ss )
{
    m_SockState = ss;
}
bool  CTcpSession::IsConnected()
{
    return m_SockState == SS_CONNECTED;
}

void    CTcpSession::HeartTimeout( evutil_socket_t fd, short event, void* data )
{
    boost::weak_ptr<CTcpSession>* p = (boost::weak_ptr<CTcpSession>*)(data);
    boost::shared_ptr<CTcpSession> pconn = p->lock();
    if( pconn )
    {
        pconn->HandleClose();
    }
}