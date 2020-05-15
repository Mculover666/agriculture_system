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

# 3. 运行服务端

服务端源码在server文件夹，需要在云服务器上运行，使用gcc编译。

进入到server文件夹，执行make命令编译：

```bash
cd server
make
```

![](https://img-blog.csdnimg.cn/20200515150850878.png)

然后运行程序（默认监听8002端口，使用UDP协议，如果有安全组需要放行该端口）:
```bash
./server
```

# 4. 运行客户端

客户端运行在桌面Linux上，源码在client文件夹下面，进入之后使用make命令编译：

```bash
cd client
make
```

![](https://img-blog.csdnimg.cn/20200515152140854.png)

编译之后运行程序：

```bash
./client
```

## 4.1. 查询当前阈值

![](https://img-blog.csdnimg.cn/20200515152309512.png)

## 4.2. 修改阈值

修改最大湿度值为50.5：
![](https://img-blog.csdnimg.cn/20200515152425159.png)

# 5. 运行终端

终端中只使用到了UDP Socket编程，所以可以编译为桌面Linux的程序，也可以编译为ARM开发板上的程序。

## 5.1. 在桌面Linux上测试

进入endpoint文件夹，编译：
```bash
cd endpoint
make endpoint_PC
```
![](https://img-blog.csdnimg.cn/20200515153340362.png)

运行程序，第一个参数是云服务器ip，第二个参数是云服务器端口：

![](https://img-blog.csdnimg.cn/20200515154625401.png)

可以看到终端每隔5s向服务器上报一次数据，在服务端也可以看到上报的数据：

![](https://img-blog.csdnimg.cn/20200515154706907.png)

在MySQL中查看历史记录：

![](https://img-blog.csdnimg.cn/20200515154905673.png)

## 5.2. 在ARM开发板测试（使用Qemu模拟）

编译：
```bash
arm-linux-gnueabi-gcc endpoint.c -o endpoint.o
```

![](https://img-blog.csdnimg.cn/20200515155523640.png)

使用NFS复制可执行程序到开发板中：

![](https://img-blog.csdnimg.cn/20200515160047582.png)

运行程序：

![](https://img-blog.csdnimg.cn/20200515160630915.png)

在服务器端查看数据：

![](https://img-blog.csdnimg.cn/20200515160655807.png)

接收更多精彩文章及资源推送，欢迎订阅我的微信公众号：『mculover666』。

![](https://img-blog.csdnimg.cn/20200515160814838.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01jdWxvdmVyNjY2,size_16,color_FFFFFF,t_70)
