# "一生一芯"工程项目

这是"一生一芯"的工程项目. 通过运行
```bash
bash init.sh subproject-name
```
进行初始化, 具体请参考[实验讲义][lecture note].

[lecture note]: https://docs.ysyx.org/schedule.html

框架代码 ：
- Navy-apps                 # 应用程序集 
- am-kernels                # 基于(AM)开发的应用程序 
- abstract-machine(AM)      # 抽象计算机(AM)
- nemu                      # 全系统模拟器 NEMU
- NPC                       # new process cpu
- init.sh                   # 初始化脚本
- Makefile                  # 用于工程打包提交




![AM FrameWork](./Slides%26Image/image/abstract-machine-framwork.jpg)

# Abstract machine 
AM(Abstract Machine) - 计算机抽象模型
AM = TRM + IOE + CTE + VME + MPE 
- TRM(TuRing Machine) - 图灵机 
- IOE(I/O Extension) - 输入输出扩展 
- CTE(ConText Extension) – 上下文扩展 (一生一芯只要求做到这里) 
- VME(Virtual Memory Extension) - 保护扩展 
- MPE(Multi-Processor Extension) - 多处理器扩展

# test
