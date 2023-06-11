/*
 * project007.c
 *
 * Created: 2023/6/7 17:58:17
 * Author : 雷樱者
 */ 
#define F_CPU 800000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define INT8U unsigned int
#define uchar unsigned char
#define Get_Bit(val, bitn) (val &(1<<(bitn)) )

#pragma interrupt_handler int0_isr:2 //声明中断,2表示中断向量号

const INT8U AAW[]={0x01,0x02,0x04,0x08};
const INT8U BBW[]={0x03,0x06,0x0c,0x09};
const INT8U ABW[]={0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09};
const INT8U step[] = { 0x04, 0x08, 0x0c, 0x0c, 0x10, 0x14, 0x18, 0x20};//对应4,8,12,16,20,24,28,32步

//按键定义
#define K1_DOWN()(Get_Bit(PIND,0)!=0)//K1:反转
#define K2_DOWN()(Get_Bit(PIND,1)!=0)//K2:正转
#define K3_DOWN()(Get_Bit(PIND,6)!=0)//K3:四相单四拍模式
#define K4_DOWN()(Get_Bit(PIND,4)!=0)//K4:四相双四拍模式
#define K5_DOWN()(Get_Bit(PIND,5)!=0)//K5:四相单双八拍模式
#define K6_UP()(Get_Bit(PIND,2)==0)//K6：总开关
#define K7_DOWN()(Get_Bit(PIND,7)!=0)//K7:过热指示

#define A1_DOWN()(Get_Bit(PINA,0)!=0)//A1:4步
#define A2_DOWN()(Get_Bit(PINA,1)!=0)//A2:8步
#define A3_DOWN()(Get_Bit(PINA,2)!=0)//A3:12步
#define A4_DOWN()(Get_Bit(PINA,3)!=0)//A4:16步
#define A5_DOWN()(Get_Bit(PINA,4)!=0)//A5:20步
#define A6_DOWN()(Get_Bit(PINA,5)!=0)//A6:24步
#define A7_DOWN()(Get_Bit(PINA,6)!=0)//A7:28步
#define A8_DOWN()(Get_Bit(PINA,7)!=0)//K8:32步


#define DQ_DIR_IN()    DDRC &= ~(1<<4)      //PC1引脚设置为输入
#define DQ_DIR_OUT()   DDRC |= (1<<4)		//将RC1设置为输出
#define DQ_DATA     ((PINC&(1<<4))>>4)		//引脚上的输入的电平
#define DQ_SET()     PORTC |= (1<<4)	    //输出高电平
#define DQ_CLEAR()    PORTC &= ~(1<<4)	//输出低电平

#define LED_DIR_OUT()   DDRD |= (1<<6)		//将PD6设置为输出
#define LED_ARL()    PORTD |= (1<<6)        //将第六位置1输出高电平
#define LED_CAN()  PORTD &= ~(1<<6) //将第六位置0输出低电平

//外部中断相关的寄存器初始化，因为接的是PD2，使用的是INT0
void INT_init()
{
	GIFR = 0x40; //INTF0标志位清零
	MCUCR = 0X00; //INT0低电平触发
	GICR = 0X40; //INT0、INT1触发允许
}


void int0_isr(void){
	PORTB=0x01;
	while(K6_UP());
}



INT8U STEP(void){
	if(A1_DOWN()){
		_delay_ms(10);
		if(A1_DOWN()){
			return step[0];
		}
	}
	if(A2_DOWN()){
		_delay_ms(10);
		if(A2_DOWN()){
			return step[1];
		}
	}
	if(A3_DOWN()){
		_delay_ms(10);
		if(A3_DOWN()){
			return step[2];
		}
	}
	if(A4_DOWN()){
		_delay_ms(10);
		if(A4_DOWN()){
			return step[3];
		}
	}
	if(A5_DOWN()){
		_delay_ms(10);
		if(A5_DOWN()){
			return step[4];
		}
	}
	if(A6_DOWN()){
		_delay_ms(10);
		if(A6_DOWN()){
			return step[5];
		}
	}
	if(A7_DOWN()){
		_delay_ms(10);
		if(A7_DOWN()){
			return step[6];
		}
	}
	return 0;		
}


int main()
{
	DDRA=0X00;PORTA=0x00; //输入步数
	DDRB=0XFF;PORTB=0x01;   //代表B为输出，并置位电机的初始位置
	DDRD=0X00;PORTD=0x00;     //代表D为按键输入
	DDRC=0X00;PORTC=0x00; //C为温度传感器的信号输入
	LED_DIR_OUT();
	INT_init();
	asm("sei"); //启用全局中断
	while(1)
	{
		if(K1_DOWN())
		{
			_delay_us(2000);
			if(K1_DOWN()&& !K6_UP()){
					INT8U j;
				if(K3_DOWN()){
					for(j=0;j<STEP();j++)//走对应的是step[Index]步
					{
						PORTB=AAW[j%4];
						_delay_ms(500);
					}
					//PORTB=0x01;//最后一圈之后输出0x01这一拍电机回到起点
					}else if(K4_DOWN()){
					for(j=0;j<STEP();j++)//走对应的是step[Index]步
					{
						PORTB=BBW[j%4];
						_delay_ms(500);
					}
					//PORTB=0x01;//最后一圈之后输出0x00这一拍电机回到起点
					}else if(K5_DOWN()){
					for(j=0;j<STEP();j++)//走对应的是step[Index]步
					{
						PORTB=ABW[j%8];
						_delay_ms(500);
					}
					//PORTB=0x01;//最后一圈之后输出0x01这一拍电机回到起点
					}else{
					PORTB=0x01;
				}	
			}
		}
		if(K2_DOWN())
		{
			_delay_us(2000);
			if(K2_DOWN()&& !K6_UP()){
					INT8U j;
					if(K3_DOWN()){
						for(j=0;j<STEP();j++)//走对应的是step[Index]步
						{
							PORTB=AAW[3-(j%4)];
							_delay_ms(500);
						}
						//PORTB=0x01;//最后一圈之后输出0x01这一拍电机回到起点
						}else if(K4_DOWN()){
						for(j=0;j<STEP();j++)//走对应的是step[Index]步
						{
							PORTB=BBW[3-(j%4)];
							_delay_ms(500);
						}
						//PORTB=0x01;//最后一圈之后输出0x01这一拍电机回到起点
						}else if(K5_DOWN()){
						for(j=0;j<STEP();j++)//走对应的是step[Index]步
						{
							PORTB=ABW[7-(j%8)];
							_delay_ms(500);
						}
						//PORTB=0x01;//最后一圈之后输出0x01这一拍电机回到起点
						}else{
						PORTB=0x01;
					}
			}
		}
		_delay_ms(3000);
	}
}


