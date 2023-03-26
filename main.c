#include <reg52.h>	         //���õ�Ƭ��ͷ�ļ�
#include<intrins.h>
#define uchar unsigned char  //�޷����ַ��� �궨��	������Χ0~255
#define uint  unsigned int	 //�޷������� �궨��	������Χ0~65535
#include <REG52.H>
#include "lcd1602.h"

#define K_MG_MV    120/66 

typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  �޷���8λ���ͱ���  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  �޷���16λ���ͱ��� */

unsigned char TL_baojing = 20; 
unsigned char TH_baojing = 35;  //�¶ȱ���ֵ
unsigned char HH_baojing = 70;
unsigned int L_baojing = 450;
unsigned int P_baojing = 100;
unsigned char bj_buff[4];  //��������

unsigned char moshi=0;
unsigned char data1;
long Value1,Value2;
unsigned char dat = 0x00;      //ADֵ

unsigned char str_gz[4];//����ֵ
unsigned char str_pm[4];//����ֵ

uint sum=0;
uchar IntToString(unsigned char *str, int dat);

U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;

U8 str_T[3];	 //	�¶�ֵ
U8 str_H[3];	// �¶�

bit flag1s;  //1s��־��־
bit xx_flag = 0; 

uchar value,i;		//����
uchar flag_lj_en;  //�߼�
uchar flag_lj_en_value;
uint flag_time;
uchar smg_i;
uchar dis_smg[6]={0};
uchar password[6]={6,5,4,3,2,1};	    //��ǰ���õ�����
uchar password_bj[6]={1,2,3,4,5,6};	    //����Ƚϣ�������������룩
uchar flag_password;      //������ȷ��
uchar flag_password_cichu1;//����������
uchar key_can;
uchar menu_1,menu_i;

sbit  feng  = P1^3;
sbit  relay = P1^4;//

sbit CS   = P1^5;
sbit Clk = P1^6;
sbit DATI = P1^7;
sbit DATO = P1^7;

sbit BUZZER = P3^4;
sbit  P2_0  = P3^5;  //DHT11�ӿ�

sbit JDQ1 = P3^6;
sbit JDQ2 = P3^7;

/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    TR1 = 1;       //����T1
	EA=1;
//	ES=1;
}
//�򴮿ڷ���һ���ַ�  
void send_char_com(unsigned char ch) 
{     
	SBUF=ch;   
	while(TI==0);   
	TI=0; 
} 
 //�򴮿ڷ���һ���ַ���,strlenΪ���ַ�������    
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

