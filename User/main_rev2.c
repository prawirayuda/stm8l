/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l10x.h"

#ifdef _RAISONANCE_
 #define PUTCHAR_PROTOTYPE int putchar (char c)
 #define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
 #define PUTCHAR_PROTOTYPE char putchar (char c)
 #define GETCHAR_PROTOTYPE char getchar (void)
#else /* _IAR_ */
 #define PUTCHAR_PROTOTYPE int putchar (int c)
 #define GETCHAR_PROTOTYPE int getchar (void)
 #define ASM asm
#endif


#define STARTUP_SWIM_DELAY_5S \
 { \
 ASM(" PUSHW X \n" \
 " PUSH A \n" \
 " LDW X, #0xFFFF \n" \
 "loop1: LD A, #50 \n" \
 \
 "loop2: DEC A \n" \
 " JRNE loop2 \n" \
 \
 " DECW X \n" \
 " JRNE loop1 \n" \
 \
 " POP A \n" \
 " POPW X " );\
 }
/**
  * @addtogroup USART_HyperTerminalInterrupt
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
/* Private functions ---------------------------------------------------------*/
static void TIM2_Config(void);
void delay(uint32_t miliseconds);

//Variable global 
volatile uint32_t u16_counter_ms = 0;




/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  STARTUP_SWIM_DELAY_5S;
  /* USART Configuration -----------------------------------------------------*/
  USART_Config();
  USART_Cmd(ENABLE);
  //CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1); // 16MHz
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PB6/SPI_MOSI 
  TIM2_Config();    
  enableInterrupts();

  GPIO_SetBits(GPIOB, GPIO_Pin_6);

  while (1)
  {
    // delay(1000);
    // uint8_t temp;
    // temp = (USART_ReceiveData8() & 0x7F);
    // USART_SendData8(temp);
  }
  
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
          - BaudRate = 9600 baud  
          - Word Length = 8 Bits
          - One Stop Bit
          - Odd parity
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



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
