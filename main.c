/*
	Title			 : CRC
	Author		 : mbedsyst / Zalman Ul Fariz P A
	Description: Configuring CRC Peripheral to create a checksum for 
							 each input data and print on a serial terminal.
*/

#include <stm32f401xe.h>
#include <stdio.h>

void UART_Init(void);
void UART_Tx(char data);
void UART_Tx_String(char *str);

void CRC_Init(void);
void CRC_DeInit(void);
uint32_t CRC_Word(const uint32_t *data);
uint32_t CRC_Block(const uint32_t *data, uint32_t len);

void UART_Init(void)
{
  RCC->AHB1ENR |= 1<<0;
  RCC->APB1ENR |= 1<<17;

  GPIOA->MODER &= ~((1U<<4) |(1<<6));
  GPIOA->MODER |=  (1<<5) | (1<<7);
  GPIOA->AFR[0] |= (0x07<<8) | (0x07<<12);

  USART2->BRR = 0x008B;  
  USART2->CR1 |= (1<<13) | (1<<3) | (1<<2);
}

void UART_Tx(char data)
{
	
	while (!(USART2->SR & 1<<7)){}
  USART2->DR = data;
}

void UART_Tx_String(char *str)
{
	while(*str)
	{
		UART_Tx(*str++);
	}
}

void CRC_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
	CRC->CR |= CRC_CR_RESET;
}
void CRC_DeInit(void)
{
	RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;
}

uint32_t CRC_Word(const uint32_t *data)
{
	CRC->CR |= CRC_CR_RESET;
	CRC->DR = *data;
	return CRC->DR;
}

uint32_t CRC_Block(const uint32_t *data, uint32_t len)
{
	CRC->CR |= CRC_CR_RESET;
	while(len--)
	{
		CRC->DR = *data++;
	}
	return CRC->DR;
}

int main(void)
{
	uint32_t CRC_Code;
	char buf[100];
	const uint32_t testData[] = {0x12345678, 0x9ABCDEF0, 0xFEDCBA98};
	
	UART_Init();
	CRC_Init();
	
	CRC_Code = CRC_Word(&testData[0]);
	sprintf(buf,"\t\t CRC Word-wise: 0x%08X\n", CRC_Code);
	UART_Tx_String(buf);
	
	CRC_Code = CRC_Block(testData, sizeof(testData) / sizeof(testData[0]));
	sprintf(buf,"\t\t CRC Block-wise: 0x%08X\n", CRC_Code);
	UART_Tx_String(buf);

	CRC_DeInit();	
	
	while(1)
	{
		
	}
}
