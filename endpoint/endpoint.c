/**
 * @brief   智慧农业采集控制系统数据采集终端
 * @author  Mculover666
 * @date    2020/05/10
 * @note    本系统使用开源库cjson解析数据
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ENDPOINT_ID "2016211961"

int main(int argc, char* argv[])
{
    int sock_fd;
    struct sockaddr_in server_addr;
    char recv_buf[100];
    int nbytes = 0;
    socklen_t len = 0;
    time_t cur_time;
    struct tm *p;

    //终端数据
    float temp = 0.0;
    float humi = 0.0;
    char motor[5] = "off";
    char sw[5] = "on";

    /* 打印启动界面 */
    printf("************************************************\n");
    printf("*                                              *\n");
    printf("*       欢迎使用智慧农业采集控制系统!(终端)       *\n");
    printf("*                                              *\n");
    printf("*                                              *\n");
    printf("*       作者：Mculover666  计算机与信息学院        *\n");
    printf("*                                              *\n");
    printf("************************************************\n");

    /* 创建Socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        printf("[UDP Client]客户端Socket创建失败!\n");
        return -1;
    }
    printf("[UDP Client]客户端Socket创建成功!\n");

    /* 绑定ip和端口 */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));//指定端口号

    /* 定时 每 10s一次向服务器发送当前采集的温湿度数据 */
    while(1)
    {

        //随机生成温湿度数据
        temp = (float)((rand()%500)/10.0);
        humi = (float)((rand()%700)/10.0);

        /* 获取当前时间 */
        time(&cur_time);
        p = gmtime(&cur_time);
	    len = sizeof(server_addr);

        /* 发送数据给服务端 */
        sprintf(recv_buf, "{\"id\":\"%10s\",\"temp\":%2.1f,\"humi\":%2.1f,\"motor\":\"%3s\",\"sw\":\"%3s\"}", ENDPOINT_ID, temp,humi,motor, sw);
        sendto(sock_fd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)(&server_addr),len);
        printf("[UDP Client][%02d:%02d:%02d]发送数据:%s\n",p->tm_hour+8, p->tm_min, p->tm_sec, recv_buf);
        
        
        /* 接收UDP服务端返回的数据 */
        len = sizeof(server_addr);
        nbytes = recvfrom(sock_fd, recv_buf, 100, 0, (struct sockaddr *)&server_addr, &len);
        recv_buf[nbytes] = '\0';

        /* 判断是否需要改变电机状态 */
        if(strstr(recv_buf, "motoron"))
        {
            printf("[System]电机已开启!\n");
            strcpy(motor, "on");
        }
        else if(strstr(recv_buf, "motoroff"))
        {
            printf("[System]电机已关闭!\n");
            strcpy(motor, "off");
        }
        else if(strstr(recv_buf, "switchon"))
        {
            printf("水阀已开启!\n");
            strcpy(sw, "on");
        }
        else if(strstr(recv_buf, "switchoff"))
        {
            printf("水阀已关闭!\n");
            strcpy(sw, "off");
        }
        else
        {
            sprintf(udp_buf, "normal");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统正常运行!\n");
        }

        /* 延时5s */
        sleep(5);
    }

    return 0;
}