#include "receiver.h"

#include "stm32f0xx.h"

typedef struct
{
	CircularBuffer* cb;
	int flags;
} Receiver;

static Receiver g_receiver;

static void RcvpReply(uint8_t overwrite)
{
	if (!overwrite)
	{
		GPIOB->ODR |= GPIO_ODR_9;   // CTS signal
	}
	else
	{
		GPIOB->ODR |= GPIO_ODR_10;  // Overwrite signal
	}
	EXTI->PR |= EXTI_PR_PR8;  // Clear before setting CTS/OV
}

void RcvInit(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1
		| GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1
		| GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR9_1 | GPIO_PUPDR_PUPDR10_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1 | GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3
		| GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7
		| GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10;
}

void RcvListen(CircularBuffer* cb, int flags)
{
	g_receiver.cb = cb;
	g_receiver.flags = flags;

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	EXTI->IMR |= EXTI_IMR_MR8;
	EXTI->RTSR |= EXTI_RTSR_TR8;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PB;
	NVIC_SetPriority(EXTI4_15_IRQn, 2);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void)
{
	CircularBuffer* cb = g_receiver.cb;

	GPIOB->ODR &= ~(GPIO_ODR_9 | GPIO_ODR_10);
	RcvpReply(!CbProduceOne(cb, (uint8_t)GPIOB->IDR) && g_receiver.flags & RCV_FLAG_RETRY_IF_OVERWRITE);
}
