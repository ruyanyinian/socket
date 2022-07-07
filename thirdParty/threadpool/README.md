# 需求
1. 怎么能够ThreadPool.c文件调用TaskQueue.cpp里面的问题接口
   1. （尝试）解决方案是先把.cpp生成库, 然后threadpool里面再去调用即可
   2. 这样我们是不是应该写一个C的接口