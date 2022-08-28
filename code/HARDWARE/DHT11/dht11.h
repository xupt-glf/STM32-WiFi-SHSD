#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   

 
//IO��������
void DHT11_IO_IN(void);
void DHT11_IO_OUT(void);
////IO��������											   
#define	DHT11_DQ_OUT PBout(15) //���ݶ˿�	
#define	DHT11_DQ_IN  PBin(15)  //���ݶ˿�	 


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















