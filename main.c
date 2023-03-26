#include <reg52.h>	         //调用单片机头文件
#include<intrins.h>
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535
#include <REG52.H>
#include "lcd1602.h"

#define K_MG_MV    120/66 

typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  无符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  无符号16位整型变量 */

unsigned char TL_baojing = 20; 
unsigned char TH_baojing = 35;  //温度报警值
unsigned char HH_baojing = 70;
unsigned int L_baojing = 450;
unsigned int P_baojing = 100;
unsigned char bj_buff[4];  //报警缓存

unsigned char moshi=0;
unsigned char data1;
long Value1,Value2;
unsigned char dat = 0x00;      //AD值

unsigned char str_gz[4];//光照值
unsigned char str_pm[4];//烟雾值

uint sum=0;
uchar IntToString(unsigned char *str, int dat);

U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;

U8 str_T[3];	 //	温度值
U8 str_H[3];	// 温度

bit flag1s;  //1s标志标志
bit xx_flag = 0; 

uchar value,i;		//变量
uchar flag_lj_en;  //边加
uchar flag_lj_en_value;
uint flag_time;
uchar smg_i;
uchar dis_smg[6]={0};
uchar password[6]={6,5,4,3,2,1};	    //当前设置的密码
uchar password_bj[6]={1,2,3,4,5,6};	    //密码比较（现在输入的密码）
uchar flag_password;      //密码正确否
uchar flag_password_cichu1;//密码错误次数
uchar key_can;
uchar menu_1,menu_i;

sbit  feng  = P1^3;
sbit  relay = P1^4;//

sbit CS   = P1^5;
sbit Clk = P1^6;
sbit DATI = P1^7;
sbit DATO = P1^7;

sbit BUZZER = P3^4;
sbit  P2_0  = P3^5;  //DHT11接口

sbit JDQ1 = P3^6;
sbit JDQ2 = P3^7;

/* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    TR1 = 1;       //启动T1
	EA=1;
//	ES=1;
}
//向串口发送一个字符  
void send_char_com(unsigned char ch) 
{     
	SBUF=ch;   
	while(TI==0);   
	TI=0; 
} 
 //向串口发送一个字符串,strlen为该字符串长度    
void send_string_com(unsigned char *str,unsigned int strlen)
 {
   unsigned int k=0;    
   do 
   {  
   send_char_com(*(str + k));   
   k++;     
   }
   while(k < strlen);
 }

/***********************1ms延时函数*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}
void key()	 //独立按键程序
{
	static uchar key_new = 0, key_l;
	key_can = 20;                   //按键值还原
	P2 = 0x0f;
	if((P2 & 0x0f) != 0x0f)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(((P2 & 0x0f) != 0x0f) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;
			key_l = (P2 | 0xf0);   //矩阵键盘扫描
			P2 = key_l;
			switch(P2)
			{
				case 0xee:  key_can = 1;  break;  //得到按键值 
				case 0xde:  key_can = 4;  break;   //得到按键值 
				case 0xbe:  key_can = 7;  break;   //得到按键值 
				case 0x7e:  key_can = 10;  break;   //得到按键值 
	
				case 0xed:  key_can = 2;  break;  //得到按键值 
				case 0xdd:  key_can = 5;  break;   //得到按键值 
				case 0xbd:  key_can = 8;  break;   //得到按键值 
				case 0x7d:  key_can = 0;  break;   //得到按键值 
	
				case 0xeb:  key_can = 3;  break;  //得到按键值 
				case 0xdb:  key_can = 6;  break;   //得到按键值 
				case 0xbb:  key_can = 9;  break;   //得到按键值 
				case 0x7b:  key_can = 11;  break;   //得到按键值 
	
				case 0xe7:  key_can = 15;  break;  //得到按键值 
				case 0xd7:  key_can = 14;  break;  //得到按键值 
				case 0xb7:  key_can = 13;  break;   //得到按键值 
				case 0x77:  key_can = 12;  break;  //得到按键值  

			}
				
		}			
	}
	else 
	{
		key_new = 1;
		flag_lj_en = 0;
	}	
}


/***************把数组清空**********************/
void clear_shuzu(uchar *p)
{
	for(i=0;i<7;i++)
		p[i] = 0;
}
/*************定时器0初始化程序***************/
void time_init()	  
{
	EA   = 1;	 	  //开总中断
	TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
	TH0=(65536-10000)/256;		  //重新赋值 30ms
	TL0=(65536-10000)%256;
	EA=1;            //总中断打开
	ET0=1;           //定时器中断打开
	TR0=1;           //定时器开关打开
	PT0=1;           //优先级打开
}
void Key_set_scan()
{ 
	if(key_can == 10)
	{
		write_string(1,0,"                ");
		write_string(2,0,"                ");
		moshi++;
		if(moshi >= 6)moshi = 0;
		if(moshi == 0)
		{ 
			if(xx_flag==0)
			{
				write_string(1,0,"T:  C  H:  %RH  ");
				write_string(2,0,"M:   PPM L:   Lx");
			}
			else 
			{
				write_string(2,0," Input:         ");
			}
			
		}
		else if(moshi == 1)write_string(2, 0,"Set_TL:   C    ");
		else if(moshi == 2)write_string(2, 0,"Set_TH:   C    ");
		else if(moshi == 3)write_string(2, 0,"Set_H:   %RH   ");
		else if(moshi == 4)write_string(2, 0,"Set_M:    PPM  ");
		else if(moshi == 5)write_string(2, 0,"Set_L:    Lx   ");
	}	
	if(key_can == 11)
	{ 	
		if(moshi==1)
		{
		 TL_baojing++ ;
		 if( TL_baojing>=99 )TL_baojing =99;
		}
		else if(moshi==2)
		{
		 TH_baojing++ ;
		 if( TH_baojing>=99 )TH_baojing =99;
		}
		else if(moshi==3)
		{
		 HH_baojing++ ;
		 if( HH_baojing>=99 )HH_baojing =99;
		}
		else if(moshi==4)
		{
		 P_baojing++ ;
		 if( P_baojing>=999 )P_baojing =999;
		}
		else if(moshi==5)
		{
		 L_baojing++ ;
		 if( L_baojing>=999 )L_baojing =999;
		}
	}
	if(key_can == 12)
   {
	   if(moshi == 0)
		{ 
			xx_flag = ~xx_flag;
			if(xx_flag==0)
			{
				write_string(1,0,"T:  C  H:  %RH  ");
				write_string(2,0,"M:   PPM L:   Lx");
			}
			else 
			{
				write_string(1,0,"                ");
				write_string(2,0," Input:         ");
			}
		} 
		else if(moshi==1)
		{
		 if( TL_baojing>0 ) TL_baojing-- ;
		}
		else if(moshi==2)
		{
		 if( TH_baojing>0 ) TH_baojing-- ;
		}
		else if(moshi==3)
		{
		 if( HH_baojing>0 ) HH_baojing-- ;
		}
		else if(moshi==4)
		{
		 if( P_baojing>0 ) P_baojing-- ;
		}
		else if(moshi==5)
		{
		 if( L_baojing>0 ) L_baojing-- ;
		}
   }	 
}
/****************按键功能处理********************/	   
void key_with()
{
	static uchar value;

	if(key_can == 13)   //退出
	{
		clear_shuzu(dis_smg); 
		write_string(1,0,"                ");
		write_string(2,0," Input:         ");
		menu_i = 0;
    }
	if(key_can == 14)   //手动关闭密码锁
	{
		relay = 1;
		password_bj[0] = 0;
		password_bj[1] = 0;
		password_bj[2] = 0;
		password_bj[3] = 0;
		password_bj[4] = 0;
		password_bj[5] = 0;
    }
	if(menu_1 == 0)						                                        
	{
		if(key_can <= 9)
		if(menu_i < 6)   //密码输入
		{   
		password_bj[menu_i++] = key_can;		          
		for(i=0;i<menu_i;i++)
		dis_smg[i] ='*';//0x30 + password_bj[i];
		write_string(2,7,dis_smg);	
		}	

		if(key_can == 15)  //确认键
		{												 
			value = 0;
			for(i=0;i<6;i++)
			{
				if(password_bj[i] == password[i])
				{	value ++;
					if(value>=6)
					{
						flag_password = 1;	 //密码正确
						relay = 0;     //打开密码锁
						flag_password_cichu1 = 0;
						for(i=0;i<6;i++)
						dis_smg[i] = 0xbf;
					}
				}
				else 
				{
					flag_password = 0;
					flag_password_cichu1++;					
					delay_1ms(280);
			
					for(i=0;i<6;i++)
					{
						BUZZER  = ~BUZZER ;
						delay_1ms(200);
					}
					BUZZER  = 1;
					delay_1ms(800);
					break;               //密码不正确
				}
			}
			clear_shuzu(dis_smg); 
			
	        write_string(2,0," Input:         ");
			menu_i = 0;
		}
	}									
}

 void Delay1(U16 j)
 {    
    U8 i;
	for(;j>0;j--)
	{ 	
	for(i=0;i<27;i++);
	}
 }
void  Delay_10us(void)
      {
        U8 i;
        i--;
        i--;
        i--;
        i--;
        i--;
        i--;
       }
	
