//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_CTCPSESSION_H
#define LR_DATARECEIVE_CTCPSESSION_H


#include "CLibeventThread.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <boost/enable_shared_from_this.hpp>
#include "TcpNetStruct.h"

class CTcpSession : public boost::enable_shared_from_this<CTcpSession>
{
public:
    CTcpSession(int iSocket, CLibeventThread* p);
    ~CTcpSession();
    void  Establish();
    void  ShutDown();
    void  SendMsg( char* pMsg, int iLen );
    void  SetSessionID( uint64_t uSessionID );
    uint64_t GetSessionID(  );
    void  SetConnectCallback( SessionConnectCallback connCallback );
    void  SetDisconnectCallback( SessionDisConnectCallback disConnCallback);
    void  SetMessageCallback( SessionMessageCallback messageCallback);
    void  SetWriteCOmpleteCallback( SessionWriteCompleteCallback writeComCallback );
    bool  IsConnected();
    void  SetHeartCheckTime( int iHeartTime );
private:
    void  InThreadEstabclish();
    void  InThreadSend( char* pMsg, int iLen );
    static void  OnDataReceived( struct bufferevent* bev, void* data );
    static void  OnWriteComplete(struct bufferevent* bev,void* data);
    static void  OnException(struct bufferevent* bev,short what,void* data);
    void   HandleClose();
    void   HandleError();
    void   SetSockState( SOCKSTATE  ss );

    void StartHeartCheck();
    static void  HeartTimeout( evutil_socket_t fd, short event, void* data );
private:
    int                          m_iSocketID;
    SOCKSTATE                    m_SockState;
    CLibeventThread*             m_pEventThread;
    uint64_t                     m_uSessionID;
    uint32_t                     m_uHeartTime;
    //weakptr seems not working, because when the object destruct, weakptr is not
    //exist too,so ,interesting...
    boost::weak_ptr<CTcpSession> m_WeakPtr;
    struct bufferevent*          m_pBufEvent;
    struct evbuffer*             m_pBufferRead;
    struct evbuffer*             m_pBufferWrite;
    struct event*                m_pEventHeart;
    int                          m_iHeartTime;

    SessionConnectCallback         m_pConnCallback;
    SessionDisConnectCallback      m_pDisConnCallback;
    SessionMessageCallback         m_pMessageCallback;
    SessionWriteCompleteCallback   m_pWriteCompleCallback;
};


#endif //LR_DATARECEIVE_CTCPSESSION_H
