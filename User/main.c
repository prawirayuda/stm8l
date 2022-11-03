/**
  ******************************************************************************
  * @file    main.c
  * @author  Muhammad Prawira Yuda
  * @version V1.0
  * @date    25-October-2022
  * @brief   Main program body
  ******************************************************************************

  GPIOx->DDR |=  For unused pin 
  GPIOx->ODR |=  Set Value high or low 
  Testing for GPIO hardreset done,
  
  
  
  ******************************************************************************
  */

#include "stm8l10x.h"


#ifdef _COSMIC_
  #define ASM  _asm
#endif
#ifdef EWSTM8_IAR
  #define ASM  asm
#endif

/* This delay should be added just after reset to have access to SWIM pin and to be
   able to reprogram the device after power on (otherwise the device will be locked) */
#define STARTUP_SWIM_DELAY_5S      \
{                                  \
  ASM("       PUSHW X        \n"   \
      "       PUSH  A        \n"   \
      "       LDW X, #0xFFFF \n"   \
      "loop1: LD  A, #50     \n"   \
                                   \
      "loop2: DEC A          \n"   \
      "       JRNE loop2     \n"   \
                                   \
      "       DECW X         \n"   \
      "       JRNE loop1     \n"   \
                                   \
      "       POP  A         \n"   \
      "       POPW X           "); \
}


static void TIM2_Config(void);
void delay(uint32_t miliseconds);

//Variable global 
volatile uint32_t u16_counter_ms = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define LED_GPIO_PORT  GPIOB
#define LED_GPIO_PINS  GPIO_Pin_6  // for HARDRESET

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void USART_Config(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Initialize LEDs mounted on STM8L101-EVAL board */
  STARTUP_SWIM_DELAY_5S;
    //Configure the clock 
  CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1); // 16MHz
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  
  
  //inisialisasi pin
  GPIO_Init(LED_GPIO_PORT, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Fast); // HARDRESET
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast); //PB5/ FOR WDT 
  // GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fas);          //PB7/SPI_MISO <<<<==== BUTTON

  /* -- Pin 1 --------------------------------------------------------------- */
//  GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PA0/SWIM/BEEP/IR_TIM
//  GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast); //PC3/USART_TX
//  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PC4/USART_CK/CCO
  /* -- Pin 2 --------------------------------------------------------------- */
//  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PA2
//  GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PA4/TIM2_BKIN
//  GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PA6/COMP_REF
  /* -- Pin 5 --------------------------------------------------------------- */
//  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);   //PB3/TIM2_ETR/COMP2_CH2 <<<<==== LED
 //GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PD0/TIM3_CH2/COMP1_CH3
  /* -- Pin 6 --------------------------------------------------------------- */
 //GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PB6/SPI_MOSI 
  /* -- Pin 7 --------------------------------------------------------------- */
  //GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);          //PB7/SPI_MISO <<<<==== Test GPIO OUTPUT 
//  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PC0/I2C_SDA
  /* -- Pin 8 --------------------------------------------------------------- */
// GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast); //PC1/I2C_SCL  RX 
//  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PC2/USART_RX
  /* ------------------------------------------------------------------------ */

  TIM2_Config();
  USART_Config();
  enableInterrupts();


  while (1)
  {
   GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PINS);
   delay(3600000); // 1HOURS
   GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PINS);
   delay(1800000); // 30 MINUTES
  }
}


/**
  * @brief  TIM2_Config
  * @param  void                          
  * @retval None 
  *   */
static void TIM2_Config(void)
{
  //Configure frequency tim2 = 1000 Hz
  uint16_t Period = 999;
  
  //Init TIM2
  TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, Period);
  
  //Enable interrupt overflow("UPDATE")
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  
  //Activate peripheral TIM2
  TIM2_Cmd(ENABLE);
  
  //enable event generation
  TIM2->EGR |= 0x0001;
  
  //clear flag tim2
  TIM2_ClearFlag(TIM2_FLAG_Update);
}



/**
  * @brief  delay
  * @param  miliseconds                          
  * @retval None
  *   */

 void delay(uint32_t miliseconds)
 {
  u16_counter_ms = 0;
  while(u16_counter_ms < miliseconds);
 }


/**
  * @brief  Configure USART peripheral
  * @param  None
  * @retval None
  */

static void USART_Config(void)
{
    /*High speed internal clock prescaler: 1*/
    CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);
    
    /*Set the USART RX and USART TX at high level*/
    GPIO_ExternalPullUpConfig(GPIOC,GPIO_Pin_3|GPIO_Pin_4, ENABLE);
    
    /* Enable USART clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_USART, ENABLE);
    
    USART_DeInit();
    /* USART configuration ------------------------------------------------------*/
    /* USART configured as follow:
          - BaudRate = 115200 baud  
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Receive and transmit enabled
    */
    USART_Init((uint32_t)115200, USART_WordLength_8D, USART_StopBits_1,
               USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));


    
    /* Enable the USART Transmit interrupt: this interrupt is generated when the
       USART transmit data register is empty */
    USART_ITConfig(USART_IT_TXE, ENABLE);
    /* Enable the USART Receive interrupt: this interrupt is generated when the
       USART receive data register is not empty */
    USART_ITConfig(USART_IT_RXNE, ENABLE);

    /* Enable general interrupts */
    enableInterrupts();
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
