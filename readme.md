本项目中更改的文件都放在了文件夹experiment中，实验思路如下

我们设计了三个阶梯性地实验
实验一：模拟DOS攻击，比较攻击前后对网络性能的影响
评价指标：吞吐量的变化以及拥塞窗口的变化
对标方法：对比分析法
实验场景：NS3仿真平台
> 更改文件：bulk-send-application.cc
>          bulk-send-application.h
> 函数的definition is in line 251~256. The calling place is in line 157~158
> 上面两个文件代替src/internet/model目录下的相应文件


实验二：探究不同因素（攻击周期，脉冲时长，脉冲强度）对攻击速率的影响
评价指标：吞吐量的变化
对标方法：控制变量法
> 本实验需要多次更改变量，所以文件夹中的脚本文件是涌来完成本实验

实验三：随机化minRTO防御低速DOS攻击
评价指标：吞吐量的变化
对标方法：对比分析法
实验场景：NS3仿真平台
> 更改文件：tcp-socket-base.cc
>          tcp-socket-base.h
> 函数的 The calling place is in line 830左右
> 上面两个文件代替src/internet/model目录下的相应文件

实验四：基于RED算法的路由器防御低速DOS攻击
评价指标：吞吐量的变化
对标方法：对比分析法
实验场景：NS3仿真平台
> 待更新