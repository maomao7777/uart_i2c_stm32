/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
#include "string.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer))
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
__IO uint32_t     Transfer_Direction = 0;
__IO uint32_t     Xfer_Complete = 0;
_Bool cmd_cplt=false;
_Bool I2C_cmd_cplt=false;
uint8_t rx_buff[50],cmd_buff[50],tmp_buff[60];
/* Buffer used for transmission */
uint8_t aTxBuffer[4];
/* Buffer used for reception */
uint8_t aRxBuffer[4];
#define RxBuf_SIZE 20
uint8_t RxBuf[RxBuf_SIZE];
uint8_t getMasterInput = 0;
uint8_t getTransferDirection = 0;
uint8_t i2c_cmdline[6];
uint8_t i2c_rmsg[3];// response String for command
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  Retargets the C library printf function to the USART.
  *   None
  * @retval None
*/
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
_Bool exec_i2c_cmd(uint8_t *cmd_buff)
{
	_Bool rslt=true;
	//float val;
	//uint16_t errorCode=0;
	i2c_rmsg[0]=0x7;
	i2c_rmsg[1]=0x7;
	i2c_rmsg[2]=0x7;
    if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t*)i2c_rmsg, sizeof(i2c_rmsg),1000)==HAL_OK)
    {
        // printf("Status code %02x,%02x%02x  OK\r\n",res_msg[0],res_msg[1],res_msg[2]);
    }
    else
    {
        printf("Send Response fail!\r\n");
    }

    return rslt;
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size)
{
//	uint16_t i;
//    if (huart->Instance==USART1)
//    {
//        for (i=0;i<Size;i++)
//        {
//            if ( (RxBuf[i]==0x0D) || (RxBuf[i]==0x0A) )
//            {
//                rx_buff[rx_idx]=0;
//                //HAL_UART_Transmit(&huart2,"\r\n",2,5); //if received CR(0x0D), then send 0x0A
//                cmd_cplt=true;
//                break;
//            }
//            else if  (RxBuf[i]!=0x0A)
//            {
//                rx_buff[rx_idx++]=RxBuf[i];
//                //HAL_UART_Transmit(&huart2,&rx_buff[i],1,HAL_MAX_DELAY);
//            }
//
//        }
//        if (cmd_cplt)
//        {
//            RxBuf[i]=0x0D;
//            RxBuf[i+1]=0x0A;
//            // RxBuf[Size-1]=0x0D;
//            // RxBuf[Size]=0x0A;
//            HAL_UART_Transmit(&huart2,RxBuf,i+2,HAL_MAX_DELAY);
//            // HAL_UART_Transmit(&huart2,RxBuf,Size+1,HAL_MAX_DELAY);
//        }
//        else
//        {
//            HAL_UART_Transmit(&huart2,RxBuf,Size,HAL_MAX_DELAY);
//        }
//        //memcpy(MainBuf,RxBuf,Size);
//        HAL_UARTEx_ReceiveToIdle_DMA(&huart2,RxBuf,RxBuf_SIZE);
//        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx,DMA_IT_HT);
//        //HAL_UART_Transmit(&huart2,MainBuf,Size,10);
//    }
    if(huart->Instance==USART1)
    {

    	 HAL_UART_Transmit(&huart1,RxBuf,RxBuf_SIZE,HAL_MAX_DELAY);
    	 HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RxBuf,RxBuf_SIZE);
    	 __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
    }

}
/**
  * @brief  Tx Transfer completed callback.
  *   I2cHandle: I2C handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
*/
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	printf("cbak tx  buffer 0x%02x 0x%02x 0x%02x 0x%02x \r\n",
			aTxBuffer[0],
			aTxBuffer[1],
			aTxBuffer[2],
			aTxBuffer[3]);
	Xfer_Complete = 1;
	aTxBuffer[0]++;
	aTxBuffer[1]++;
	aTxBuffer[2]++;
	aTxBuffer[3]++;

}
/**
  * @brief  Rx Transfer completed callback.
  *   I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
*/
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
//	printf("rcv rx buffer 0x%02x 0x%02x 0x%02x 0x%02x \r\n",
//			aRxBuffer[0],
//			aRxBuffer[1],
//			aRxBuffer[2],
//			aRxBuffer[3]);
//	Xfer_Complete = 1;
//	aRxBuffer[0]=0x00;
//	aRxBuffer[1]=0x00;
//	aRxBuffer[2]=0x00;
//	aRxBuffer[3]=0x00;
	printf("rcv rx buffer 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \r\n",
			i2c_cmdline[0],
			i2c_cmdline[1],
			i2c_cmdline[2],
			i2c_cmdline[3],
			i2c_cmdline[4],
			i2c_cmdline[5]);
	I2C_cmd_cplt=true;
}

