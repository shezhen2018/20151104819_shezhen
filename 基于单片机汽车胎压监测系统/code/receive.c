#include"reg52.h"     //ͷ�ļ�
#include"Allhead.h"
#include <intrins.h>
#include "eeprom52.h"
sbit K1 = P3^2;    //�˵�
sbit K2 = P3^3;    //��
sbit K3 = P3^4;    //��
sbit Buzzer= P1^7;  //���ƶ� 

unchar yemian=0;
unchar flag=0,a,b,c;      //������ֵ
unchar SJwendu,SJshidu;   //ʵ����������¶Ⱥ���ѹ ��ѹ
unchar wendu,shidu,diya;  //����wendu�����¶ȣ�shidu������ѹ���ޣ�diya������ѹ���ޡ�
unchar lcdwendu[5],lcdshidu[5],lcddiya[5];

/**�����ݱ��浽��Ƭ���ڲ�eeprom��*/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, wendu);
	byte_write(0x2001, shidu);
	byte_write(0x2002, diya);
    byte_write(0x2060, a_a);	
}

/*�����ݴӵ�Ƭ���ڲ�eeprom�ж�����*/
void read_eeprom()
{
	wendu = byte_read(0x2000);
	shidu = byte_read(0x2001);
	diya  = byte_read(0x2002);
    a_a   = byte_read(0x2060);
}

/*�����Լ�eeprom��ʼ��**/
void init_eeprom()
{
	read_eeprom();		//�ȶ�
	if(a_a != 2)		//�µĵ�Ƭ����ʼ��Ƭ������eeprom
	{
		wendu = 30;
	    shidu = 98;
		diya=   90;
		a_a = 2;
		write_eeprom();
	}	
}




void wenduchuli()   //�¶���ʾ
 {
    lcdwendu[0]=wendu/10+'0';
    lcdwendu[1]=wendu%10+'0';	
  }
 
 
 void shiduchuli()   //��ѹ��ʾ	 ����
 {
    lcdshidu[0]=shidu/100+'0';
    lcdshidu[1]=shidu%100/10+'0';
	lcdshidu[2]=shidu%100%10+'0';	
 }

 
  void diyachuli()   //��ѹ��ʾ	����
 {
    lcddiya[0]=diya/100+'0';
    lcddiya[1]=diya%100/10+'0';
	lcddiya[2]=diya%100%10+'0';	
 }
 
 
 baojin() //�¶ȼ��
{
   
	    a=RevTempDate[0]-48;
	    b=RevTempDate[1]-48;
        SJwendu=a*10+b;   //�����ʵ���¶�
	
	    a=RevTempDate[3]-48;
	    b=RevTempDate[4]-48;
		c=RevTempDate[5]-48;
        SJshidu=a*100+b*10+c;   //�����ʵ����ѹ
	
	
     if( SJwendu>=wendu  || SJshidu>=shidu   ||  SJshidu<=diya  )  
			{
              Buzzer=0; // ����
			} 

     if( SJwendu<wendu  && SJshidu>diya  &&  SJshidu<shidu )
			{
              Buzzer=1; //ֹͣ����
			}

}


void key() //�������� ѡ��˵�ģʽ
{
     
	  if(!K1)
		   {  
			   LCDDelay(20);
			    if(!K1)
		         {   
							 while(!K1)
				           ;
							  yemian++;
								if(yemian>=4) { yemian=0; } 
								
             }
		   }
 
	
	
}


shezhi()  //���������ֵ�����޸�
{
    if(!K2)
		   {  
			   NRFDelay(120);
			    if(!K2)
		         {   
							// while(!K2)
				           ;
							 if(yemian==1)
							 {
							  wendu+=1;
								if(wendu>99) { wendu=99; } 
								write_eeprom();       //��������		
								wenduchuli();         //�¶���ʾ
							  LcdShow(1,6,lcdwendu);
               }
							 
							 if(yemian==2)
							 {
							    shidu+=1;
								if(shidu>250) { shidu=250; } 
								write_eeprom();       //��������		
								shiduchuli();         //�¶���ʾ
								LcdShow(1,4,lcdshidu);
                             }

							 if(yemian==3)
							 {
							    diya+=1;
								if(diya>250) { diya=250; } 
								write_eeprom();       //��������		
								diyachuli();          //��ʾ
								LcdShow(1,4,lcddiya);
                             }
             }
		   }
			 
			  if(!K3)
		   {  
			    NRFDelay(120);
			     if(!K3)
		         {   
							// while(!K3)
				           ;
							 if(yemian==1)
							 {
							    wendu-=1;
								if(wendu<=1) { wendu=1; } 
								write_eeprom();       //��������		
								wenduchuli();         //�¶���ʾ
							   LcdShow(1,6,lcdwendu);
               }
							 
							 if(yemian==2)
							    {
							    shidu-=1;
								if(shidu<=1) { shidu=1; } 
								write_eeprom();       //��������		
								shiduchuli();         //�¶���ʾ
								LcdShow(1,4,lcdshidu);
                                }
							 if(yemian==3)
							 {
							    diya-=1;
								if(diya<=1) { diya=1; } 
								write_eeprom();       //��������		
								diyachuli();          //��ʾ
								LcdShow(1,4,lcddiya);
                             }
             }
		   }

}




/************************������********************/
void main()
{  
	LCDInt();
//	LcdShow(0,2," Hello,Welcome");
//	NRFDelay(1000);//������ʱ
//	LcdShow(2,1,"             ");
//	NRFDelay(1500);//������ʱ
	
	 LCDInt();   //��ʼ��
	 init_eeprom();      			//��eeprom����
   *(RevTempDate+4)='\0';
   NRF24L01Int();   //��ʼ��
 
	while(1)
    {
		key(); //����ɨ��

	//���빤��״̬	
		if(yemian==0)  
			{
			  if(flag==0)
				 {
                 flag=1;
                 LCDInt();
				 LcdShow(0,0,"   T:     C");
                 LcdShow(2,2,"P:      Kpa");
                 }
        NRFSetRXMode();//����Ϊ����ģʽ
        GetDate();     //��ʼ������
			 
	   }
		 
  //�����¶�����״̬			 
		if(yemian==1)  //��������ģʽ
		 { 
			  
			   if(flag==1)
				 {
                       flag=2;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,0,"   Temperature  ");
					   LcdShow(2,2,"      .0 C");
					   wenduchuli();            //�¶���ʾ
					   LcdShow(1,6,lcdwendu);
					   LcdShow(2,2,0xdf);
         }
				 
				 shezhi();
				 
			
      }
			
//������ѹ��������״̬			 
		if(yemian==2)  //��������ģʽ
		 { 
			  
			   if(flag==2)
				 {
                       flag=3;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,3,"   Pressure_H  ");
					   LcdShow(2,2,"     .00Kpa");
					   shiduchuli(); //��ѹ��ʾ
				       LcdShow(1,4,lcdshidu);
                 }
              shezhi();
        }


//������ѹ��������״̬			 
		if(yemian==3)  //��������ģʽ
		 { 
			  
			   if(flag==3)
				 {
                       flag=0;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,3,"   Pressure_L  ");
					   LcdShow(2,2,"     .00Kpa");
					   diyachuli(); //��ѹ��ʾ
				       LcdShow(1,4,lcddiya);
                 }
              shezhi();
        }
			
			
	 }
}
