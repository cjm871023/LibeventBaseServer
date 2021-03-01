//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_CTCPLISTEN_H
#define LR_DATARECEIVE_CTCPLISTEN_H
#include <event2/listener.h>
#include "CTcpSession.h"
#include "CLibeventThread.h"
#include "TcpNetStruct.h"

class CTcpListen
{
public:
    CTcpListen( CLibeventThread* pThread );
    void    SetNewconnCallback( OnNewconnCallback newCallback );
    ~CTcpListen();
    void StartListen( struct sockaddr_in& serAddr );
private:
    static void OnListenError( evconnlistener* pListener, void* data );
    static void OnNewConnection( evconnlistener* pListener, evutil_socket_t sockfd, sockaddr* pAddress, int addLen, void* data );
private:
    evconnlistener*          m_pListener;
    CLibeventThread*         m_pThread;
    OnNewconnCallback        m_NewConnCallback;
};


#endif //LR_DATARECEIVE_CTCPLISTEN_H
