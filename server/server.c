/**
 * @brief   智慧农业采集控制系统
 * @author  Mculover666
 * @date    2020/05/10
 * @note    本系统使用开源库cjson解析数据
*/
 
#include <stdio.h>
#include <mysql.h>
#include <my_global.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "cJSON.h"

#define DATABASE        "ia_system"
#define TABLE_HISTORY   "history"
#define TABLE_VALUE     "value"
#define PORT            8002
#define VER             "1.0"

int parse_json_recv(const char* buf, char* id, float* temp, float *humi, char* motor, char* sw);
void finish_with_error(MYSQL *con);
int get_value(MYSQL *con, const char* table, const char* field, const char* id, char* buf);
int insert_value(MYSQL *con, const char* table, char* id, float temp, float humi, char* motor, char* sw);

int main()
{
    MYSQL *con = NULL;
    char id[20] = {0};
    char buf[20] = {0};
    char sql[100];

    /* 存放终端信息 */
    float temp = 0.0;
    float humi = 0.0;
    char motor[10]={0};
    char sw[10]={0};

    /* 存放阈值 */
    float temp_max, temp_min;
    float humi_max, humi_min; 

    /* UDP scoket */
    int server_sock_fd;
    struct sockaddr_in server_addr, client_addr;
    char udp_buf[100];
    int nbytes = 0;
    socklen_t len = 0;

    /* 打印菜单 */
    printf("************************************************\n");
    printf("*                                              *\n");
    printf("*             智慧农业信息采集控制系统          *\n");
    printf("*                                              *\n");
    printf("*                                              *\n");
    printf("*       作者：wangshiwei  UDP监听端口:%-4d     *\n", PORT);
    printf("*       数据库：MySQL      版本：v%3s          *\n", VER);
    printf("*                                              *\n");
    printf("************************************************\n");


    /* 初始化MYSQL变量并连接数据库 */
    con = mysql_init(NULL);
    if(con == NULL)
    {
        printf("MySQL init fail.\n");
        fprintf(stderr,"%s\n",mysql_error(con));
        return -1;
    }

    /* 连接数据库history */
    if(NULL == mysql_real_connect(con,"117.50.111.72","mculover666","mculover666",DATABASE,3306,NULL,0))
    {
        printf("MySQL connect fail.\n");
        fprintf(stderr,"%s\n",mysql_error(con));
        mysql_close(con);
        return -1;
    }
    printf("[System]成功连接到数据库：%s!\n", DATABASE);

    /* 创建Server Socket */
    server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock_fd < 0)
    {
        printf("[System]服务端Socket创建失败\n");
        return -1;
    }

    /* 绑定ip和端口 */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);//指定端口号
    bind(server_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    printf("[System]系统已启动监听端口%d!\n", PORT);

    while(1)
    {
        printf("************************************************\n");
        /* 
         * 接收UDP客户端的数据
         * 数据为JSON格式：{"id":"2016211961","temp":12.3,"humi":33.3,"motor":"on","sw":"off"} 
        */
        len = sizeof(client_addr);
        nbytes = recvfrom(server_sock_fd, udp_buf, 100, 0, (struct sockaddr *)&client_addr, &len);
        udp_buf[nbytes] = '\0';
        printf("[UDP Server]接收到来自数据终端(%s)的消息:%s\n", inet_ntoa(client_addr.sin_addr), udp_buf);

        //使用CJSON解析提取终端上报数据
        if(-1 == parse_json_recv(udp_buf, id, &temp, &humi, motor, sw))
        {
            printf("[CJSON]解析终端数据失败!\n");
            return -1;
        }

        /* 插入历史记录 */
        if(-1 != insert_value(con, TABLE_HISTORY, id, temp, humi, motor, sw))
        {
            printf("[MySQL]终端[%10s]插入一条记录！\n", id);
        }
        else
        {
            printf("[MySQL]终端[%10s]插入记录失败！\n", id);
            return -1;
        }

        /* 查询此终端当前阈值 */
        //查询当前温度最大阈值
        if(-1 == get_value(con, TABLE_VALUE, "temp_max", id, buf))
        {
            
            printf("[MySQL][终端%s]最大温度阈值查询失败!\n", id);
            return -1;
        }
        temp_max = atof(buf);
        //查询当前温度最小阈值
        if(-1 == get_value(con, TABLE_VALUE, "temp_min", id, buf))
        {
            printf("[MySQL][终端%s]最小温度阈值查询失败!\n", id);
            return -1;
        }
        temp_min = atof(buf);
        //查询当前湿度最大阈值
        if(-1 == get_value(con, TABLE_VALUE, "humi_max", id, buf))
        {
            printf("[MySQL][终端%s]最大湿度阈值查询失败!\n", id);
            return -1;
        }
        humi_max = atof(buf);
        //查询当前湿度最小阈值
        if(-1 == get_value(con, TABLE_VALUE, "humi_min", id, buf))
        {
            printf("[MySQL][终端%s]最小温度阈值查询失败!\n", id);
            return -1;
        }
        humi_min = atof(buf);

        printf("[MySQL]当前终端[%10s]的设定阈值为：temp_max=%2.1f,temp_min=%2.1f,humi_max=%2.1f,humi_min=%2.1f\n",id,temp_max,temp_min,humi_max,humi_min);
    
        /* 报警判断 */
        if(temp > temp_max)
        {
            sprintf(udp_buf, "motoron");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统已经向控制终端发送电机开启指令!\n");
        }
        else if(temp < temp_min)
        {
            sprintf(udp_buf, "motoroff");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统已经向控制终端发送电机关闭指令!\n");
        }
        if(humi > humi_max)
        {
            sprintf(udp_buf, "switchon");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统已经向控制终端发送水阀关闭指令!\n");
        }
        else if(humi < humi_min)
        {
            sprintf(udp_buf, "switchoff");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统已经向控制终端发送水阀开启指令!\n");
        }
        else
        {
            sprintf(udp_buf, "normal");
            sendto(server_sock_fd,udp_buf,strlen(udp_buf),0,(struct sockaddr *)&client_addr,len);
            printf("[UDP Server]系统正常运行!\n");
        }
    
    }
    mysql_close(con);
    return 0; 
}
/** 
 * 使用cJSON从接收到数据中提取终端ID、温度、湿度、电机状态、开关状态
 * @param buf 需要处理的数据(json格式)
 * @param id 存放提取出的用户ID
 * @param 
 * @return 解析成功返回0，解析失败返回-1
*/
int parse_json_recv(const char* buf, char* id, float* temp, float *humi, char* motor, char* sw)
{
    cJSON* cjson_all = NULL;
    cJSON* cjson_id = NULL;
    cJSON* cjson_temp = NULL;
    cJSON* cjson_humi = NULL;
    cJSON* cjson_motor = NULL;
    cJSON* cjson_switch = NULL;
    
    /* 解析整段JSON数据 */
    cjson_all = cJSON_Parse(buf);
    if(cjson_all == NULL)
    {
        return -1;
    }

    /* 根据键值提取终端ID */
    cjson_id = cJSON_GetObjectItem(cjson_all, "id");
    strcpy(id, cjson_id->valuestring);

    /* 根据键值提取终端温度 */
    cjson_temp = cJSON_GetObjectItem(cjson_all, "temp");
    *temp = (float)cjson_temp->valuedouble;

    /* 根据键值提取终端湿度 */
    cjson_humi = cJSON_GetObjectItem(cjson_all, "humi");
    *humi = (float)cjson_humi->valuedouble;

    /* 根据键值提取终端电机状态 */
    cjson_motor = cJSON_GetObjectItem(cjson_all, "motor");
    strcpy(motor, cjson_motor->valuestring);

    /* 根据键值提取终端开关状态 */
    cjson_switch = cJSON_GetObjectItem(cjson_all, "sw");
    strcpy(sw, cjson_switch->valuestring);

    /* 释放cjson占用内存 */
    cJSON_Delete(cjson_all);

    return 0;
}

