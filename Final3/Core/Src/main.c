/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <time.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
struct co {
	int x;
	int y;
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
const char msg[] = "please enter \"snake\": ";
const char clear[] = "\033[J";
const char clearAll[] = { "\033[2J" };
const char showCursor[] = { 27, '[', '?', '2', '5', 'h' };
const char hideCursor[] = { 27, '[', '?', '2', '5', 'l' };
const char saveCursor[] = { 27, '7' };
const char restoreCursor[] = { 27, '8' };
const int width = 100;
const int height = 50;
char render[51][101];
char buff[1];
char buff2[100];
char dirBuff[3];
int status = 0;
int i;
struct co food;
int sx[200];
int sy[200];
int snakeLength;
int dir;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void clearMap() {
	for (int i = 0; i < height + 1; i++) {
		for (int j = 0; j < width + 1; j++) {
			if (i == 0 || i == height || j == 0 || j == width) {
				render[i][j] = '#';
			} else {
				render[i][j] = ' ';
			}
		}
	}
}
void display() {
	char buffer[20] = { 27, '8' };
	HAL_UART_Transmit(&huart2, buffer, sizeof(buffer), 10);
	HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
	for (int i = 0; i < height + 1; i++) {
		for (int j = 0; j < width + 1; j++) {
			buff[0] = render[i][j];
			HAL_UART_Transmit(&huart2, buff, sizeof(buff), 1000);
		}
		HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
	}
}
void randFood() {
	int x = (rand() % (width) + 1);
	int y = (rand() % (height) + 1);
	food.x = x;
	food.y = y;
}

int buttonDir() {
	int pressDir = 0;
	if (dirBuff[0] == 27 && dirBuff[1] == '[' && dirBuff[2] == 'A') {
		if (dir != 3 && !(sy[1] == sy[0] - 1 && sx[0] == sx[1])) {
			pressDir = 1;
		} else {
			pressDir = 3;
		}
	} else if (dirBuff[0] == 27 && dirBuff[1] == '[' && dirBuff[2] == 'C') {
		if (dir != 4 && !(sx[1] == sx[0] + 1 && sy[1] == sy[0])) {
			pressDir = 2;
		} else {
			pressDir = 4;
		}
	} else if (dirBuff[0] == 27 && dirBuff[1] == '[' && dirBuff[2] == 'B') {
		if (dir != 1 && !(sy[1] == sy[0] + 1 && sx[1] == sx[0])) {
			pressDir = 3;
		} else {
			pressDir = 1;
		}
	} else if (dirBuff[0] == 27 && dirBuff[1] == '[' && dirBuff[2] == 'D') {
		if (dir != 2 && !(sx[1] == sx[0] - 1 && sy[1] == sy[0])) {
			pressDir = 4;
		} else {
			pressDir = 2;
		}
	}

	return pressDir;
}

int checkGameLose() {
	//check colide with border
	if (sx[0] == 0 || sx[0] == 100 || sy[0] == 0 || sy[0] == 50) {
		return 1;
	}
	//check snake eat their body
	for (int i = 1; i < snakeLength; i++) {
		if (sx[0] == sx[i] && sy[0] == sy[i]) {
			return 1;
		}
	}
	return 0;
}

void update() {
	if (sx[0] == food.x && sy[0] == food.y) {
		int tailX = sx[snakeLength - 1];
		int tailY = sy[snakeLength - 1];
		int befTailX = sx[snakeLength - 2];
		int befTailY = sy[snakeLength - 2];
		if (tailX == befTailX) {
			sx[snakeLength] = tailX;
			sy[snakeLength] = tailY + (tailY - befTailY);
		} else {
			sx[snakeLength] = tailX + (tailX - befTailX);
			sy[snakeLength] = tailY;
		}
		snakeLength++;
		randFood();
	}
	render[sy[snakeLength - 1]][sx[snakeLength - 1]] = ' ';
	for (int i = snakeLength - 1; i > 0; i--) {
		sx[i] = sx[i - 1];
		sy[i] = sy[i - 1];
	}
	if (dir == 1) {
		sx[0] = sx[0];
		sy[0] = sy[0] - 1;

	} else if (dir == 2) {
		sx[0] = sx[0] + 1;
		sy[0] = sy[0];

	} else if (dir == 3) {
		sx[0] = sx[0];
		sy[0] = sy[0] + 1;

	} else if (dir == 4) {
		sx[0] = sx[0] - 1;
		sy[0] = sy[0];

	}
	if (!checkGameLose()) {
		render[food.y][food.x] = 'F';
		for (int i = 0; i < snakeLength; i++) {
			if (i == 0) {
				render[sy[i]][sx[i]] = 'Q';
			} else {
				render[sy[i]][sx[i]] = 'O';
			}
		}
		HAL_UART_Receive_IT(&huart2, dirBuff, sizeof(dirBuff));
	} else {
		status = 3;
	}
}

void gameInit() {
	clearMap();
	HAL_UART_Transmit(&huart2, showCursor, sizeof(showCursor), 10);
	i = 0;
	randFood();
	sx[0] = 32;
	sx[1] = 31;
	sx[2] = 30;
	sy[0] = 30;
	sy[1] = 30;
	sy[2] = 30;
	snakeLength = 3;
	dir = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	dir = buttonDir();
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_USART2_UART_Init();

	/* USER CODE BEGIN 2 */
	gameInit();
	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		if (status == 0) {
			memset(buff, 0, 1);
			HAL_UART_Receive(&huart2, buff, sizeof(buff), 5000);
			HAL_UART_Transmit(&huart2, buff, sizeof(buff), 5000);
			if (buff[0] == 13) {
				if (buff2[i - 1] == 'e' && buff2[i - 2] == 'k'
						&& buff2[i - 3] == 'a' && buff2[i - 4] == 'n'
						&& buff2[i - 5] == 's') {
					HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
					HAL_UART_Transmit(&huart2, "Do you want to play SNAKE?: ",
							28, 100);
					i = 0;
					status = 1;
				} else {
					HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
					HAL_UART_Transmit(&huart2, msg, sizeof(msg), 10);
				}
			} else {
				buff2[i++] = buff[0];
			}
		} else if (status == 1) {
			memset(buff, 0, 1);
			HAL_UART_Receive(&huart2, buff, sizeof(buff), 5000);
			HAL_UART_Transmit(&huart2, buff, sizeof(buff), 5000);
			if (buff[0] == 13) {
				if (buff2[i - 1] == 'y' || buff2[i - 1] == 'Y') {
					HAL_UART_Transmit(&huart2, clearAll, sizeof(clearAll), 10);
					char snake[] =
							" _____ _   _  ___  _   _______ \r\n/  ___| \\ | |/ _ \\| | / /  ___|\r\n\\ `--.|  \\| / /_\\ \\ |/ /| |__  \r\n `--. \\ . ` |  _  |    \\|  __| \r\n/\\__/ / |\\  | | | | |\\  \\ |___ \r\n\\____/\\_| \\_|_| |_|_| \\_|____/ \r\n";
					HAL_UART_Transmit(&huart2, snake, sizeof(snake), 1000);
					HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
					HAL_UART_Transmit(&huart2, "SNAKE Ready!\r\n",
							sizeof("SNAKE Ready!\r\n"), 100);

					HAL_UART_Transmit(&huart2, saveCursor, sizeof(saveCursor),
							10);
					HAL_UART_Transmit(&huart2, hideCursor, sizeof(hideCursor),
							10);
					HAL_UART_Receive_IT(&huart2, dirBuff, sizeof(dirBuff));
					display();
					status = 2;
				} else {
					HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
					HAL_UART_Transmit(&huart2, msg, sizeof(msg), 10);
					status = 0;
				}
			} else {
				buff2[i++] = buff[0];
			}
		} else if (status == 2) {
			update();
			display();
			HAL_Delay(100);
		} else if (status == 3) {
			char msg1[] = "\r\nGame Over!!!\r\nPlease hold \"q\" to try again.";
			HAL_UART_Transmit(&huart2, msg1, sizeof(msg1), 10);
			HAL_UART_Transmit(&huart2, restoreCursor, sizeof(restoreCursor),
					10);
			status = 4;
		} else if (status == 4) {
			HAL_UART_Receive(&huart2, buff, sizeof(buff), 1000);
			if (buff[0] == 'q' || buff[0] == 'Q') {
				gameInit();
				HAL_UART_Transmit(&huart2, clear, sizeof(clear), 10);
				HAL_UART_Transmit(&huart2, hideCursor, sizeof(hideCursor), 10);
				display();
				status = 2;
			}
		}
		/* USER CODE END 3 */
	}
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 921600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
