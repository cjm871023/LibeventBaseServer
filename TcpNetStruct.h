//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_TCPNETSTRUCT_H
#define LR_DATARECEIVE_TCPNETSTRUCT_H
#include <boost/function.hpp>
#include <event.h>
class CTcpSession;
typedef boost::function<void (evutil_socket_t, struct sockaddr*, int)> OnNewconnCallback;

typedef boost::function<void(boost::shared_ptr<CTcpSession> )>                   SessionConnectCallback;
typedef boost::function<void(boost::shared_ptr<CTcpSession> )>                   SessionDisConnectCallback;
typedef boost::function<void(boost::shared_ptr<CTcpSession> ,struct evbuffer* )> SessionMessageCallback;
typedef boost::function<void(boost::shared_ptr<CTcpSession> ,struct evbuffer*)>  SessionWriteCompleteCallback;

typedef boost::function<void(bool, evutil_socket_t)>  ConnectorResultCallback;
enum SOCKSTATE{ SS_CLOSED, SS_CONNECTING, SS_CONNECTED, SS_DISCONNECTING, SS_DISCONNECTED };
#endif //LR_DATARECEIVE_TCPNETSTRUCT_H
