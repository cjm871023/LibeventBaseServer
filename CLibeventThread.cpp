//
// Created by chenjianmin on 2020/11/12.
//

#include "CLibeventThread.h"
#include <boost/lexical_cast.hpp>
CLibeventThread::CLibeventThread()
{
    m_pBase = NULL;
    m_pEvnetPersist = NULL;
}

CLibeventThread::~CLibeventThread()
{

}
void CLibeventThread::Start()
{
    if( m_pBase != NULL )
    {
        return;
    }
    boost::unique_lock<boost::mutex> lock( m_mutex );
    m_pWorkThread.reset(new boost::thread(&CLibeventThread::StartWork, this ));
    m_Condition.wait( lock );
}
void CLibeventThread::StartWork()
{
    boost::unique_lock<boost::mutex> lock( m_mutex );
    m_pBase = event_base_new();
    std::string strID = boost::lexical_cast<std::string>(boost::this_thread::get_id());
    m_pid = atoi( strID.c_str() );
    StartPersist();
    m_Condition.notify_one();
    lock.unlock();

    event_base_dispatch( m_pBase );
    event_base_free( m_pBase );
}
void CLibeventThread::StartPersist()
{
    struct timeval interval = { 60, 0 };
    m_pEvnetPersist = event_new( m_pBase, -1, EV_PERSIST, OnPersist, NULL );
    event_add( m_pEvnetPersist, &interval );
}
void CLibeventThread::StopPersist()
{
    if( m_pEvnetPersist )
    {
        event_del( m_pEvnetPersist );
        event_free( m_pEvnetPersist );
    }
}
void CLibeventThread::OnPersist( evutil_socket_t fd, short swhat, void* pdata )
{

}