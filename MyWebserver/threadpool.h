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
    //�����߳����еĺ��������ϴӹ���������ȡ������ִ��
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;            //�̳߳ص�����
    int m_max_requests;                 //������е��������
    pthread_t *m_threads;           //ָ���̳߳ص�����
    std::list<T *> m_workqueue;     //�������
};

#endif