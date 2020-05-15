/**
 * @brief   智慧农业采集控制系统用户端
 * @author  Mculover666
 * @date    2020/05/10
*/
 
#include <stdio.h>
#include <mysql.h>
#include <my_global.h>
#include <string.h>

#define DATABASE    "ia_system"
#define TABLE       "value"

void finish_with_error(MYSQL *con);
int get_value(MYSQL *con, const char* table, const char* field, const char* id, char* buf);
int set_value(MYSQL *con, const char* table, const char* field, const char* id, char* buf);

int main()
{
    int menu;
    MYSQL *con = NULL;
    char id[20] = {0};
    char buf[20] = {0};
    char sql[100];

    float value = 0.0;

    /* 打印启动界面 */
    printf("************************************************\n");
    printf("*                                              *\n");
    printf("*       欢迎使用智慧农业采集控制系统!(用户端)     *\n");
    printf("*                                              *\n");
    printf("*                                              *\n");
    printf("*       作者：Mculover666  计算机与信息学院        *\n");
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

    /* 连接数据库 */
    if(NULL == mysql_real_connect(con,"117.50.111.72","mculover666","mculover666",DATABASE,3306,NULL,0))
    {
        printf("MySQL connect fail.\n");
        fprintf(stderr,"%s\n",mysql_error(con));
        mysql_close(con);
        return -1;
    }

    printf("远程数据库[%s]登录成功!\n", DATABASE);
    //打印菜单提供选择
    printf("请输入终端号:");
    //接收用户输入
    scanf("%s", id);
    printf("请选择需要进行的操作： 1)查询; 2）更新\n");

    //接收用户输入
    scanf("%d", &menu);
    switch(menu)
    {
        case 1:
            /* 查询功能 */
            /* 查询终端最大温度信息并打印结果 */
            if(-1 != get_value(con, TABLE, "temp_max", id, buf))
            {
                printf("--->终端%s信息查询成功，当前最大温度为:%s\n", id, buf);
            }
            else
            {
                printf("***>终端%s信息查询失败!\n", id);
                return -1;
            }
            /* 查询终端最大湿度信息并打印结果 */
            if(-1 != get_value(con, TABLE, "temp_min", id, buf))
            {
                printf("--->终端%s信息查询成功，当前最大湿度为:%s\n", id, buf);
            }
            else
            {
                printf("***>终端%s信息查询失败!\n", id);
                return -1;
            }
            /* 查询终端最小温度信息并打印结果 */
            if(-1 != get_value(con, TABLE, "temp_min", id, buf))
            {
                printf("--->终端%s信息查询成功，当前最小温度为:%s\n", id, buf);
            }
            else
            {
                printf("***>终端%s信息查询失败!\n", id);
                return -1;
            }
            /* 查询终端最小湿度信息并打印结果 */
            if(-1 != get_value(con, TABLE, "humi_min", id, buf))
            {
                printf("--->终端%s信息查询成功，当前最小湿度为:%s\n", id, buf);
            }
            else
            {
                printf("***>终端%s信息查询失败!\n", id);
                return -1;
            }
            break;
        case 2:
            /* 更新功能 */
            //打印二级菜单进行选择
            printf("请选择需要更新的阈值设定信息:\n");
            printf("--->1)最大温度; 2)最大湿度; 3)最小温度; 4)最小湿度\n");
            //接收用户输入
            scanf("%d", &menu);
            switch(menu)
            {
                case 1:
                    //接收用户输入的温度值
                    printf("请输入新的最大温度阈值:");
                    scanf("%f", &value);
                    sprintf(buf, "%2.1f", value);
                    if(-1 != set_value(con, TABLE, "temp_max", id, buf))
                    {
                        printf("--->终端%s最大温度阈值修改成功!\n", id);
                    }
                    else
                    {
                        printf("***>终端%s最大温度阈值修改失败!\n", id);
                        return -1;
                    }
                    //执行完之后再次查询
                    if(-1 != get_value(con, TABLE, "temp_max", id, buf))
                    {
                        printf("--->终端%s最大温度阈值查询成功，为:%s\n", id, buf);
                    }
                    else
                    {
                        printf("***>终端%s最大温度阈值查询失败!\n", id);
                        return -1;
                    }
                    break;
                case 2:
                    //接收用户输入的最大湿度
                    printf("请输入新的最大湿度阈值:");
                    scanf("%f", &value);
                    sprintf(buf, "%2.1f", value);
                    if(-1 != set_value(con, TABLE, "humi_max", id, buf))
                    {
                        printf("--->终端%s最大湿度阈值修改成功!\n", id);
                    }
                    else
                    {
                        printf("***>终端%s最大湿度阈值修改失败!\n", id);
                        return -1;
                    }
                    //执行完之后再次查询
                    if(-1 != get_value(con, TABLE, "humi_max", id, buf))
                    {
                        printf("--->终端%s最大湿度阈值查询成功，为:%s\n", id, buf);
                    }
                    else
                    {
                        printf("***>终端%s最大湿度阈值查询失败!\n", id);
                        return -1;
                    }
                    break;
                case 3:
                    //接收用户输入的最小温度
                    printf("请输入新的最小温度阈值:");
                    scanf("%f", &value);
                    sprintf(buf, "%2.1f", value);
                    if(-1 != set_value(con, TABLE, "temp_min", id, buf))
                    {
                        printf("--->终端%s最小温度阈值修改成功!\n", id);
                    }
                    else
                    {
                        printf("***>终端%s最小温度阈值修改失败!\n", id);
                        return -1;
                    }
                    //执行完之后再次查询
                    if(-1 != get_value(con, TABLE, "temp_min", id, buf))
                    {
                        printf("--->终端%s最小温度阈值查询成功，为:%s\n", id, buf);
                    }
                    else
                    {
                        printf("***>终端%s最小温度阈值查询失败!\n", id);
                        return -1;
                    }
                    break;
                case 4:
                    //接收用户输入的最小湿度
                    printf("请输入新的最小湿度阈值:");
                    scanf("%f", &value);
                    sprintf(buf, "%2.1f", value);
                    if(-1 != set_value(con, TABLE, "humi_min", id, buf))
                    {
                        printf("--->终端%s最小湿度阈值修改成功!\n", id);
                    }
                    else
                    {
                        printf("***>终端%s最小湿度阈值修改失败!\n", id);
                        return -1;
                    }
                    //执行完之后再次查询
                    if(-1 != get_value(con, TABLE, "humi_min", id, buf))
                    {
                        printf("--->终端%s最小湿度阈值查询成功，为:%s\n", id, buf);
                    }
                    else
                    {
                        printf("***>终端%s最小湿度阈值查询失败!\n", id);
                        return -1;
                    }
                    break;
                default:
                    /* 输入错误 */
                    printf("选项错误，系统推出!\n");
                    mysql_close(con);
                    return -1;
            }
            break;
        default:
            /* 输入错误 */
            printf("选项错误，系统退出!\n");
            mysql_close(con);
            return 0; 
    }
    mysql_close(con);
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
    sprintf(sql, "select %s from %s where id=%10s;", field, table, id);
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
 * 从数据库中更新用户信息
 * @param con 成功连接的MySQL变量
 * @param table 需要更新的表
 * @param id 用户id
 * @param buf 存放待更新数据的缓冲区
 * @return 成功返回0，失败则返回-1
*/
int set_value(MYSQL *con, const char* table, const char* field, const char* id, char* buf)
{
    char sql[100]; 
    
    //构造完整sql语句
    sprintf(sql, "update %s set %s=%s where id=%11s;", table, field, buf, id);
    //执行
    if(mysql_query(con,sql))
    {
        finish_with_error(con);
        return -1;
    }

    return 0;
}