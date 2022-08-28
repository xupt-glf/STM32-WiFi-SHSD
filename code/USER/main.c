#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "wifi.h"
#include "dht11.h"
#include "lcd1602.h"
#include "adc.h"
#include "timer.h"

u8 bbuf[16]="WIFI ERROE!     ";
char bbbuf[12]="SMOG HIGH!!!";
u8 buf[6]="Auto  ";
u8 buf1[6]="Manual";
u8 humi_value,temp_value;
u8 flag,flag1;
u16 smog_value,beam_value;

//�������ݴ���
//0-3:�¶�ֵ
//4-7:����ֵ
//8-11������ֵ
char value_buf[14];  


//��ת
void F_Rotation(u16 x)
{
	u8 j;
	for(;x>0;x--)
	{
		switch(j)
		{
			case 0:motor1 = 1;motor2 = 0;motor3 = 0;motor4 = 0;break;
			case 1:motor1 = 0;motor2 = 1;motor3 = 0;motor4 = 0;break;
			case 2:motor1 = 0;motor2 = 0;motor3 = 1;motor4 = 0;break;
			case 3:motor1 = 0;motor2 = 0;motor3 = 0;motor4 = 1;break;
		}
		j++;
		if(j>=4)   j=0;
		delay_ms(2);
	}
	
}

//��ת
void B_Rotation(u16 x)
{
	u8 j;
	for(;x>0;x--)
	{
		switch(j)
		{
			case 0:motor1 = 0;motor2 = 0;motor3 = 0;motor4 = 1;break;
			case 1:motor1 = 0;motor2 = 0;motor3 = 1;motor4 = 0;break;
			case 2:motor1 = 0;motor2 = 1;motor3 = 0;motor4 = 0;break;
			case 3:motor1 = 1;motor2 = 0;motor3 = 0;motor4 = 0;break;
		}
		j++;
		if(j>=4)   j=0;
		delay_ms(2);
	}	
}

//���ݴ�����ʾ���ж�
void Data_Deal(void)
{
	DHT11_Read_Data(&temp_value,&humi_value);  //��ȡ��ʪ��ֵ
	smog_value = Get_Adc_Average(ADC_Channel_9,20);  //��ȡ����Ũ��ֵ
	beam_value = Get_Adc_Average(ADC_Channel_8,20);  //��ȡ����ֵ
	
	//LCD��ʾ����
	//��ʾ�¶�
	value_buf[0] = 0x30 + temp_value / 1000;
	value_buf[1] = 0x30 + temp_value % 1000 / 100;
	value_buf[2] = 0x30 + temp_value % 100 / 10;
	value_buf[3] = 0x30 + temp_value % 10;
	LCD1602_Write_Cmd(0x80);
	LCD1602_Write_Dat('T');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[0]);
	LCD1602_Write_Dat(value_buf[1]);
	LCD1602_Write_Dat(value_buf[2]);
	LCD1602_Write_Dat(value_buf[3]);
	
	
	value_buf[4] = 0x30 + smog_value / 1000;
	value_buf[5] = 0x30 + smog_value % 1000 / 100;
	value_buf[6] = 0x30 + smog_value % 100 / 10;
	value_buf[7] = 0x30 + smog_value % 10;
	//��ʾ����Ũ��
	LCD1602_Write_Cmd(0x87);
	LCD1602_Write_Dat('S');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[4]);
	LCD1602_Write_Dat(value_buf[5]);
	LCD1602_Write_Dat(value_buf[6]);
	LCD1602_Write_Dat(value_buf[7]);
	
	value_buf[8] = 0x30 + beam_value / 1000;
	value_buf[9] = 0x30 + beam_value % 1000 / 100;
	value_buf[10] = 0x30 + beam_value % 100 / 10;
	value_buf[11] = 0x30 + beam_value % 10;
	//��ʾ����ǿ��
	LCD1602_Write_Cmd(0x80 + 0x40);
	LCD1602_Write_Dat('B');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[8]);
	LCD1602_Write_Dat(value_buf[9]);
	LCD1602_Write_Dat(value_buf[10]);
	LCD1602_Write_Dat(value_buf[11]);
	
	value_buf[12] = 0x30 + humi_value / 10;
	value_buf[13] = 0x30 + humi_value % 10;
	LCD1602_Write_Cmd(0x80 + 0x47);
	LCD1602_Write_Dat('H');
	LCD1602_Write_Dat(':');
	LCD1602_Write_Dat(value_buf[12]);
	LCD1602_Write_Dat(value_buf[13]);
	LCD1602_Write_Dat('%');
	
	
	//�¶�ֵ�жϱȽ�
	if(temp_value > 35 && flag1)   
	{
		Relay = 0;//�̵�������������ת��
	}
	else if((temp_value < 30 && temp_value >= 15) && flag1) 
	{
		Relay = 1;//�̵����رգ�����ֹͣת��
	}
	else if(temp_value < 15 && flag1) 
	{
		LED_GREEN = 0;//����LED����ɫ����ģ����������
	}
	
	//����ֵ�жϱȽ�
	if(smog_value > 1500)   
	{
		LED_RED = ~LED_RED;//�����������������˸led�ƣ���ɫ�� �����ͱ�����Ϣ��app ����һ��ֵ һ��ʱ���ȡ������
		Beep = ~Beep;
		ESP8266_send_data(bbbuf);
		delay_ms(100);
	}
	else
	{
		LED_RED = 1;   //LED���������ر�
		Beep = 1;
	}
	
	if(humi_value)   //ʪ�ȳ���
	{
	
	}
	else
	{
		
	}
	
	
	//����ֵ�ж�
	if((beam_value > 1500 && flag == 1) && flag1)  
	{
		F_Rotation(2000);//�����ڹ�壨ģ�ⴰ������
		LED_YELLOW = 1;
		flag = 2;
	}
	else if((beam_value <= 1500 && flag == 2) && flag1)
	{
		B_Rotation(2000);  //����һ��ֵ�ر��ڹ�����led�ƣ���ɫ��
		LED_YELLOW = 0;
		flag = 1;
	}
	
}

