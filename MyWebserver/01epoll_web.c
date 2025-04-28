#include<stdio.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include"dirent.h"
#include"pub.h"
#include"wrap.h"
#define PORT 8080

void send_header(int cfd , int code , char *info , char *filetype , int lenght)
{
    char buf[1024] = "";
    int len = 0;
    len = sprintf(buf , "HTTP/1.1 %d %s\r\n" , code , info);
    send(cfd , buf , len , 0);
    len = sprintf(buf , "Content-Type:%s\r\n" , filetype);
    send(cfd , buf , len , 0);
    if(lenght > 0)
    {
        len = sprintf(buf , "Content-Lenght:%d\r\n" , lenght);
        send(cfd , buf , len , 0);
    }
    send(cfd , "\r\n" , 2 , 0);
}

void send_file(int cfd , char *path , struct epoll_event *ev , int epfd , int flag)
{
    int fd = open(path , O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        return;
    }
    char buf[1024] = "";
    int len = 0;
    while(1)
    {
        len = read(fd , buf , sizeof(buf));
        if(len < 0)
        {
            perror("read");
            break;
        }
        else if(len == 0)
            break;
        else
        {
            send(cfd , buf , len , 0);
        }
    }
    close(fd);
    if(flag == 1)
    {
        close(cfd);
        epoll_ctl(epfd , EPOLL_CTL_DEL , cfd , ev);
    }
}

void read_client_request(int epfd , struct epoll_event *ev)
{
    //读取请求(先读取第一行，再把其他行读取，扔掉)
    char buf[1024] = "";
    char tmp[1024] = "";
    int n = Readline(ev->data.fd, buf, sizeof(buf));
    if(n <= 0)
    {
        printf("close or error...\n");
        epoll_ctl(epfd , EPOLL_CTL_DEL , ev->data.fd , ev);
        close(ev->data.fd);
        return;
    }
    printf("[%s]\n", buf);
    int ret = 0;
    while( (ret = Readline(ev->data.fd, tmp, sizeof(tmp))) > 0 );
    //printf("read ok!!!\n");

    //解析请求
    char method[256] = "";
    char content[256] = "";
    char protocol[256] = "";
    sscanf(buf , "%[^ ] %[^ ] %[^ \r\n]" , method , content , protocol);
    printf("[%s] [%s] [%s]\n" , method , content , protocol);

    //判断是否为 GET/get 请求
    if( strcasecmp(method , "get") == 0)
    {
        char *strfile = content + 1;
        strdecode(strfile , strfile);
        //判断请求的文件是否存在
        if(*strfile == 0)
            strfile = "./";
        struct stat sf;
        if(stat(strfile , &sf)< 0)
        {
            printf("The file not fount....\n");

            send_header(ev->data.fd , 404 , "NOT FOUNT" , get_mime_type("*.html") , 0);

            send_file(ev->data.fd , "error.html" , ev , epfd , 1);
        }
        else
        {
            //请求的是一个普通文件
            if(S_ISREG(sf.st_mode))
            {
                printf("file...\n");

                send_header(ev->data.fd , 200 , "OK" , get_mime_type(strfile) , sf.st_size);
            
                send_file(ev->data.fd , strfile , ev , epfd , 1);
                
            }
            else if(S_ISDIR(sf.st_mode))     //请求的是一个目录
            {
                printf("dir...\n");

                send_header(ev->data.fd , 200 , "OK" , get_mime_type("*.html") , 0);

                send_file(ev->data.fd , "dir_header.html" , ev , epfd , 0);
                //send_file(ev->data.fd , "dir_header.html" , ev , epfd , 0);
                struct dirent **mylist = NULL;
                char buf[1024] = "";
                int len = 0;
                int num = scandir(strfile , &mylist , NULL , alphasort);
                for(int i = 0; i < num; i++)
                {
                    if(mylist[i]->d_type == DT_DIR)
                    {
                        len = sprintf(buf , "<li><a href=%s/> %s </a></li>" , mylist[i]->d_name , mylist[i]->d_name);
                    }
                    else
                    {
                        len = sprintf(buf , "<li><a href=%s> %s </a></li>" , mylist[i]->d_name , mylist[i]->d_name);
                    }
                    send(ev->data.fd , buf , len , 0);
                }
                send_file(ev->data.fd , "dir_tail.html" , ev , epfd , 1);

            }
        }


    }
}

int main(int argc , char *argv[])
{
    //切换工作目录
    //获取当前目录的工作路径
    char pwd_path[256] = "";
    char *path = getenv("PWD");
    strcpy(pwd_path , path);
    strcat(pwd_path , "/web-http");
    chdir(pwd_path);

    signal(SIGPIPE , SIG_IGN);

    //创建套接字    绑定
    int lfd = tcp4bind(PORT,NULL);

    //监听
    Listen(lfd , 128);

    //创建树
    int epfd = epoll_create(1);

    //将lfd上树
    struct epoll_event ev , evs[1024];
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd , EPOLL_CTL_ADD , lfd , &ev);

    //循环监听
    while(1)
    {
        int nready = epoll_wait(epfd , evs , 1024 , -1);
        if(nready < 0)
        {
            perror("epoll_wait");
            break;
        }
        else if(nready == 0)
            continue;
        else
        {
            for(int i = 0;i < nready;i++)
            {
                //判断是否是lfd
                if((evs[i].data.fd == lfd) && (evs[i].events & EPOLLIN))
                {
                    struct sockaddr_in cliaddr;
                    char ip[16] = "";
                    socklen_t len = sizeof(cliaddr);
                    int cfd = Accept(lfd , (struct sockaddr *)&cliaddr , &len);
                    printf("A new client connect , ip = %s , port = %d\n", 
                            inet_ntop(AF_INET , &cliaddr.sin_addr.s_addr , ip , 16) ,
                            ntohs(cliaddr.sin_port));

                    //设置cfd非阻塞
                    int flag = fcntl(cfd , F_GETFL);
                    flag |= O_NONBLOCK;
                    fcntl(cfd , F_SETFL , flag);
                    

                    //上树、
                    ev.data.fd = cfd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd , EPOLL_CTL_ADD , cfd , &ev);
                }
                else if(evs[i].events & EPOLLIN)    //cfd变化
                {
                    read_client_request(epfd , &evs[i]);

                }
            }
        }
    }
    //收尾
}