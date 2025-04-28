#include"threadpool.h"

template <typename T>
threadpool<T>::threadpool(int thread_number = 8 , int max_request = 10000):m_thread_number(thread_number) , 
                            m_max_request(max_request) , m_threads(NULL);
{
    if (thread_number <= 0 || max_requests <= 0)
    throw std::exception();
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
    throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}