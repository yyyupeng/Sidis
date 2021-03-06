## 介绍
- Sidis是一个采用c++编写、基于C/S架构的小型K-V缓存数据库，支持String、List、Hash、Set、Sorted-Set数据类型操作，提供类似Redis的用户操作界面。
## 运行环境
- 系统：ubuntu19.10
- 处理器：Intel® Core™ i5-7200U CPU @ 2.50GHz × 4
- 内存：7.7 GiB
- 编译器：gcc version 9.2.1
## 特性
- 基于单线程的Reactor模型
- 使用非阻塞IO + epoll(LT)的多路复用技术
- 实现用户态Buffer，提高服务器反应速度及其可用性
- 使用哈希表、链表、红黑树、自写跳跃表作为对象的底层数据结构
- 使用GCC内置扩展的内存分配器，减少malloc的使用
- 实现LRU算法对内存进行管理
- 使用智能指针、析构函数来实现RAII机制，管理对象生命周期、避免内存泄漏
- 使用bind + function实现回调函数，基于事件驱动
- 实现分库管理机制，默认设定16个库（0~15）
- 参考Redis，简单实现RDB持久化
- 使用惰性删除策略来淘汰键值
## 目录说明
```
|—— client 客户端目录
|—— image 测试截图
|—— server 服务端目录
	|—— base 基础支持相关
	|—— db 数据库设计相关
	|—— net 网络相关
|—— test 测试代码
```
## 安装及运行
运行前请先安装CMake（2.8及以上版本）
```
cd server
cmake .
make
./Server
```
## 测试
- 基础功能

![image --basic](https://raw.githubusercontent.com/yyyupeng/Sidis/main/image/Basic_functions.png)
- 性能测试

![image --performance](https://raw.githubusercontent.com/yyyupeng/Sidis/main/image/performance.png)

