#include "wifi.h"
#include "string.h"
#include "delay.h"

///////////////���ڻ�������//////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

/*************  ���ر�������	**************/
char Uart3_Buf[Buf3_Max];//����2���ջ���
u8 First_Int = 0;

//����3��ʼ��
void USART3_init(u32 bound)
{
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3��GPIOBʱ��
	
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB

	//USART3_RX	  GPIOB11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB  

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3

}

void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res=0;
	Res = USART_ReceiveData(USART3);
	Uart3_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
	First_Int++;                	  //����ָ������ƶ�
	if(First_Int >= Buf3_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
	{
		First_Int = 0;
	}
} 

//������ڻ�������
void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf3_Max;k++)      //��������������
	{
		Uart3_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

//�����ַ���
void UART3_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
		USART_SendData(USART3 ,*s++);//���͵�ǰ�ַ�
	}
}

//��������
void UART3_Send_Command(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART3_SendString(s); //�����ַ���
	UART3_SendString("\r"); //���Զ����� \r\n�����ַ�
}


//Ѱ���ַ���
//���أ�1  ���ҵ�  0  δ�ҵ�
u8 Find(char *a)
{ 
	if(strstr(Uart3_Buf, a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}


//����ATָ��
//*b����Ҫ���͵��ַ���
//*a�������Ƿ񷵻ص��ַ���
//wait_time�����͵Ĵ���
//interval_time:ÿ�εȴ���ʱ��
u8 UART3_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART3_Send_Command(b);//����2���� b �ַ��� �����Զ�����\r\n  �൱�ڷ�����һ��ָ��
		delay_ms(interval_time); //�ȴ�һ��ʱ�� ��50�Ļ����� 50*20ms = 1��
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//����WIFIģ��ģʽ
//������ AP ��������� ����  ͨ����  WPA2_PSK 
signed ESP8266_wifi_Init(void)
{
	u8 ret;
	ret = UART3_Send_AT_Command("AT\r\n","OK",3,100);    //���ģ���Ƿ����
	if(ret == 0)
	{
		return -1;
	}
	
	ret = UART3_Send_AT_Command("AT+CWMODE=2\r\n","OK",3,100);   //����ģ��ΪAPģʽ
	if(ret == 0)
	{
		return -2;
	}
	
	ret = UART3_Send_AT_Command("AT+CWSAP=\"wifi\",\"12345678\",5,3\r\n","OK",3,100);  //���ý�������� ����  ͨ����  WPA2_PSK
	if(ret == 0)
	{
		return -3;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPMUX=1\r\n","OK",3,100); //����Ϊ������
	if(ret == 0)
	{
		return -4;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPSERVER=1,5000\r\n","OK",3,100);  //����ģ��ΪTCP���������˿ں�Ϊ5000
	if(ret == 0)
	{
		return -5;
	}
	
	ret = UART3_Send_AT_Command("AT+CIPSTO=0\r\n","OK",3,100);   //���ó�ʱʱ��Ϊ0
	if(ret == 0)
	{
		return -6;
	}
	
	return 0;
}

//��������
//*buf����Ҫ���͵��ַ���
void ESP8266_send_data(char *buf)
{
	UART3_Send_AT_Command("AT+CIPSEND=0,14\r\n",">",3,50);   //����ͨ���ţ��Լ���Ҫ���͵��ַ�����
	delay_ms(200);
	UART3_SendString(buf);
	delay_ms(100);
}
















