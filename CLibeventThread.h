//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_CLIBEVENTTHREAD_H
#define LR_DATARECEIVE_CLIBEVENTTHREAD_H
#include <event2/event.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
class CLibeventThread
{
public:
    CLibeventThread();
    ~CLibeventThread();
    //start 必须被调用
    void Start();

    int                 m_pid;
    struct event_base*  m_pBase;
private:
    boost::shared_ptr<boost::thread>  m_pWorkThread;
    boost::condition_variable         m_Condition;
    boost::mutex                      m_mutex;
    struct  event *m_pEvnetPersist;
private:
    void StartWork();
    void StartPersist();
    void StopPersist();
    static void OnPersist( evutil_socket_t fd, short swhat, void* pdata );
};


#endif //LR_DATARECEIVE_CLIBEVENTTHREAD_H
