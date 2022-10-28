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
  
  ******************************************************************************
  */

#include "stm8l10x.h"

#ifdef _COSMIC_
  #define ASM _asm
#endif
#ifdef EWSTM8_IAR
  #define ASM asm 
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
volatile unsigned char u8_button = 0;

main()
{
  STARTUP_SWIM_DELAY_5S;

  //Configure the clock 
  CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1); // 16MHz
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);


  //inisialisasi pin
  /* -- Pin 1 --------------------------------------------------------------- */
//  GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PA0/SWIM/BEEP/IR_TIM
//  GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast); //PC3/USART_TX
//  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PC4/USART_CK/CCO
  /* -- Pin 2 --------------------------------------------------------------- */
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PA2
//  GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PA4/TIM2_BKIN
//  GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PA6/COMP_REF
  /* -- Pin 5 --------------------------------------------------------------- */
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);   //PB3/TIM2_ETR/COMP2_CH2 <<<<==== LED
//  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast); //PB5/SPI_SCK
//  GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PD0/TIM3_CH2/COMP1_CH3
  /* -- Pin 6 --------------------------------------------------------------- */
 GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PB6/SPI_MOSI 
  /* -- Pin 7 --------------------------------------------------------------- */
 GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_IT);          //PB7/SPI_MISO <<<<==== BUTTON
  //GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);          //PB7/SPI_MISO <<<<==== Test GPIO OUTPUT 
//  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PC0/I2C_SDA
  /* -- Pin 8 --------------------------------------------------------------- */
 GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast); //PC1/I2C_SCL  RX 
//  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PC2/USART_RX
  /* ------------------------------------------------------------------------ */

  GPIO_ExternalPullUpConfig(GPIOC,GPIO_Pin_3|GPIO_Pin_4, ENABLE);  
  
  EXTI_SetPinSensitivity(EXTI_Pin_7, EXTI_Trigger_Rising);  //interupt external pin7
  TIM2_Config();
  enableInterrupts();

  
  GPIO_ResetBits(GPIOB, GPIO_Pin_3);

  while(1)
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
    delay(10800000);
    GPIO_ResetBits(GPIOB, GPIO_Pin_3); 
    delay(1800000);


    // switch(u8_button)
    // {
    //   case 0 : 

    //     GPIO_SetBits(GPIOB, GPIO_Pin_3);
    //     delay(100);
    //     GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    //     delay(100);
    //     break;
      
    //   case 1 : 
    //     GPIO_WriteBit(GPIOB, GPIO_Pin_3, SET);
    //     delay(200);
    //     GPIO_WriteBit(GPIOB, GPIO_Pin_3, RESET);
    //     delay(3600); //600.000 seconds TEST 30 MENIT OK  NOW TEST 1 HOUR 

    //     //test 3600 seconds result = 3640 seconds
        
    //     break;

    //   default :
    //     break;
      
    // }
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
