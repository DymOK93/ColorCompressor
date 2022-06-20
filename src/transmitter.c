#include "transmitter.h"
#include "packet.h"

#include <stm32f0xx.h>

#include <stdint.h>
#include <string.h>

typedef struct {
  volatile uint8_t active_transfer;
  unsigned char request[PACKET_MAX_SIZE + 1];
} Transmitter;

#define TRM_REQUEST_HEADER(trm) ((trm)->request[0])
#define TRM_REQUEST_BODY(trm) ((trm)->request + 1)

static Transmitter g_transmitter = {0};

static int TrmpCompareExchangeFlag(volatile uint8_t* flag,
                                   uint8_t expected,
                                   uint8_t desired) {
  uint8_t value;

  __disable_irq();
  value = *flag;
  if (value == expected) {
    *flag = desired;
  }
  __enable_irq();
  return value;
}

static void TrmpRestartTransfer(void) {
  for (;;) {
    while (g_transmitter.active_transfer != 0)
      ;
    if (TrmpCompareExchangeFlag(&g_transmitter.active_transfer, 0, 1) == 0) {
      break;
    }
  }
}

static void TrmpFinalizeTransfer(void) {
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  g_transmitter.active_transfer = 0;
}

static void TrmpDmaRequest(uint16_t bytes_count) {
  DMA1_Channel2->CNDTR = bytes_count + 1u;  // + size of packet header
  DMA1_Channel2->CCR |= DMA_CCR_EN;
}

static void TrmpSendDataChunk(const void* buffer, uint16_t bytes_count) {
  // bytes_count <= PACKET_MAX_SIZE
  TrmpRestartTransfer();
  TRM_REQUEST_HEADER(&g_transmitter) =
      (unsigned char)PACKET_HEADER(PACKET_TYPE_DATA, bytes_count);
  memcpy(TRM_REQUEST_BODY(&g_transmitter), buffer, bytes_count);
  TrmpDmaRequest(bytes_count);
}

void TrmInit(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
  GPIOA->OTYPER |= GPIO_OTYPER_OT_9;
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0;
  GPIOA->AFR[1] |= 0x00000110;

  RCC->AHBENR |= RCC_AHBENR_DMAEN;
  DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TEIE |
                        DMA_CCR_TCIE;  // Transfer Error & Complete interrupts
  DMA1_Channel2->CPAR = (uint32_t)&USART1->TDR;
  DMA1_Channel2->CMAR = (uint32_t)g_transmitter.request;

  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  USART1->BRR |= 69;  // 115200
  USART1->CR3 |= USART_CR3_DMAT;
  USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void TrmSendData(const void* buffer, uint16_t bytes_count) {
  while (bytes_count >= PACKET_MAX_SIZE) {
    TrmpSendDataChunk(buffer, PACKET_MAX_SIZE);
    buffer = (const unsigned char*)buffer +
             PACKET_MAX_SIZE;  // (const void* + N) is illegal => cast to const
                               // unsigned char*
    bytes_count -= PACKET_MAX_SIZE;
  }
  if (bytes_count > 0)  // 0 < bytes_count < PACKET_MAX_SIZE
  {
    TrmpSendDataChunk(buffer, bytes_count);
  }
}

void TrmSendCommand(uint8_t command) {
  TrmpRestartTransfer();
  TRM_REQUEST_HEADER(&g_transmitter) =
      (unsigned char)PACKET_HEADER(PACKET_TYPE_COMMAND, 1);
  *TRM_REQUEST_BODY(&g_transmitter) = command;
  TrmpDmaRequest(1);
}

void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void) {
  DMA1->IFCR = DMA_IFCR_CTEIF2 | DMA_IFCR_CTCIF2;
  TrmpFinalizeTransfer();
}
