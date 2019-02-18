#include"reg52.h"     //头文件
#include"Allhead.h"
#include <intrins.h>
#include "eeprom52.h"
sbit K1 = P3^2;    //菜单
sbit K2 = P3^3;    //加
sbit K3 = P3^4;    //减
sbit Buzzer= P1^7;  //控制端 

unchar yemian=0;
unchar flag=0,a,b,c;      //报警伐值
unchar SJwendu,SJshidu;   //实际中所测的温度和气压 低压
unchar wendu,shidu,diya;  //变量wendu代表温度，shidu代表气压上限，diya代表气压下限。
unchar lcdwendu[5],lcdshidu[5],lcddiya[5];

/**把数据保存到单片机内部eeprom中*/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, wendu);
	byte_write(0x2001, shidu);
	byte_write(0x2002, diya);
    byte_write(0x2060, a_a);	
}

/*把数据从单片机内部eeprom中读出来*/
void read_eeprom()
{
	wendu = byte_read(0x2000);
	shidu = byte_read(0x2001);
	diya  = byte_read(0x2002);
    a_a   = byte_read(0x2060);
}

/*开机自检eeprom初始化**/
void init_eeprom()
{
	read_eeprom();		//先读
	if(a_a != 2)		//新的单片机初始单片机内问eeprom
	{
		wendu = 30;
	    shidu = 98;
		diya=   90;
		a_a = 2;
		write_eeprom();
	}	
}




void wenduchuli()   //温度显示
 {
    lcdwendu[0]=wendu/10+'0';
    lcdwendu[1]=wendu%10+'0';	
  }
 
 
 void shiduchuli()   //气压显示	 上限
 {
    lcdshidu[0]=shidu/100+'0';
    lcdshidu[1]=shidu%100/10+'0';
	lcdshidu[2]=shidu%100%10+'0';	
 }

 
  void diyachuli()   //气压显示	下限
 {
    lcddiya[0]=diya/100+'0';
    lcddiya[1]=diya%100/10+'0';
	lcddiya[2]=diya%100%10+'0';	
 }
 
 
 baojin() //温度检测
{
   
	    a=RevTempDate[0]-48;
	    b=RevTempDate[1]-48;
        SJwendu=a*10+b;   //所测的实际温度
	
	    a=RevTempDate[3]-48;
	    b=RevTempDate[4]-48;
		c=RevTempDate[5]-48;
        SJshidu=a*100+b*10+c;   //所测的实际气压
	
	
     if( SJwendu>=wendu  || SJshidu>=shidu   ||  SJshidu<=diya  )  
			{
              Buzzer=0; // 报警
			} 

     if( SJwendu<wendu  && SJshidu>diya  &&  SJshidu<shidu )
			{
              Buzzer=1; //停止报警
			}

}


void key() //按键程序 选择菜单模式
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


shezhi()  //对里面的数值进行修改
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
								write_eeprom();       //保存数据		
								wenduchuli();         //温度显示
							  LcdShow(1,6,lcdwendu);
               }
							 
							 if(yemian==2)
							 {
							    shidu+=1;
								if(shidu>250) { shidu=250; } 
								write_eeprom();       //保存数据		
								shiduchuli();         //温度显示
								LcdShow(1,4,lcdshidu);
                             }

							 if(yemian==3)
							 {
							    diya+=1;
								if(diya>250) { diya=250; } 
								write_eeprom();       //保存数据		
								diyachuli();          //显示
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
								write_eeprom();       //保存数据		
								wenduchuli();         //温度显示
							   LcdShow(1,6,lcdwendu);
               }
							 
							 if(yemian==2)
							    {
							    shidu-=1;
								if(shidu<=1) { shidu=1; } 
								write_eeprom();       //保存数据		
								shiduchuli();         //温度显示
								LcdShow(1,4,lcdshidu);
                                }
							 if(yemian==3)
							 {
							    diya-=1;
								if(diya<=1) { diya=1; } 
								write_eeprom();       //保存数据		
								diyachuli();          //显示
								LcdShow(1,4,lcddiya);
                             }
             }
		   }

}




/************************主函数********************/
void main()
{  
	LCDInt();
//	LcdShow(0,2," Hello,Welcome");
//	NRFDelay(1000);//短暂延时
//	LcdShow(2,1,"             ");
//	NRFDelay(1500);//短暂延时
	
	 LCDInt();   //初始化
	 init_eeprom();      			//读eeprom数据
   *(RevTempDate+4)='\0';
   NRF24L01Int();   //初始化
 
	while(1)
    {
		key(); //按键扫描

	//进入工作状态	
		if(yemian==0)  
			{
			  if(flag==0)
				 {
                 flag=1;
                 LCDInt();
				 LcdShow(0,0,"   T:     C");
                 LcdShow(2,2,"P:      Kpa");
                 }
        NRFSetRXMode();//设置为接收模式
        GetDate();     //开始接受数
			 
	   }
		 
  //进入温度设置状态			 
		if(yemian==1)  //进入设置模式
		 { 
			  
			   if(flag==1)
				 {
                       flag=2;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,0,"   Temperature  ");
					   LcdShow(2,2,"      .0 C");
					   wenduchuli();            //温度显示
					   LcdShow(1,6,lcdwendu);
					   LcdShow(2,2,0xdf);
         }
				 
				 shezhi();
				 
			
      }
			
//进入气压上限设置状态			 
		if(yemian==2)  //进入设置模式
		 { 
			  
			   if(flag==2)
				 {
                       flag=3;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,3,"   Pressure_H  ");
					   LcdShow(2,2,"     .00Kpa");
					   shiduchuli(); //气压显示
				       LcdShow(1,4,lcdshidu);
                 }
              shezhi();
        }


//进入气压下限设置状态			 
		if(yemian==3)  //进入设置模式
		 { 
			  
			   if(flag==3)
				 {
                       flag=0;
					   Buzzer=1;
					   LCDInt();
                       LcdShow(0,3,"   Pressure_L  ");
					   LcdShow(2,2,"     .00Kpa");
					   diyachuli(); //气压显示
				       LcdShow(1,4,lcddiya);
                 }
              shezhi();
        }
			
			
	 }
}