void  COM(void)
   {   
	    U8 i;
        for(i=0;i<8;i++)	   
	  {
	     U8FLAG=2;	
	   	while((!P2_0)&&U8FLAG++);
			Delay_10us();
            Delay_10us();				
            Delay_10us();
	  		U8temp=0;
	     if(P2_0)U8temp=1;
		    U8FLAG=2;
		 while((P2_0)&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	 if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	 // 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	   
	}

	//--------------------------------
	//-----湿度读取子程序 ------------
	//--------------------------------
	//----以下变量均为全局变量--------
	//----温度高8位== U8T_data_H------
	//----温度低8位== U8T_data_L------
	//----湿度高8位== U8RH_data_H-----
	//----湿度低8位== U8RH_data_L-----
	//----校验 8位 == U8checkdata-----
	//----调用相关子程序如下----------
	//---- Delay1();, Delay_10us();,COM(); 
	//--------------------------------

	void RH(void)
	{
	  //主机拉低18ms 
       P2_0=0;
	   Delay1(180);
	   P2_0=1;
	 //总线由上拉电阻拉高 主机延时20us
	   Delay_10us();
	   Delay_10us();
       Delay_10us();
	   Delay_10us();	    
	 //主机设为输入 判断从机响应信号 
	   P2_0=1;
	 //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	   if(!P2_0)		 //T !	  
	   {
	   U8FLAG=2;
	 //判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!P2_0)&&U8FLAG++);
	   U8FLAG=2;
	 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	   while((P2_0)&&U8FLAG++);
	 //数据接收状态		 
	   COM();
	   U8RH_data_H_temp=U8comdata;
	   COM();
	   U8RH_data_L_temp=U8comdata;
	   COM();
	   U8T_data_H_temp=U8comdata;
	   COM();
	   U8T_data_L_temp=U8comdata;
	   COM();
	   U8checkdata_temp=U8comdata;
	   P2_0=1;
	 //数据校验 
	 
	   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	   if(U8temp==U8checkdata_temp)
	   {
	   	  U8RH_data_H=U8RH_data_H_temp;
	   	  U8RH_data_L=U8RH_data_L_temp;
		  U8T_data_H=U8T_data_H_temp;
	   	  U8T_data_L=U8T_data_L_temp;
	   	  U8checkdata=U8checkdata_temp;
	   }//fi
	   }//fi

	}

/****************************************************************************
函数功能:AD转换子程序
入口参数:CH
出口参数:dat
****************************************************************************/
unsigned char adc0832(unsigned char CH)
{
      unsigned char i,test,adval;
     adval = 0x00;
     test = 0x00;
     Clk = 0;       //初始化
     DATI = 1;
    _nop_();
    CS = 0;
    _nop_();
    Clk = 1;
   _nop_();


   if ( CH == 0x00 )      //通道选择
   {
       Clk = 0;
       DATI = 1;      //通道0的第一位
       _nop_();
      Clk = 1;
        _nop_();
        Clk = 0;
      DATI = 0;      //通道0的第二位
      _nop_();
      Clk = 1;
      _nop_();
    } 
    else
    {
       Clk = 0;
    DATI = 1;      //通道1的第一位
      _nop_();
      Clk = 1;
      _nop_();
      Clk = 0;
      DATI = 1;      //通道1的第二位
    _nop_();
     Clk = 1;
     _nop_();
   }

      Clk = 0;
      DATI = 1;
    for( i = 0;i < 8;i++ )      //读取前8位的值
    {
       _nop_();
       adval <<= 1;
       Clk = 1;
       _nop_();
       Clk = 0;
       if (DATO)
          adval |= 0x01;
      else
          adval |= 0x00;
    }
      for (i = 0; i < 8; i++)      //读取后8位的值
      {
           test >>= 1;
           if (DATO)
              test |= 0x80;
           else 
              test |= 0x00;
          _nop_();
          Clk = 1;
          _nop_();
          Clk = 0;
      }
      if(adval == test) //比较前8位与后8位的值，如果不相同舍去。若一直出现显示为零，请将该行去掉
       dat = test;
       nop_();
       CS = 1;        //释放ADC0832
       DATO = 1;
       Clk = 1;
       return dat;
}
void WSDXS() //温湿度显示
{	   
	str_T[0]=U8T_data_H%100/10+0x30;	// 整数十位
	str_T[1]=U8T_data_H%10+0x30;		// 整数个位
	str_T[2] = '\0';
				  // 湿度
	str_H[0]=U8RH_data_H%100/10+0x30;   // 整数十位
	str_H[1]=U8RH_data_H%10+0x30;	      // 整数个位
	str_H[2]= '\0';   						     

	write_string(1, 2, str_T);        //显示到液晶屏上
	write_string(1, 9, str_H);        //显示到液晶屏上
}

/******************主程序**********************/	   
void main()
{	
    uchar m, len;
	
	time_init();   //定时器初始化  
	ConfigUART(9600);
	init_1602_2();  //   显示屏初始化
	
	if(xx_flag==0)
	{
		write_string(1,0,"T:  C  H:  %RH  ");
		write_string(2,0,"M:   PPM L:   Lx");
	}
	else 
	{
		write_string(2,0," Input:         ");
	}
	
	while(1)
	{		
	   	key();	//按键扫描
		Key_set_scan();//按键处理函数
		
		if(moshi == 0)	  //设置模式切换
		{	
		    if(xx_flag==1)key_with();	//密码锁按键处理函数
			else
			{
				RH();    //调用温湿度读取子程序
				WSDXS(); //温湿度显示
				
				for(m=0;m<10;m++)
				{
					Value1 = adc0832(0); //烟雾检测
					sum=sum+Value1;
				}
				Value1=sum/10;
				sum=0; 
				Value1 =  Value1;//*(float)(Value1/5);//浓度校准
				if(	Value1>999)Value1=999;
				len = IntToString(str_pm,Value1); //转换成字符串

				while (len < 3)                  //用空格补齐到5个字符长度
				{
					str_pm[len++] = ' ';
				}
				str_pm[len] = '\0';              //添加字符串结束符
				
				write_string(2, 2, str_pm); //烟雾浓度显示到液晶屏上  
				
				Value2 = adc0832(0x01); //光照强度检测
				Value2 = 500-Value2*1.96;//数据处理	 ADC转换器的分辨率是255，转换成500要乘以1.96 得出A。   然后由于电路中光照越强A越小，所以再让500-A。

				len = IntToString(str_gz,Value2); //转换成字符串

				while (len < 3)                  //用空格补齐到5个字符长度
				{
				str_gz[len++] = ' ';
				}
				str_gz[len] = '\0';              //添加字符串结束符

				write_string(2, 11, str_gz); //光照浓度显示到液晶屏上  
				

				if((U8T_data_H>TH_baojing)||(U8RH_data_H>HH_baojing)||(Value1>P_baojing)||(Value2>L_baojing)) BUZZER = 0; else BUZZER = 1;
				
				if((U8T_data_H>TH_baojing)) feng = 0; else feng = 1; 
				if(Value2>L_baojing) JDQ2 = 0; else JDQ2 = 1;
				if((U8T_data_H<TL_baojing)) JDQ1 = 0; else JDQ1 = 1;

				if(flag1s==1)
				{	 
					send_string_com("T:",2);  
					send_string_com(str_T,2);  
					send_string_com("C H:",4); 
					send_string_com(str_H,2);  
					send_string_com("%RH",3); 					
					send_string_com("\r\n",2);
					
					send_string_com("M:",2);  
					send_string_com(str_pm,3);  
					send_string_com("PPM L:",6);  
					send_string_com(str_gz,3);  
					send_string_com("Lx",2);  
					send_string_com("\r\n",2);
					send_string_com("\r\n",2);
				
					flag1s = 0;
				}	
			}			
		}
		else if(moshi == 1)	//设置温度下限模式
		{	
			bj_buff[0] = TL_baojing/10+0x30;
			bj_buff[1] = TL_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 7,bj_buff);
		}
		else if(moshi == 2)	//设置温度上限模式
		{	
			bj_buff[0] = TH_baojing/10+0x30;
			bj_buff[1] = TH_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 7,bj_buff);
		}
		else if(moshi == 3)	  //设置湿度上限模式
		{
			bj_buff[0] = HH_baojing/10+0x30;
			bj_buff[1] = HH_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 6,bj_buff);
		}
		else if(moshi == 4)	 //设置烟雾浓度上限模式
		{
			bj_buff[0] = P_baojing/100+0x30;
			bj_buff[1] = P_baojing%100/10+0x30;
			bj_buff[2] = P_baojing%10+0x30;
			bj_buff[3] = '\0';
			write_string(2, 6,bj_buff);
		}	
		else if(moshi == 5)	  //设置湿度下限模式
		{
			bj_buff[0] = L_baojing/100+0x30;
			bj_buff[1] = L_baojing%100/10+0x30;
			bj_buff[2] = L_baojing%10+0x30;
			bj_buff[3] = '\0';
			write_string(2, 6,bj_buff);
		}		
	}
}

/*************定时器0中断服务程序***************/
void Time_T0(void) interrupt 1//1ms
{	
	static unsigned char times;
	TH0=(65536-10000)/256;   
	TL0=(65536-10000)%256;
	
	times++;
	if(times>=100)
	{
		times = 0;
		flag1s = 1;
	}
}
/* 整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //如果为负数，首先
	
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }

    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}


