#include <event2/event.h>
#include <signal.h>
#include <iostream>
#include <errno.h>
#include <string.h>
using namespace std;
#define SPORT 5001

/* 正常断开连接会进入， 超时也会进入 */
void client_cb(evutil_socket_t s, short w, void *arg)
{
    event *ev = (event *)arg;
    /* 判断超时 */
    if (w & EV_TIMEOUT) {
        cout << "timeout" << endl;
        event_free(ev);
        evutil_closesocket(s);
        return;
    }
    char buf[1024] = {0};
    int len = recv(s, buf, sizeof(buf) - 1, 0);
    if (len > 0) {
        cout << buf << endl;
        send(s, "ok", 2, 0);
    } else {
        /* 需要清理event */
        cout << "event_free" << flush;
        event_free(ev);
        evutil_closesocket(s);
    }
}
void listen_cb(evutil_socket_t s, short w, void *arg)
{
    cout << "listen_cb" << endl;
    sockaddr_in sin;
    socklen_t size = sizeof(sin);

    /* 读取连接信息 */
    evutil_socket_t client = accept(s, (sockaddr *)&sin, &size);
    char ip[16] = {0};
    evutil_inet_ntop(AF_INET, &sin.sin_addr, ip, sizeof(ip) - 1);
    cout << "client ip is " << ip << endl;
    /* 客户端数据读取事件 */
    event_base *base = (event_base *)arg;
    event *ev = event_new(base, client, EV_READ | EV_PERSIST, client_cb, event_self_cbarg());
    timeval t = {10, 0};
    event_add(ev, &t);
}

int main(int argc, char *argv[])
{
    /* 忽略管道信号, 发送数据给已关闭的socket */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        return 1;
    }

    event_base *base = event_base_new();

    /* event服务器 */
    cout << "test event server" << endl;

    /* 创建socket */
    evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0) {
        cout << "socket error: " << strerror(errno) << endl;
        return -1;
    }

    /* 设置地址复用和非阻塞 */
    evutil_make_socket_nonblocking(sock);
    evutil_make_listen_socket_reuseable(sock);
    

    /* 绑定端口和地址 */
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);
    
    int ret = ::bind(sock, (sockaddr *)&sin, sizeof(sin));
    if (ret != 0) {
        cerr << "bind error:" << strerror(errno) << endl;
        return -1;
    }

    /* 开始监听 */
    listen(sock, 10);

    /* 开始接受连接事件 默认水平触发 */
    event *ev = event_new(base, sock, EV_READ | EV_PERSIST, listen_cb, base);
    event_add(ev, 0);

    

    /* 进入事件主循环 */
    event_base_dispatch(base);
    event_base_free(base);
    evutil_closesocket(sock);
    return 0;
}