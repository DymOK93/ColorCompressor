#include "transmitter.h"
#include "packet.h"

#include <string.h>

#include "stm32f0xx.h"


typedef struct
{
	volatile uint8_t active_transfer;
	unsigned char request[PACKET_MAX_SIZE + 1];
} Transmitter;

static Transmitter g_transmitter = { 0 };

static void TrmpWaitForTransferComplete(void)
{
	while (g_transmitter.active_transfer);
}

static void TrmpStartTransfer(void)
{
	g_transmitter.active_transfer = 1;
}

static void TrmpFinalizeTransfer(void)
{
	g_transmitter.active_transfer = 0;
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;
}

static void TrmpDmaSend(const void* buffer, uint16_t bytes_count)
{
	DMA1_Channel2->CMAR = (uint32_t)buffer;
	DMA1_Channel2->CNDTR = bytes_count;
	DMA1_Channel2->CCR |= DMA_CCR_EN;
}

static void TrmpSendDataChunk(const void* buffer, uint16_t bytes_count)
{
	// bytes_count <= PACKET_MAX_SIZE

	TrmpWaitForTransferComplete();
	g_transmitter.request[0] = (unsigned char)PACKET_HEADER(PACKET_TYPE_DATA, bytes_count);
	memcpy(g_transmitter.request + 1, buffer, bytes_count);

	TrmpStartTransfer();
	TrmpDmaSend(buffer, bytes_count);
}

void TrmInit(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT_9;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0;
	GPIOA->AFR[1] |= 0x00000110;

	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_DIR |
		DMA_CCR_TEIE | DMA_CCR_TCIE; // Transfer Error & Complete interrupts
	DMA1_Channel2->CPAR = (uint32_t)&USART1->TDR;
	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR |= 69;  // 115200
	USART1->CR3 |= USART_CR3_DMAT;
	USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void TrmSendData(const void* buffer, uint16_t bytes_count)
{
	while (bytes_count >= PACKET_MAX_SIZE)
	{
		TrmpSendDataChunk(buffer, PACKET_MAX_SIZE);
		buffer = (const unsigned char*)buffer + PACKET_MAX_SIZE; // (const void* + N) is illegal => cast to const unsigned char*
		bytes_count -= PACKET_MAX_SIZE;
	}
	if (bytes_count > 0) // 0 < bytes_count < PACKET_MAX_SIZE
	{
		TrmpSendDataChunk(buffer, bytes_count);
	}
}

void TrmSendCommand(uint8_t command)
{
	TrmpWaitForTransferComplete();
	g_transmitter.request[0] = (unsigned char)PACKET_HEADER(PACKET_TYPE_COMMAND, 1);
	g_transmitter.request[1] = command;

	TrmpStartTransfer();
	TrmpDmaSend(g_transmitter.request, 2);
}

void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void)
{
	DMA1->IFCR = DMA_IFCR_CTEIF2 | DMA_IFCR_CTCIF2;
	TrmpFinalizeTransfer();
}