/** 
 * 语句执行出错处理
 * @param con 需要处理的MYSQL变量
 * @return -1;
*/
void finish_with_error(MYSQL *con)
{
    fprintf(stderr,"%s\n",mysql_error(con));
    mysql_close(con);
}

/**
 * 从数据库中查询用户信息
 * @param con 成功连接的MySQL变量
 * @param table 需要查询的表
 * @param id 用户id
 * @param buf 存放查询结果的缓冲区
 * @return 成功返回0，失败则返回-1
*/
int get_value(MYSQL *con, const char* table, const char* field, const char* id, char* buf)
{
    char sql[100];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    //构造完整sql语句
    sprintf(sql, "select %s from %s where id=%11s;", field, table, id);
    //查询
    if(mysql_query(con,sql))
    {
        finish_with_error(con);
        return -1;
    }
    //获取并存储查询结果
    result = mysql_store_result(con);
    if(NULL == result)
    {
        finish_with_error(con);
        return -1;
    }
    //根据行数查询数据
    if(row = mysql_fetch_row(result))
    {
        if(row[0] != NULL)
        {
            strcpy(buf, row[0]);
        }
        else
        {
            strcpy(buf, "NULL");
        }
        //printf("用户%s的用电类型为：%s\n", id, );
    }
    mysql_free_result(result);

    return 0;
}
/**
 * 向数据库中插入信息
 * @param con 成功连接的MySQL变量
 * @param table 需要更新的表
 * @param id 用户id
 * @param buf 存放待更新数据的缓冲区
 * @return 成功返回0，失败则返回-1
*/
int insert_value(MYSQL *con, const char* table, char* id, float temp, float humi, char* motor, char* sw)
{
    char sql[100]; 
    
    //构造完整sql语句
    sprintf(sql, "insert into %s (id,temp,humi,motor,switch) values(%10s,%2.1f,%2.1f,\"%3s\",\"%3s\")", table, id, temp, humi, motor,sw);
    //执行
    if(mysql_query(con,sql))
    {
        finish_with_error(con);
        return -1;
    }

    return 0;
}