/**
  * @brief  Slave Address Match callback.
  *   hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  *   TransferDirection: Master request Transfer Direction (Write/Read), value of @ref I2C_XferOptions_definition
  *   AddrMatchCode: Address Match Code
  * @retval None
*/
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
//	Transfer_Direction = TransferDirection;
//	printf("_AddrCallback dir %d \r\n",TransferDirection);
//	if (Transfer_Direction != 0)
//	{
//		/*##- Start the transmission process #####################################*/
//		/* While the I2C in reception process, user can transmit data through
//	 	 "aTxBuffer" buffer */
//		if (HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, (uint8_t *)aTxBuffer, TXBUFFERSIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
//		{
//			/* Transfer error in transmission process */
//			printf("HAL_I2C_Slave_Seq_Transmit_IT set err \r\n");
//			Error_Handler();
//		}
//		printf("HAL_I2C_Slave_Seq_Transmit_IT was set \r\n");
//	}
//	else
//	{
//
//		/*##- Put I2C peripheral in reception process ###########################*/
//		if (HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, (uint8_t *)aRxBuffer, RXBUFFERSIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
//		{
//			/* Transfer error in reception process */
//			Error_Handler();
//			printf("HAL_I2C_Slave_Seq_Receive_IT set err \r\n");
//		}
//		printf("HAL_I2C_Slave_Seq_Receive_IT was set \r\n");
//
//	}
//	getMasterInput = 1;
//	getTransferDirection = TransferDirection;
}

/**
  * @brief  Listen Complete callback.
  *   hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
*/
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	printf("ListenCpltCallback\r\n");
	Xfer_Complete = 1;//123321

}

/**
  * @brief  I2C error callbacks.
  *   I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
*/

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
	/** Error_Handler() function is called when error occurs.
	* 1- When Slave doesn't acknowledge its address, Master restarts communication.
	* 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
	*/
	printf("I2C_ErrorCallback..\r\n");
	if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
	{
		printf("I2C rub Error_hdl\r\n");
		Error_Handler();
	}
	//restart i2c irq
	if(HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)i2c_cmdline, sizeof(i2c_cmdline)) != HAL_OK)  //I2C Slave Read
	{
	  // Transfer error in reception process
	  printf("Slave Received_IT Fail\r\n");
	  Error_Handler();
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USB_PCD_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //printf("test i2c1 init down \r\n");
//  if(HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK)
//  {
//      /* Transfer error in reception process */
//      Error_Handler();
//  }
//  else
//  {
//	  printf("i2c1 _EnableListen_IT \r\n");
//  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_Delay(500);
  HAL_UART_Transmit(&huart1,(uint8_t *)"OK>\r\n",3,100);
  //UART DMA Receive
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RxBuf,sizeof(RxBuf));
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
  // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,PIN_LOW); //test pin of I2C exec time

  //HAL_UART_Receive_IT(&huart2,&rx_char,sizeof(rx_char));
  if(HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)i2c_cmdline, sizeof(i2c_cmdline)) != HAL_OK)  //I2C Slave Read
  {
	  /* Transfer error in reception process */
	  printf("Slave Received_IT Fail\r\n");
	  //errFlag=1;
	  Error_Handler();
  }
  while (1)
  {
	  if (cmd_cplt)
	  {
		  //__HAL_UART_DISABLE_IT(&huart2,UART_IT_RXNE);
		  strcpy((char *)cmd_buff,(char *)rx_buff);
		  //exec_uart_cmd(cmd_buff);
		  //rx_idx=0;
		  cmd_cplt=false;
		  // printf(cmd_buff,"\n%s\r\n",cmd_buff);
		  //__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	  }
	  if (I2C_cmd_cplt)
	  {
		  exec_i2c_cmd(i2c_cmdline);
		  //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,PIN_LOW);

		  I2C_cmd_cplt=false;
		  if(HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)i2c_cmdline, sizeof(i2c_cmdline)) != HAL_OK)  //I2C Slave Read
		  {
			  // Transfer error in reception process
			  printf("Slave Received_IT Fail\r\n");
			  //errFlag=2;
			  Error_Handler();
		  }
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 36;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