/***********************1ms��ʱ����*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}
void key()	 //������������
{
	static uchar key_new = 0, key_l;
	key_can = 20;                   //����ֵ��ԭ
	P2 = 0x0f;
	if((P2 & 0x0f) != 0x0f)		//��������
	{
		delay_1ms(1);	     	//����������
		if(((P2 & 0x0f) != 0x0f) && (key_new == 1))
		{						//ȷ���ǰ�������
			key_new = 0;
			key_l = (P2 | 0xf0);   //�������ɨ��
			P2 = key_l;
			switch(P2)
			{
				case 0xee:  key_can = 1;  break;  //�õ�����ֵ 
				case 0xde:  key_can = 4;  break;   //�õ�����ֵ 
				case 0xbe:  key_can = 7;  break;   //�õ�����ֵ 
				case 0x7e:  key_can = 10;  break;   //�õ�����ֵ 
	
				case 0xed:  key_can = 2;  break;  //�õ�����ֵ 
				case 0xdd:  key_can = 5;  break;   //�õ�����ֵ 
				case 0xbd:  key_can = 8;  break;   //�õ�����ֵ 
				case 0x7d:  key_can = 0;  break;   //�õ�����ֵ 
	
				case 0xeb:  key_can = 3;  break;  //�õ�����ֵ 
				case 0xdb:  key_can = 6;  break;   //�õ�����ֵ 
				case 0xbb:  key_can = 9;  break;   //�õ�����ֵ 
				case 0x7b:  key_can = 11;  break;   //�õ�����ֵ 
	
				case 0xe7:  key_can = 15;  break;  //�õ�����ֵ 
				case 0xd7:  key_can = 14;  break;  //�õ�����ֵ 
				case 0xb7:  key_can = 13;  break;   //�õ�����ֵ 
				case 0x77:  key_can = 12;  break;  //�õ�����ֵ  

			}
				
		}			
	}
	else 
	{
		key_new = 1;
		flag_lj_en = 0;
	}	
}


/***************���������**********************/
void clear_shuzu(uchar *p)
{
	for(i=0;i<7;i++)
		p[i] = 0;
}
/*************��ʱ��0��ʼ������***************/
void time_init()	  
{
	EA   = 1;	 	  //�����ж�
	TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
	TH0=(65536-10000)/256;		  //���¸�ֵ 30ms
	TL0=(65536-10000)%256;
	EA=1;            //���жϴ�
	ET0=1;           //��ʱ���жϴ�
	TR0=1;           //��ʱ�����ش�
	PT0=1;           //���ȼ���
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
/****************�������ܴ���********************/	   
void key_with()
{
	static uchar value;

	if(key_can == 13)   //�˳�
	{
		clear_shuzu(dis_smg); 
		write_string(1,0,"                ");
		write_string(2,0," Input:         ");
		menu_i = 0;
    }
	if(key_can == 14)   //�ֶ��ر�������
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
		if(menu_i < 6)   //��������
		{   
		password_bj[menu_i++] = key_can;		          
		for(i=0;i<menu_i;i++)
		dis_smg[i] ='*';//0x30 + password_bj[i];
		write_string(2,7,dis_smg);	
		}	

		if(key_can == 15)  //ȷ�ϼ�
		{												 
			value = 0;
			for(i=0;i<6;i++)
			{
				if(password_bj[i] == password[i])
				{	value ++;
					if(value>=6)
					{
						flag_password = 1;	 //������ȷ
						relay = 0;     //��������
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
					break;               //���벻��ȷ
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
	   	//��ʱ������forѭ��		  
	   	 if(U8FLAG==1)break;
	   	//�ж�����λ��0����1	 
	   	 // ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	   
	}

	//--------------------------------
	//-----ʪ�ȶ�ȡ�ӳ��� ------------
	//--------------------------------
	//----���±�����Ϊȫ�ֱ���--------
	//----�¶ȸ�8λ== U8T_data_H------
	//----�¶ȵ�8λ== U8T_data_L------
	//----ʪ�ȸ�8λ== U8RH_data_H-----
	//----ʪ�ȵ�8λ== U8RH_data_L-----
	//----У�� 8λ == U8checkdata-----
	//----��������ӳ�������----------
	//---- Delay1();, Delay_10us();,COM(); 
	//--------------------------------

	void RH(void)
	{
	  //��������18ms 
       P2_0=0;
	   Delay1(180);
	   P2_0=1;
	 //������������������ ������ʱ20us
	   Delay_10us();
	   Delay_10us();
       Delay_10us();
	   Delay_10us();	    
	 //������Ϊ���� �жϴӻ���Ӧ�ź� 
	   P2_0=1;
	 //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	   if(!P2_0)		 //T !	  
	   {
	   U8FLAG=2;
	 //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	   while((!P2_0)&&U8FLAG++);
	   U8FLAG=2;
	 //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	   while((P2_0)&&U8FLAG++);
	 //���ݽ���״̬		 
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
	 //����У�� 
	 
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
��������:ADת���ӳ���
��ڲ���:CH
���ڲ���:dat
****************************************************************************/
unsigned char adc0832(unsigned char CH)
{
      unsigned char i,test,adval;
     adval = 0x00;
     test = 0x00;
     Clk = 0;       //��ʼ��
     DATI = 1;
    _nop_();
    CS = 0;
    _nop_();
    Clk = 1;
   _nop_();


   if ( CH == 0x00 )      //ͨ��ѡ��
   {
       Clk = 0;
       DATI = 1;      //ͨ��0�ĵ�һλ
       _nop_();
      Clk = 1;
        _nop_();
        Clk = 0;
      DATI = 0;      //ͨ��0�ĵڶ�λ
      _nop_();
      Clk = 1;
      _nop_();
    } 
    else
    {
       Clk = 0;
    DATI = 1;      //ͨ��1�ĵ�һλ
      _nop_();
      Clk = 1;
      _nop_();
      Clk = 0;
      DATI = 1;      //ͨ��1�ĵڶ�λ
    _nop_();
     Clk = 1;
     _nop_();
   }

      Clk = 0;
      DATI = 1;
    for( i = 0;i < 8;i++ )      //��ȡǰ8λ��ֵ
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
      for (i = 0; i < 8; i++)      //��ȡ��8λ��ֵ
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
      if(adval == test) //�Ƚ�ǰ8λ���8λ��ֵ���������ͬ��ȥ����һֱ������ʾΪ�㣬�뽫����ȥ��
       dat = test;
       nop_();
       CS = 1;        //�ͷ�ADC0832
       DATO = 1;
       Clk = 1;
       return dat;
}
void WSDXS() //��ʪ����ʾ
{	   
	str_T[0]=U8T_data_H%100/10+0x30;	// ����ʮλ
	str_T[1]=U8T_data_H%10+0x30;		// ������λ
	str_T[2] = '\0';
				  // ʪ��
	str_H[0]=U8RH_data_H%100/10+0x30;   // ����ʮλ
	str_H[1]=U8RH_data_H%10+0x30;	      // ������λ
	str_H[2]= '\0';   						     

	write_string(1, 2, str_T);        //��ʾ��Һ������
	write_string(1, 9, str_H);        //��ʾ��Һ������
}

/******************������**********************/	   
void main()
{	
    uchar m, len;
	
	time_init();   //��ʱ����ʼ��  
	ConfigUART(9600);
	init_1602_2();  //   ��ʾ����ʼ��
	
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
	   	key();	//����ɨ��
		Key_set_scan();//����������
		
		if(moshi == 0)	  //����ģʽ�л�
		{	
		    if(xx_flag==1)key_with();	//����������������
			else
			{
				RH();    //������ʪ�ȶ�ȡ�ӳ���
				WSDXS(); //��ʪ����ʾ
				
				for(m=0;m<10;m++)
				{
					Value1 = adc0832(0); //������
					sum=sum+Value1;
				}
				Value1=sum/10;
				sum=0; 
				Value1 =  Value1;//*(float)(Value1/5);//Ũ��У׼
				if(	Value1>999)Value1=999;
				len = IntToString(str_pm,Value1); //ת�����ַ���

				while (len < 3)                  //�ÿո��뵽5���ַ�����
				{
					str_pm[len++] = ' ';
				}
				str_pm[len] = '\0';              //����ַ���������
				
				write_string(2, 2, str_pm); //����Ũ����ʾ��Һ������  
				
				Value2 = adc0832(0x01); //����ǿ�ȼ��
				Value2 = 500-Value2*1.96;//���ݴ���	 ADCת�����ķֱ�����255��ת����500Ҫ����1.96 �ó�A��   Ȼ�����ڵ�·�й���ԽǿAԽС����������500-A��

				len = IntToString(str_gz,Value2); //ת�����ַ���

				while (len < 3)                  //�ÿո��뵽5���ַ�����
				{
				str_gz[len++] = ' ';
				}
				str_gz[len] = '\0';              //����ַ���������

				write_string(2, 11, str_gz); //����Ũ����ʾ��Һ������  
				

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
		else if(moshi == 1)	//�����¶�����ģʽ
		{	
			bj_buff[0] = TL_baojing/10+0x30;
			bj_buff[1] = TL_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 7,bj_buff);
		}
		else if(moshi == 2)	//�����¶�����ģʽ
		{	
			bj_buff[0] = TH_baojing/10+0x30;
			bj_buff[1] = TH_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 7,bj_buff);
		}
		else if(moshi == 3)	  //����ʪ������ģʽ
		{
			bj_buff[0] = HH_baojing/10+0x30;
			bj_buff[1] = HH_baojing%10+0x30;
			bj_buff[2] = '\0';
			write_string(2, 6,bj_buff);
		}
		else if(moshi == 4)	 //��������Ũ������ģʽ
		{
			bj_buff[0] = P_baojing/100+0x30;
			bj_buff[1] = P_baojing%100/10+0x30;
			bj_buff[2] = P_baojing%10+0x30;
			bj_buff[3] = '\0';
			write_string(2, 6,bj_buff);
		}	
		else if(moshi == 5)	  //����ʪ������ģʽ
		{
			bj_buff[0] = L_baojing/100+0x30;
			bj_buff[1] = L_baojing%100/10+0x30;
			bj_buff[2] = L_baojing%10+0x30;
			bj_buff[3] = '\0';
			write_string(2, 6,bj_buff);
		}		
	}
}

/*************��ʱ��0�жϷ������***************/
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
/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //���Ϊ����������
	
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }

    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}


