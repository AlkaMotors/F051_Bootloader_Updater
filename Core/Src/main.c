/* Bootloader Updater for open esc bootloader.
 *
 * This program is to be uploaded to memory address 0x1000 the new target bootloader is to
 * be uploaded to 0x2000.
 * Upon running this will copy 4Kb from 0x2000 to base memory adddress 0x0000 and set the boot
 * bit in the eeprom to 0 ( so the program will not run again ).
 * If will reboot upon completion
 * --alka*/
#include "main.h"

#include <string.h>

#define page_size 0x400                   // 1 kb for f051
uint32_t FLASH_FKEY1 =0x45670123;
uint32_t FLASH_FKEY2 =0xCDEF89AB;
uint8_t rxBuffer[258];
#define APPLICATION_ADDRESS 0x08001000
#define EEPROM_START_ADD  (uint32_t)0x08007C00

void initAfterJump(){
    volatile uint32_t *VectorTable = (volatile uint32_t *)0x20000000;
    uint32_t vector_index = 0;
    for(vector_index  = 0; vector_index  < 48; vector_index++)
    {
      VectorTable[vector_index ] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (vector_index <<2));         // no VTOR on cortex-MO so need to copy vector table
    }
  //	  /* Enable the SYSCFG peripheral clock*/
    do {
       volatile uint32_t tmpreg;
       ((((RCC_TypeDef *) ((((uint32_t)0x40000000U) + 0x00020000) + 0x00001000))->APB2ENR) |= ((0x1U << (0U))));
           /* Delay after an RCC peripheral clock enabling */
      tmpreg = ((((RCC_TypeDef *) ((((uint32_t)0x40000000U) + 0x00020000) + 0x00001000))->APB2ENR) & ((0x1U << (0U))));
         ((void)(tmpreg));
          } while(0U);
  //	  /* Remap SRAM at 0x00000000 */
    do {((SYSCFG_TypeDef *) (((uint32_t)0x40000000U) + 0x00010000))->CFGR1 &= ~((0x3U << (0U)));
        ((SYSCFG_TypeDef *) (((uint32_t)0x40000000U) + 0x00010000))->CFGR1 |= ((0x1U << (0U)) | (0x2U << (0U)));
      }while(0);

    if (SysTick_Config(SystemCoreClock / 1000))
     {
       /* Capture error */
       while (1)
       {
       }
     }
    __enable_irq();
}

void save_flash_nolib(uint8_t *data, int length, uint32_t add){
	uint16_t data_to_FLASH[length / 2];
	memset(data_to_FLASH, 0, length / 2);
	for(int i = 0; i < length / 2 ; i ++ ){
		data_to_FLASH[i] =  data[i*2+1] << 8 | data[i*2];   // make 16 bit
	}
	volatile uint32_t data_length = length / 2;

	// unlock flash

	while ((FLASH->SR & FLASH_SR_BSY) != 0) {
	/*  add time-out*/
	}
	if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
	FLASH->KEYR = FLASH_FKEY1;
	FLASH->KEYR = FLASH_FKEY2;
	}

	// erase page if address even divisable by 1024
	 if((add % 1024) == 0){


	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = add;
	FLASH->CR |= FLASH_CR_STRT;
	while ((FLASH->SR & FLASH_SR_BSY) != 0){
	/*  add time-out */
	}
	if ((FLASH->SR & FLASH_SR_EOP) != 0){
	FLASH->SR = FLASH_SR_EOP;
	}
	else{
	/* error */
	}
	FLASH->CR &= ~FLASH_CR_PER;

	 }

	 volatile uint32_t write_cnt=0, index=0;
	 while(index < data_length)
			  {

	    	FLASH->CR |= FLASH_CR_PG; /* (1) */
	    	*(__IO uint16_t*)(add+write_cnt) = data_to_FLASH[index];
	    	while ((FLASH->SR & FLASH_SR_BSY) != 0){ /*  add time-out  */
	    	}
	   	 if ((FLASH->SR & FLASH_SR_EOP) != 0){
	   	 FLASH->SR = FLASH_SR_EOP;
	   	 }
	   	 else{
	   	 /*  error  */
	   	 }
	   	 FLASH->CR &= ~FLASH_CR_PG;
				  write_cnt += 2;
				  index++;
		  }
	 SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}




void read_flash_bin(uint8_t*  data , uint32_t add , int out_buff_len){
	//volatile uint32_t read_data;
	for (int i = 0; i < out_buff_len ; i ++){
		data[i] = *(uint8_t*)(add + i);
	}
}

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  initAfterJump();
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  FLASH->ACR |= FLASH_ACR_PRFTBE;   // prefetch buffer enable
  SystemClock_Config();
  MX_GPIO_Init();

  for(int i = 0; i < 4096; i+=256){
	  read_flash_bin(rxBuffer , 0x08002000 + i, 256);

	  save_flash_nolib(rxBuffer, 256, 0x08000000 + i);
  }

  read_flash_bin(rxBuffer , 0x08007C00, 48);
  rxBuffer[0] = 0;     // set boot bit to zero
  save_flash_nolib(rxBuffer, 256, 0x08007C00);

  NVIC_SystemReset();

  while (1)
  {

  }

}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(48000000);
  LL_SetSystemCoreClock(48000000);
}

static void MX_GPIO_Init(void)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
}

void Error_Handler(void)
{

}

