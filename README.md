# 1.系统设计与实现
本设计的是“智慧农业信息采集控制系统”，系统主要实现的功能有：

（1）采集终端向服务器上报温度、湿度、电机状态（用于降低温度）、开关状态（用于自动浇水）；

（2）服务器接收到数据后提取信息，将数据及其上报时间写入数据库存储历史记录，便于用于查看；

（3）服务器接收到数据后从数据库中读取用户设定的阈值，对数据进行判断，如果超过或者低于阈值即发送对应指令，打开/关闭电机，或者打开/关闭开关；

（4）用户端程序可以修改报警阈值；

整个系统的实现架构如下图：

（1）农业采集终端运行在嵌入式Linux开发板上（使用Qemu模拟）；

（2）农业采集系统服务端使用云服务器，操作系统为Ubuntu18.04；

（3）数据存储使用MySQL服务器；

（4）农业采集用户端运行在桌面Linux上，操作系统为Ubuntu16.04；

![](https://img-blog.csdnimg.cn/20200514224654740.png)

其中，所有数据均采用JSON数据格式，使用UDP传输，数据库中有两张表，一张表为history用于保存历史数据，一张表为value用于保存阈值信息。

# 2. 建立数据库及数据表

① 建立数据库ia_system:

```sql
create database ia_system;
```

![](https://img-blog.csdnimg.cn/20200514224831380.png#pic_center)

② 建立数据表history:

```sql
create table history (id INT(10) not null,temp FLOAT,humi FLOAT,motor CHAR(3), switch CHAR(3));
```

![](https://img-blog.csdnimg.cn/20200514224843205.png#pic_center)

③ 建立数据表value:

```sql
create table value (id INT(10) not null,temp_max FLOAT,humi_max FLOAT,temp_min FLOAT,humi_min FLOAT)
```

![](https://img-blog.csdnimg.cn/20200514224854110.png#pic_center)

④ 提前在数据表value中设置一个阈值：

```sql
insert into value(id,temp_max,humi_max,temp_min,humi_min) values(2016211961,10.0,40.0,10.0,40.0);
```

![](https://img-blog.csdnimg.cn/20200514224907159.png#pic_center)

# 3. 
