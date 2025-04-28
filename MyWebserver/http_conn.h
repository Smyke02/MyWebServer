#ifndef _HTTP_CONN_H
#define _HTTP_CONN_H
#include<signal.h>

class http_coon
{
public:
    //�����ļ����ƴ�С
    static const int FILENAME_LEN = 200;
    //���ö���������С
    static const int READ_BUFFER_SIZE = 2048;
    //����д��������С
    static const int WRITE_BUFFER_SIZE = 1024;
    //�������󷽷�
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
    //��״̬����״̬
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    //���Ľ������
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
    //��״̬����״̬
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