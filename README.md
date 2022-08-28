# 基于STM32-WIFI智能家居系统
##  1. 智能家居系统
stm32 单片机为核心，设计一套基于wifi网络的智能家居控制系统。该系统以wifi通信为基础、能通过无线通信技术实时监控家居的温湿度状态，光照强度，烟雾。并能自由控制家居的电器的开关操作。并根据室内的温度做出降低室温、升高室温的操作。

系统分为自动控制和手动控制两种模式，手动模型需要通过智能家居系统的操作按键或者手机APP实现对各种电器的打开或关闭。当系统处于自动控制时可根据环境变化自动对相应的电器进行打开或关闭。具体如下：

1.温度超过35摄氏度 驱动继电器启动风扇 低于30摄氏度  停止风扇
2.温度低于15度  点亮一盏led（绿灯）模拟升高室温 高于15度点灭
3.烟雾浓度高于一定的值 蜂鸣器持续响持续闪烁led灯（红色） 并发送报警消息到app 低于一定值 一定时间后取消报警  
4.光照强度超过一定值，开启遮光板（模拟窗帘）。低于一定值关闭遮光板点亮led灯（黄色）

##  2. 硬件电路
### 2.1 STM32最小系统

STM32F103芯片的最小系统电路主要由复位电路，时钟电路以及辅助电源电路组成。

![Image1_STM32](image/Image1_STM32.png)

### 2.2 ESP8266WIFI模块

ESP8266是硬件系统电路与软件手机APP通信的WIFI无线模块，他能够将硬件系统检测的温室环境参数如温湿度，光照强度，烟雾浓度等信息通过无线WIFI的形式发送到手机APP中

### 2.3 步进电机驱动电路

电动机转动是需要有十几毫安的电流驱动，而单片机的IO引脚没有这么大的电流，无法驱动电动机转动，因此需要对IO口电流进行放大。我们采用了ULN2003电机驱动芯片来对电机进行驱动，ULN2003是一款高耐压，大电流的晶体管阵列，单个引脚最大输出电流500MA电流[11]，最高反向电压可达50V，输入控制电压在2.8V-24V。在ULN2003芯片的输入端接单片机的IO口，输出端接电机控制线，可以实现单片机对电机的控制。

![Image2_ULN](image/Image2_ULN.png)

### 2.4 液晶显示模块

智能温室控制系统显示部分采用的是1602液晶显示，1602液晶有叫字符型液晶是工业上比较常用的一种液晶显示模块，因为该显示模块只能显示数字、字母和符号。

### 2.5 传感器模块

传感器主要使用了温湿度DHT11传感器、烟雾浓度传感器。DHT11温湿度传感器是比较常用的一款温湿度传感器，通过将DHT11的IO口与单片机中的IO口相连接，通过按照一定的协议发送传感器的指令，就可以控制传感器采集温湿度并返回给单片机。烟雾传感器采用MQ -2气体传感器，MQ-2气体传感器对可燃性气体例如液化气，丙烷，氢气灵敏度高，同时也可以检测其他可燃性气体

## 3. 程序设计

### 3.1 主程序设计

主程序设计是微处理最先开始执行的程序，微处理按照主程序的顺序执行程序，在主程序中包含了其他模块程序，我们按照系统的功能逻辑进行程序设计，能够实现说有预设的功能。

### 3.2 温湿度程序设计

温湿度检测程序是通过微处理器给温湿度传感器发送指令，控制温湿度传感器进行温度检测，检测完毕后，温湿度传感器会给微处理器发送检测完毕的信号，此时单片机就可以发送接收温度信息的指令，当传感器收到接收温度的指令时，就会发送40位数据，分别是温度信息和湿度信息，此时微处理就获得了环境中温湿度值

## 4. 电路调试
液晶显示的调试，在硬件调试的基础上，观察到液晶的背光已经点亮，说明液晶的供电正常，旋转液晶背光对比度调节的电位器，可以观察到液晶对比度在变化说明此电路没有问题。

步进电机控制的调试，步进电机在调试过程中遇到的问题是当程序运行打开或者关闭窗户的时候步进电机只抖动而不转动，判断有可能是电路连接有问题，也有可能是程序留给步进电机反应的时间太短的，电机来不及转动程序就执行过去了，按照这两点思路，我首先是增大了步进电机节拍间隔时间，

## 5. 作品展示

原理图：

![Image4_SCH](image/Image4_SCH.jpg)

PCB图：

![Image5_PCB](image/Image5_PCB.jpg)

APP控制界面：

![Image6_APP](image/Image6_APP.jpg)

实物图：

![Image3_all](image/Image3_all.jpg)
