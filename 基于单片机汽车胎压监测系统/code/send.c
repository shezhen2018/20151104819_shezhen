#include"reg52.h"
#include"Allhead.h"
#include  <INTRINS.H>    //Keil �� 
#define   uchar unsigned char
#define   uint unsigned int	


#define	BMP085_SlaveAddress   0xee	  //����������IIC�����еĴӵ�ַ                               

#define OSS 0	
							   
typedef unsigned char  BYTE;
typedef unsigned short WORD;


sbit	  SCL=P3^3;      //IICʱ�����Ŷ���
sbit 	  SDA=P3^2;      //IIC�������Ŷ���

   	
uchar ge,shi,bai,qian,wan,shiwan;           //��ʾ����
int  dis_data;                              //����
uchar TxDate[8];

short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;
typedef bit BOOL  ; 
uchar data_byte;


void conversion(long temp_data);
uchar Single_Read(uchar REG_Address);       //������ȡ�ڲ��Ĵ�������
bit  BMP085_RecvACK();
BYTE BMP085_RecvByte();


void conversion(long temp_data)  
{  
    
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //ȡ������ 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //ȡ������
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //ȡ������
    ge=temp_data+0x30; 	
}


/*
��ʱ5΢��(STC90C52RC@12M)
��ͬ�Ĺ�������,��Ҫ�����˺�����ע��ʱ�ӹ���ʱ��Ҫ�޸�
������1T��MCUʱ,���������ʱ����
*/
void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}

/*
��ʱ5����(STC90C52RC@12M)
��ͬ�Ĺ�������,��Ҫ�����˺���
������1T��MCUʱ,���������ʱ����
*/
void Delay5ms()
{
    WORD n = 560;

    while (n--);
}

/*
��ʼ�ź�
*/
void BMP085_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}

/*
ֹͣ�ź�
*/
void BMP085_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    Delay5us();                 //��ʱ
}

/*
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
*/
void BMP085_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

/*
����Ӧ���ź�
*/
bit BMP085_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ

    return CY;
}

/*
��IIC���߷���һ���ֽ�����
*/
void BMP085_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    BMP085_RecvACK();
}

/* 
��IIC���߽���һ���ֽ�����
*/
BYTE BMP085_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}


//����BMP085�ڲ�����,��������

short Multiple_read(uchar ST_Address)
{   
	uchar msb, lsb;
	short _data;
    msb = BMP085_RecvByte();                 //BUF[0]�洢
    BMP085_SendACK(0);                       //��ӦACK
    lsb = BMP085_RecvByte();     
	BMP085_SendACK(1);                       //���һ��������Ҫ��NOACK

    BMP085_Stop();                           //ֹͣ�ź�
    Delay5ms();
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{

    BMP085_Start();                  //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //����ֹͣ�ź�
	Delay(10);	// max time is 4.5ms
	
	return (long) Multiple_read(0xF6);
}
//
long bmp085ReadPressure(void)
{
	long pressure = 0;

    BMP085_Start();                   //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34);       	  // write register data for pressure
    BMP085_Stop();                    //����ֹͣ�ź�
	Delay(10);    	                  // max time is 4.5ms
	
	pressure = Multiple_read(0xF6);
	pressure &= 0x0000FFFF;
	
	return pressure;	
	//return (long) bmp085ReadShort(0xF6);
}


//��ʼ��BMP085��������Ҫ��ο�pdf�����޸�*
void Init_BMP085()
{
	ac1 = Multiple_read(0xAA);
	ac2 = Multiple_read(0xAC);
	ac3 = Multiple_read(0xAE);
	ac4 = Multiple_read(0xB0);
	ac5 = Multiple_read(0xB2);
	ac6 = Multiple_read(0xB4);
	b1 =  Multiple_read(0xB6);
	b2 =  Multiple_read(0xB8);
	mb =  Multiple_read(0xBA);
	mc =  Multiple_read(0xBC);
	md =  Multiple_read(0xBE);
}
//
void bmp085Convert()
{
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	long  temperature;
	long  pressure;
	
	ut = bmp085ReadTemp();
	ut = bmp085ReadTemp();	   // ��ȡ�¶�
	up = bmp085ReadPressure();
	up = bmp085ReadPressure();  // ��ȡѹǿ
	
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	 temperature = (b5 + 8) >> 4;

	

	 conversion(temperature);
	
     TxDate[0]=bai;       
     TxDate[1]=shi; 
     
	 TxDate[2]=ge; 
	// display(0x09,0XDF);     //�¶ȵ�λ
	

	 
	
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	     p = (b7 * 2) / b4 ;
           else  
		    p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	 pressure = p + ((x1 + x2 + 3791) >> 4);

	 conversion(pressure);
   
	 TxDate[3]=shiwan; 
	 TxDate[4]=wan;    
     TxDate[5]=qian; 
     TxDate[6]=bai; 
     TxDate[7]=shi; 
	

}



/*********************������*************************/

void main()
{
   NRF24L01Int();
   Init_BMP085();                //��ʼ��BMP085 
   while(1)
    {
		
		 bmp085Convert();
	  
		 NRFSetTxMode(TxDate);//�����¶�
		 while(CheckACK());	//����Ƿ������
		
	}

}


