/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// ***用户***头文件
#include <stdio.h>
#include <stdbool.h>
#include "log.h"
#include "chry_ringbuffer.h"
#include "winusb.h"
#include "dap_main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// ***用户***私有类型定义
typedef struct
{
  uint8_t len;
  uint8_t opcode;
  uint16_t bits;
  uint8_t data[];
} command;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// ***用户***私有定义
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// ***用户***私有宏定义
#define ROUND_UP(bits, n) ((bits + (n - 1)) / n)
#define ROUND_UP_8(bits) ROUND_UP(bits, 8)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
// ***用户***私有变量
chry_ringbuffer_t rb;
uint8_t mempool[1024];

uint8_t buff[64] = {0};
command *curr_cmd = (command *)buff;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
// ***用户***私有函数签名
// extern void cJTAG_sequence(uint8_t *ucTMS, uint8_t *ucTDI, uint8_t *ucTDO,
//                            uint32_t bits);
extern void cJtag_active(void);
extern void cJTAG_seq(uint32_t bits, uint8_t *ucTDI, uint8_t *ucTDO);
extern void cJTAG_tms(uint32_t bits, uint8_t* ucTMS);
extern int test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ***用户***私有代码0
// commom functions
char *to_binary_string(unsigned int value, uint32_t bits)
{
    static char binary_string[64] = {0};
    uint32_t i = 0;
    // 从最高位到最低位逐位检查并填充字符串
    for (i = 0; i < bits; i++)
    {
        binary_string[i] = ((value >> i) & 1) ? '1' : '0';
    }
    // 添加字符串结束符
    binary_string[i] = '\0';

    return binary_string;
}
int __io_getchar(FILE *file) {
  (void)file;
  return 0;
}

int __io_putchar(char ch, FILE *file) {
  // (void)ch;
  (void)file;
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return 1;
}

void LED_TogglePin(void)
{
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void cJtag_task(void) {
  if (chry_ringbuffer_peek_byte(&rb, buff)) {
    // printf("len %d\r\n", buff[0]);
    if (buff[0] <= chry_ringbuffer_get_used(&rb)) {
      chry_ringbuffer_read(&rb, buff, buff[0]);
      LED_TogglePin();
      // recive a cmd
      // log_info("--cmd %d---", count++);
      // log_info("cmd[%02X]bits[%d]", curr_cmd->opcode, curr_cmd->bits);
      switch (curr_cmd->opcode) {
      case 0:    // ir/dr scan
      case 0x80: // ir/dr scan
      {
        uint8_t buffer[0x100] = {0};
        cJTAG_seq(curr_cmd->bits, (uint8_t *)&curr_cmd->data[0], buffer);
        usbd_winusb_write(buffer, ROUND_UP_8(curr_cmd->bits));
      } break;
      case 1: // tms
      {
        cJTAG_tms(curr_cmd->bits, (uint8_t *)&curr_cmd->data[0]);
      } break;
      case 0xFF:
      {
        cJtag_active();
        break;
      }
      default:
        break;
      }
    }
  }
}
typedef struct
{
  uint8_t len;
  uint8_t opcode;
  uint8_t data[];
} common_msg;

typedef struct
{
    uint8_t len;
    uint8_t opcode;
    uint8_t addr : 7;
    uint8_t is_read : 1;
    uint8_t data_len : 7;
    uint8_t is_continue : 1;
    uint8_t data[];
} i2c_cmdx;

chry_ringbuffer_t msg_rb;
static uint8_t mem_pool[1024];
static uint8_t msg_buff[64] = {0};
static common_msg *curr_msg = (common_msg *)msg_buff;
static i2c_cmdx *curr_i2c_cmd = (i2c_cmdx *)msg_buff;
uint8_t i2c_rx_buff[0x100] = {0};

void common_msg_buffer_push(uint8_t *data, uint8_t len)
{
    chry_ringbuffer_write(&msg_rb, data, len);
}

void common_msg_handler(void) {
  if (chry_ringbuffer_peek_byte(&msg_rb, msg_buff)) {
    if (msg_buff[0] <= chry_ringbuffer_get_used(&msg_rb)) {
      chry_ringbuffer_read(&msg_rb, msg_buff, msg_buff[0]);
      LED_TogglePin();
      // recive a cmd
      // log_info("--cmd %d---", count++);
      // log_info("cmd[%02X]bits[%d]", curr_msg->opcode, curr_msg->bits);

      switch (curr_msg->opcode) {
      case 0: // i2c cmd
      {
        if (curr_i2c_cmd->is_read) {
            // read
            HAL_I2C_Master_Receive(&hi2c1, curr_i2c_cmd->addr<<1,
              (uint8_t *)&i2c_rx_buff+1, curr_i2c_cmd->data_len, 0x1000);
            i2c_rx_buff[0] = 0xAA;
            log_hex("read data", i2c_rx_buff+1, curr_i2c_cmd->data_len);
            usbd_winusb_write(i2c_rx_buff, curr_i2c_cmd->data_len+1);
            // printf("read len %d\r\n", curr_i2c_cmd->data_len);   
        }
        else {
            // write
            HAL_I2C_Master_Transmit(&hi2c1, curr_i2c_cmd->addr<<1,
              (uint8_t *)&curr_i2c_cmd->data[0], curr_i2c_cmd->data_len, 0x1000);
            i2c_rx_buff[0] = 0xBB;
            usbd_winusb_write(i2c_rx_buff, 1);
            // printf("write len %d\r\n", curr_i2c_cmd->data_len);   
        }
        break;
      } 
      default:
        break;
      }
    }
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
  // ***用户***私有代码1
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // ***用户***初始化
  // chry_ringbuffer_init(&rb, mempool, 1024);
  chry_ringbuffer_init(&msg_rb, mem_pool, sizeof(mem_pool));
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // ***用户***系统初始化
  HAL_EnableCompensationCell();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  // ***用户***私有代码2
  // log_set_level(LOG_DEBUG);
  // winusb_init(0, (uintptr_t)USB_OTG_FS);
  // chry_dap_init(0, (uintptr_t)USB_OTG_FS);
  extern void winusb_init(uint8_t busid, uintptr_t reg_base);
  winusb_init(0, (uintptr_t)USB_OTG_FS);
  // cJtag_active();
  // test();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hi2c1.Init.OwnAddress1 = 0;
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
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TCKC_Pin|TMSC_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TCKC_Pin TMSC_Pin */
  GPIO_InitStruct.Pin = TCKC_Pin|TMSC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    // common_msg_handler();
    // cJtag_task();
    // static uint32_t tick = 0;
    // if ((HAL_GetTick() - tick)>2000) {
    //   LED_TogglePin();
    //   tick = HAL_GetTick();
    // }

    // chry_dap_handle();
    // chry_dap_usb2uart_handle();
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
