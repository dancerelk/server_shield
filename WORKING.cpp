#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
using namespace std;

char buf[1000];
struct pollfd poll_set[1000];
int n = 0;

void file(char *buff);
void file(char *buff)
{
    int i=0,j;
    while (buff[i]!='\n')
        i++;
    j=i;
    for (i=5; i<j-10; i++)
        buf[i-5]=buff[i];
    buf[j-15]=0;
    //printf("%s\n",buf);
}

void send(int sock);
void send(int sock)
{
    FILE *f;
    f=fopen(buf, "rb");
    //printf("%s\n",buf);
    if(f==0)
    {
        char buf1[]="HTTP/1.0 404 NOT FOUND\r\nConnection: close\r\n\r\n";
        send(sock, buf1, strlen(buf1), 0);
    }
    else{
    char buf1[]="HTTP/1.0 200 OK\r\nConnection: close\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: ";
    fseek(f, 0, SEEK_END);
    int x=ftell(f);

    fseek(f, 0, SEEK_SET);
    char buf2[100];
    //printf("%d",sizeof(buf1) + sizeof(buf2)+4);
    sprintf(buf2,"%d",x);
    char *buf3=new char[strlen(buf1) + strlen(buf2)+4];
    strcpy(buf3, buf1);
    strcat(buf3, buf2);
    strcat(buf3, "\r\n\r\n");
    strcat(buf3, "\0");

    send(sock, buf3, strlen(buf3), 0);

    buf2[0] = '\0';
    while(!feof(f))
    {
        x = fread(buf2, 1 , 100, f);
        //printf("%s\n",buff1);
        send(sock, buf2, x, 0);
    }}
    fclose(f);
    //close(sock);
}

int main(){

    int sock, ret, sock_peer;
    map <string, int> mp;
    map <int,string> mq;
    sockaddr_in addr;
    sockaddr_in peer_addr;
    socklen_t peer_addr_size;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
      printf("Error in socket creation\n");
      exit(1);
    }
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8090);
    addr.sin_family = AF_INET;
    //printf("Socket OK\n");
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if(ret<0) {
      printf("Cannot bind to port\n");
      close(sock);
    exit(1);
    }
    //struct linger linger_opt = {1, 0};
    //setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
    //printf("Bind OK\n");
    if(listen(sock, 10) != 0) {
      printf("Cannot listen\n");
      close(sock);
      exit(1);
    }
    poll_set[0].fd=sock;
    poll_set[0].events=POLLIN;
    n++;

    while (1){

        int fd_index=0;
        int nread;
        int k=n;
        printf("Waiting for client (%d total)...\n", n-1);
        poll(poll_set, n, 10000);
        for(fd_index = 0; fd_index < k; fd_index++)
        {
            //printf("%d\n",fd_index);
            if( poll_set[fd_index].revents && POLLIN) {
                    poll_set[fd_index].revents = 0;
                if(poll_set[fd_index].fd == sock) {
                    sock_peer = accept(sock, (struct sockaddr*)&peer_addr, &peer_addr_size);
                    if (sock_peer>0){
                            char *ipStr = inet_ntoa( peer_addr.sin_addr );
                            string str=(ipStr);
                            if (mp[str]<4)
                            {
                                mp[str]++;
                                mq[sock_peer]=str;
                                //printf("%d %s\n",mp[ipStr],mq[sock_peer]);
                                poll_set[n].fd = sock_peer;
                                poll_set[n].events = POLLIN;
                                n++;
                            }
                                    }
                }
                else{
                    char buff[1000];
                    ret = recv(poll_set[fd_index].fd, buff, sizeof(buff)-1, 0);
                    //printf("%d\n",ret);
                    if(ret<=0)
                    {
                        close(poll_set[fd_index].fd);
                        poll_set[fd_index].events = 0;
                        poll_set[fd_index].fd = -1; //printf("X%d\n",fd_index);
                    }
                    if (ret>0)
                    {
                        file(buff); send(poll_set[fd_index].fd);
                        close(poll_set[fd_index].fd);
                        poll_set[fd_index].events = 0;
                        mp[mq[poll_set[fd_index].fd]]--;
                        //printf("%d\n",mp[mq[poll_set[fd_index].fd]]);
                        poll_set[fd_index].fd = -1; //printf("X%d\n",fd_index);
                    }
                    }
                    }
                    }
                    for(fd_index = 0; fd_index < n; fd_index++)
                    {
                        if (poll_set[fd_index].fd==-1)
                        {
                            int i = fd_index;
                            for(i = fd_index; i<n-1; i++)
                            {
                                poll_set[i] = poll_set[i + 1];
                            }
                            n--;
                            fd_index--;
                        }
                    }
    } }
