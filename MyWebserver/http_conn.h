#ifndef _HTTP_CONN_H
#define _HTTP_CONN_H
#include<signal.h>

class http_coon
{
public:
    //设置文件名称大小
    static const int FILENAME_LEN = 200;
    //设置读缓存区大小
    static const int READ_BUFFER_SIZE = 2048;
    //设置写缓存区大小
    static const int WRITE_BUFFER_SIZE = 1024;
    //报文请求方法
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    //主状态机的状态
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    //报文解析结果
    enum HTTP_CODE
    {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    //从状态机的状态
    enum LINE_STATUS
    {
        LINE_OK=0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    http_coon(){};
    ~http_coon(){};

private:
};


#endif