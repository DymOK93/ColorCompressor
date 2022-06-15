#include "command.h"
#include "transmitter.h"

#include "stm32f0xx.h"

void CmdInit(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR9_1;
}

void CmdExecute(uint8_t command)
{
	switch (command)
	{
	case CMD_GREEN_LED_ON:
		GPIOC->ODR |= GPIO_ODR_8;
		break;
	case CMD_GREEN_LED_OFF:
		GPIOC->ODR &= ~GPIO_ODR_8;
		break;
	case CMD_GREEN_LED_TOGGLE:
		GPIOC->ODR ^= GPIO_ODR_8;
		break;
	case CMD_BLUE_LED_ON:
		GPIOC->ODR |= GPIO_ODR_9;
		break;
	case CMD_BLUE_LED_OFF:
		GPIOC->ODR &= ~GPIO_ODR_9;
		break;
	case CMD_BLUE_LED_TOGGLE:
		GPIOC->ODR ^= GPIO_ODR_9;
		break;
	default:
		break;
	}
}

void CmdListen(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI->RTSR |= EXTI_RTSR_TR0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	NVIC_SetPriority(EXTI0_1_IRQn, 3);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler(void)
{
	TrmSendCommand(CMD_NEXT_PICTURE);
	EXTI->PR |= EXTI_PR_PR0;
}
