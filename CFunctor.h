//
// Created by chenjianmin on 2020/11/12.
//

#ifndef LR_DATARECEIVE_CFUNCTOR_H
#define LR_DATARECEIVE_CFUNCTOR_H
#include <boost/function.hpp>
#include <event2/event.h>
typedef boost::function<void()> FunctorCallback;
class CFunctor
{
public:
    CFunctor(FunctorCallback callback)
    {
        m_Func = callback;
    }
    static void OnFunc(evutil_socket_t fd, short events, void* arg )
    {
        if( arg )
        {
            CFunctor* pFunctor = (CFunctor*)arg;
            pFunctor->m_Func();

            delete pFunctor;
        }
    }

    FunctorCallback m_Func;
};
#endif //LR_DATARECEIVE_CFUNCTOR_H
