#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "tube.h"

#define MAN 10000
#define DELTA (MAN/20)

int raspberry(){
	return 1;
}	


int read_cube(){
	static int status[12];
	int amount = 0;
	for (int i = 0; i<12; i++) {
		status[i] = !GPIO_ReadInputDataBit(GPIOC,(u16)1<<i);
	}
	//提示点
	static enum {normal, wait_raspi, raspi_left, raspi_right} ss = normal;
	static int times;
	
	//左转
	if (ss == raspi_left){
		for (int i = 0; i<5; i++) {
			amount += (i-5) * status[i+2];
		} // 左边五个 加权
		for (int i = 0; i<2; i++) {
			amount += (i+1) * status[i+7];
		} // 右边2个 加权
		return amount;
	}	
	
	//右转
	if (ss == raspi_right){
		for (int i = 3; i<5; i++) {
			amount += (i-5) * status[i+2];
		} // 左边2个 加权
		for (int i = 0; i<5; i++) {
			amount += (i+1) * status[i+7];
		} // 右边五个 加权
		return amount;
	}
	
	//正常情况
	if (ss == normal) {
		if (status[0]) {
			// sent signal to raspi
			ss = wait_raspi;
		}
		
		for (int i = 0; i<5; i++) {
			amount += (i-5) * status[i+2];
		} // 左边五个 加权
		for (int i = 0; i<5; i++) {
			amount += (i+1) * status[i+7];
		} // 右边五个 加权
		return amount;
	}
	
	if (ss == wait_raspi) {
		if (1) // check raspi
		{
			// read dir
			ss = raspi_left;
		}
		return 0;
//		for (int i = 0; i<5; i++) {
//			amount += (i-5) * status[i+2];
//		} // 左边五个 加权
//		for (int i = 0; i<5; i++) {
//			amount += (i+1) * status[i+7];
//		} // 右边五个 加权
//		return amount;
	}
}

void change_motor(int amount){
	if (amount == 0){
		TIM1->CCR2 = MAN/2;
		TIM1->CCR3 = MAN/2;
	}
    else{
		int abs = amount, sign=1;
		if (abs<0) abs=-abs, sign=-1;
		for (int i = 0; i<abs; i++) {
			if (TIM1->CCR2 && TIM1->CCR2 < MAN) TIM1->CCR2 += DELTA*sign;
			if (TIM1->CCR3 && TIM1->CCR3 < MAN) TIM1->CCR3 -= DELTA*sign;
		}
	}	
	return;
}	

int main(void)
{	
	delay_init(99); 			//初始化延时函数
	TUBE_Init(); 				//初始化光电管
	TIM1_PWM_Init(MAN,100-1);	//初始化PWM
	change_motor(0);		
	int amount;
    
	
	while(1){
	  amount = read_cube();
	  change_motor(amount);		
	}
}	