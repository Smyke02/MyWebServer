#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <list>
#include <pub.h>
#include <pthread.h>
#include <exception>


template <typename T>
class threadpool
{
public:
    threadpool(int thread_number = 8 , int max_request = 10000){};
    ~threadpool(){};
    bool append(T *request , int state);
    bool append_p(T *request);

private:
    //工作线程运行的函数：不断从工作队列中取出任务并执行
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;            //线程池的数量
    int m_max_requests;                 //请求队列的最大数量
    pthread_t *m_threads;           //指向线程池的数组
    std::list<T *> m_workqueue;     //请求队列
};

#endif