//��������APP�Ŀ���ָ��
void Receive_data(void)
{
	if(Find("XXXXXX") == 1)
	{
		F_Rotation(2000);
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("YYYYYY") == 1)
	{
		B_Rotation(2000);
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("WWWWWW") == 1)
	{
		Relay = 0;//�̵�������������ת��
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("NNNNNN") == 1)
	{
		Relay = 1;//�̵�������������ת��
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("KKKKKK") == 1)
	{
		LED_YELLOW = 0;//led��
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("GGGGGG") == 1)
	{
		LED_YELLOW = 1;//LED��
		flag1 = 0;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('M');
	}
	
	if(Find("ZZZZZZ") == 1)  //�����Զ�����
	{
		flag1 = 1;
		LCD1602_Write_Cmd(0x80 + 0x4f);	
		LCD1602_Write_Dat('A');
	}
}

//ϵͳӲ����ʼ��
void Sys_Init(void)
{
	u8 i;
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	USART3_init(115200);
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�
	Relay_Beep_Init();   //�̵�������������ʼ��
	Motor_Init();       //�����ʼ��
	DHT11_Init();    //��ʪ�ȴ�������ʼ��
	LCD1602_Init();	  //LCD1602��ʼ��
	Adc_Init();     //ADֵ��ʼ�������ڲ�������Ũ�Ⱥ͹���ǿ��

	while(ESP8266_wifi_Init())    //���WIFIģ���Ƿ����
	{
		LCD1602_Write_Cmd(0x01);//����
		
		//��ʾ���Ӵ������ģ������������
		LCD1602_Write_Cmd(0x80);
		for(i=0;i<16;i++)
		{
			LCD1602_Write_Dat(bbuf[i]);
		}
	}
	
	//�ϵ��ֶ����ƣ�APP����
	LCD1602_Write_Cmd(0x80 + 0x4f);
	LCD1602_Write_Dat('M');
	flag = 1;
}


//������
 int main(void)
 {	
	Sys_Init();
	delay_ms(1000);
	Data_Deal();
	if(beam_value > 1500)  
	{
		flag = 1;
	}
	else if(beam_value <= 1500)
	{
		flag = 2;
	}
	 
	while(1)
	{
		Data_Deal();  //���ݴ���
		Receive_data();  //��������APP�Ŀ���ָ��
		ESP8266_send_data(value_buf);  //�����ϱ�APP	
	}	 
}


