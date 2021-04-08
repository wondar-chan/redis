Redis中文注解版 
-------------  
中文注释版的Redis源码，代码会与官方库保持同步，除了在代码中添加了中文注释方便英文不太好的同学查阅外，另外也计划出系列博客，帮助大家系统化学习Redis。目前已添加超数百行中文注释，涵盖了大部分Redis的主要功能，更多内容仍在继续添加中。  
关于更多更详细的Redis信息，请查阅Redis官网[redis.io](https://redis.io).

另外随本仓库，我开了一个[Redis源码剖析专栏](https://blog.csdn.net/xindoo/category_10068113.html)专栏。 

## 这个专栏是什么不是什么？ 
首先，这个专栏其实不是一个面向新手的redis教程、也不是什么配置使用教程，如果是初识redis的话，建议参考官方文档，国内也有好多博主写过相关的内容了。另外如果是想找redis的某个具体的配置项，强烈建议参考官方博客。总结起来就一句话，这个项目不是redis的入门读物、更不是redis使用和配置教程。      

那这个项目是什么呢？我的目标定位，**这个项目是资深程序猿阅读和理解redis源码最优质的中文材料**。虽然世面上已有黄建宏老师的redis中文注解版[https://github.com/hujianhong/redis-3.0-annotated](https://github.com/hujianhong/redis-3.0-annotated)，但他那个是redis3.0(现在都已经到6.0.x版了)，而且多年未更新过了。我这个项目预期做到 **源码+中文注释+系列博文** 三位一体，全方位帮大家深入理解redis的原理。另外github上我源码库不是redis的copy，而是直接fork出来的，所以会定期和redis当前源码保持一致，时更时新。    

这里放上相关链接，**欢迎关注** 
Redis中文注解版仓库：[https://github.com/xindoo/redis](https://github.com/xindoo/redis)    
Redis源码剖析专栏：[https://blog.csdn.net/xindoo/category_10068113.html](https://blog.csdn.net/xindoo/category_10068113.html)   

## 目前已完成的内容
[Redis源码剖析之内存淘汰策略(Evict)](https://xindoo.blog.csdn.net/article/details/114239967)  
[Redis源码剖析之数据过期(expire)](https://xindoo.blog.csdn.net/article/details/113078136)  
[Redis源码剖析之robj(redisObject)](https://xindoo.blog.csdn.net/article/details/112449822)  
[Redis源码剖析之字典(dict)](https://xindoo.blog.csdn.net/article/details/110716234)    
[Redis源码剖析之跳表(skiplist)](https://xindoo.blog.csdn.net/article/details/109922390)  
[Redis源码剖析之快速列表(quicklist)](https://xindoo.blog.csdn.net/article/details/109150975)  
[Redis源码剖析之压缩列表(ziplist)](https://xindoo.blog.csdn.net/article/details/108923557)  
[Redis源码剖析之SDS(Simple Dynamic String)](https://xindoo.blog.csdn.net/article/details/108808273)   
[Redis源码剖析之RDB](https://xindoo.blog.csdn.net/article/details/115287396)   
[Redis源码剖析之AOF](https://blog.csdn.net/xindoo/article/details/115447240)  
## 内容规划&更新计划  
目前暂定的内容规划如下，后期可能会但我自己进度适当增删调整内容，**每月更新2-4篇**。


### 基础
- [ ] 初识Redis 
- [ ] redis的日常使用  
- [ ] 搭建redis源码环境（单步调试）  
- [ ] 了解redis的启动过程 

### 各种数据结构
- [x] [SDS(simple dynamic string)](https://editor.csdn.net/md/?articleId=108698706)
- [x] [压缩列表ziplist](https://blog.csdn.net/xindoo/article/details/108923557)
- [x] [quicklist](https://xindoo.blog.csdn.net/article/details/109150975)
- [x] [skiplist](https://xindoo.blog.csdn.net/article/details/109922390) 
- [x] [dict](https://xindoo.blog.csdn.net/article/details/110716234)
- [x] robj   
- [ ] geo 
- [ ] 队列   
- [ ] radix树  


### 高级功能
- [ ] redis的集群模式 
- [ ] 发布订阅模式实现 
- [ ] redis6.0客户端缓存 
- [ ] redis stream和消息队列 
- [ ] redis的模块   
- [ ] lua相关  

### 架构 
- [ ] Redis IO模型（单线程如何那么快）    
- [ ] redis6.0的多线程  
- [ ] redis的后台线程   
- [ ] 单机和集群模式 
- [ ] 集群数据同步   
- [ ] 高可用之sentinel  
- [ ] redis后台线程   
- [ ] 持久化存储(aof&rdb)  
- [ ] RESP协议  

### 其他
- [x] 数据数据淘汰之LRU和LFU  
- [x] redis渐进式hash详解  
- [x] redis数据过期(惰性删除)  
- [ ] 分布式锁 redlock  
- [ ] Redis基于引用计数的垃圾回收 
- [ ] redis使用注意事项  
- [ ] redis常用配置  