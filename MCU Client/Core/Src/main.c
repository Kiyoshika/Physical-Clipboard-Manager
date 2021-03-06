/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "flash_memory.h"
#include "addresses.h"
#include <string.h>
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static bool send_flash_buffer_to_server(uint32_t metadata_address, uint32_t begin_address);
static bool send_command_to_server(const char* command);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  bool button_pressed = false;
  while (1)
  {
	  // COPY BUTTONS
	  if (HAL_GPIO_ReadPin(GPIOA, Copy_Button_1_Pin))
		  button_pressed = send_command_to_server("COPY1");
	  else if (HAL_GPIO_ReadPin(GPIOA, Copy_Button_2_Pin))
		  button_pressed = send_command_to_server("COPY2");
	  else if (HAL_GPIO_ReadPin(GPIOA, Copy_Button_3_Pin))
		  button_pressed = send_command_to_server("COPY3");
	  else if (HAL_GPIO_ReadPin(GPIOA, Copy_Button_4_Pin))
		  button_pressed = send_command_to_server("COPY4");

	  // PASTE BUTTONS
	  else if (HAL_GPIO_ReadPin(GPIOB, Paste_Button_1_Pin))
		  button_pressed = send_flash_buffer_to_server(CLIPBOARD1_METADATA_ADDRESS, CLIPBOARD1_BEGIN_ADDRESS);
	  else if (HAL_GPIO_ReadPin(GPIOB, Paste_Button_2_Pin))
		  button_pressed = send_flash_buffer_to_server(CLIPBOARD2_METADATA_ADDRESS, CLIPBOARD2_BEGIN_ADDRESS);
	  else if (HAL_GPIO_ReadPin(GPIOA, Paste_Button_3_Pin))
		  button_pressed = send_flash_buffer_to_server(CLIPBOARD3_METADATA_ADDRESS, CLIPBOARD3_BEGIN_ADDRESS);
	  else if (HAL_GPIO_ReadPin(GPIOA, Paste_Button_4_Pin))
		  button_pressed = send_flash_buffer_to_server(CLIPBOARD4_METADATA_ADDRESS, CLIPBOARD4_BEGIN_ADDRESS);

	  if (button_pressed)
	  {
		  HAL_Delay(200);
		  button_pressed = false;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 5;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : Copy_Button_1_Pin Copy_Button_2_Pin Copy_Button_3_Pin Copy_Button_4_Pin
                           Paste_Button_4_Pin Paste_Button_3_Pin */
  GPIO_InitStruct.Pin = Copy_Button_1_Pin|Copy_Button_2_Pin|Copy_Button_3_Pin|Copy_Button_4_Pin
                          |Paste_Button_4_Pin|Paste_Button_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Paste_Button_2_Pin Paste_Button_1_Pin */
  GPIO_InitStruct.Pin = Paste_Button_2_Pin|Paste_Button_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static bool send_flash_buffer_to_server(uint32_t metadata_address, uint32_t begin_address)
{
	char read_flash_data[256] = {0};
	char send_server_data[256] = {0};
	read_string_from_flash(read_flash_data, NULL, metadata_address, begin_address);
	strncpy(send_server_data, "PASTE", 6);
	strncat(send_server_data, read_flash_data, 255);
	CDC_Transmit_FS((uint8_t*)send_server_data, strlen(send_server_data));

	return true;
}

static bool send_command_to_server(const char* command)
{
	char data[256] = {0};
	strncpy(data, command, strlen(command) + 1);
	CDC_Transmit_FS((uint8_t*)data, strlen(data));

	return true;
}
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
