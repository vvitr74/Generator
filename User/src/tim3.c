#include "stm32g0xx.h"
#include "tim3.h"

volatile uint32_t tim3TickCounter;
extern volatile uint32_t playClk;

void tim3Init(void)
{
	RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
	TIM3->PSC = 63;												//sets TIM3 clk 1 MHz
	TIM3->ARR = 1000;											//sets TIM3 count interval 1 ms
	TIM3->DIER = TIM_DIER_UIE;
	NVIC_SetPriority(TIM3_IRQn,15);
	NVIC_EnableIRQ(TIM3_IRQn);
	TIM3->CR1 = TIM_CR1_CEN;
}

void delay_ms(uint32_t delayTime){
	tim3TickCounter = delayTime;
	while(tim3TickCounter){}
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_SR_UIF){
		TIM3->SR = ~TIM_SR_UIF;
		tim3TickCounter--;
		playClk++;
	}
}

