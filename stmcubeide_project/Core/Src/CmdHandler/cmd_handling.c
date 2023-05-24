/*
 ******************************************************************************
 * @file           : cmd_handling.c
 * @brief          : Handling of commands received through UART
 * @author         : Hugo Perrin (h.perrin@emse.fr)
 ******************************************************************************
 */

#include "cmd_handling.h"

// Variables definition

volatile enum State state = IDLE;

uint8_t UART1_rxBuffer[10] = {0};
uint8_t Rx_data[RX_LEN];
uint8_t Rx_data_copy[RX_LEN];
char *cmds[10];

TIM_HandleTypeDef *timer;
UART_HandleTypeDef *uart;
DMA_HandleTypeDef *dma_uart_tx;

// Functions definition

/* It is mandatory to call this function in main before any other function of this file is called.
 * It will associate the timer, uart and dma tx channel to the right pointers (to make it easier to port code to
 * other STM microcontrollers or to change which timer/uart is being used).
 **/
void Init_Cmd_Handling(TIM_HandleTypeDef *p_timer, UART_HandleTypeDef *p_uart, DMA_HandleTypeDef *p_dma_uart_tx) {
  timer = p_timer;
  uart = p_uart;
  dma_uart_tx = p_dma_uart_tx;
}

void Start_Cmd_Reception(void) {
  HAL_UART_Receive_DMA(uart, Rx_data, RX_LEN);
}

/* This callback is called by the HAL_UART_IRQHandler when the given number of bytes are received */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  // Verify that the huart that triggered the IT is the right one
  if (huart->Instance == uart->Instance) {

    // Split cmd string into an array of strings
    split_cmds();

    send_ok();

    if (state == CONTINUOUS_IN_PROGRESS) {

      if (cmd_is(0, "STOP", 4)) {
        // Stop timer interrupts and PWM output
        HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2);
        HAL_TIM_Base_Stop_IT(timer);

        sram_op_context.configured = 0;
        flash_op_context.configured = 0;
        aes_op_context.configured = 0;

        state = IDLE;
        send_answer("Continuous was stopped\r\n", 24);
      }
      else {
        send_answer("Error: Continuous in progress\r\n", 31);
      }
    }

    else if (state == IDLE) {

      if (cmd_is(0, "TIMER_PSC", 9)) {
        // TIMER_PSC>psc
        uint16_t psc = atoi(cmds[1]);
        Update_Timer_Prescaler(psc);
        send_ok();
      }
      else if (cmd_is(0, "TIMER_PWM_WIDTH", 15)) {
        // TIMER_PWM_WIDTH>width
        uint16_t width = atoi(cmds[1]);
        Update_Timer_PwmWidth(width);
        send_ok();
      }
      else if (cmd_is(0, "TIMER_ARR", 9)) {
        // TIMER_ARR>arr
        uint16_t arr = atoi(cmds[1]);
        Update_Timer_Autoreload(arr);
        send_ok();
      }
      else if (cmd_is(0, "START", 5)) {
        if (cmd_is(1, "SRAM", 4)) {
          SRAM_continuous_start();
          state = CONTINUOUS_IN_PROGRESS;
          send_answer("Continuous started\r\n", 20);
        }
        else if (cmd_is(1, "FLASH", 5)) {
          FLASH_continuous_start();
          state = CONTINUOUS_IN_PROGRESS;
          send_answer("Continuous started\r\n", 20);
        }
        else if (cmd_is(1, "AES", 3)) {
          if (AES_continuous_start()) {
            state = CONTINUOUS_IN_PROGRESS;
            send_answer("Continuous started\r\n", 20);
          }
          else {
            send_answer("Could not start continuous aes\r\n", 32);
          }
        }
      }
      else if (cmd_is(0, "STOP", 4)) {
        send_answer("Error: Nothing to stop\r\n", 24);
      }
      else if (cmd_is(0, "HANDSHAKE", 4)) {
        send_ok();
      }
      else if (cmd_is(0, "trig", 4)) {
        trigger_high(0);
        trigger_low(0);
        send_ok();
      }

      else if (cmd_is(0, "ONESHOT", 7)) {
        if (cmd_is(1, "SRAM", 4)) {
          SRAM_oneshot();
        }
        else if (cmd_is(1, "FLASH", 5)) {
          FLASH_oneshot();
        }
        else {
          send_answer("Error: Unknown cmd\r\n", 20);
        }
      }
      else if (cmd_is(0, "AES_SETUP", 9)) {
        if (cmd_is(1, "KEY", 3))
          AES_setup(KEY);
        else if (cmd_is(1, "PLAIN", 5))
          AES_setup(PLAIN);
        else
          AES_setup(CIPHER);
      }
      else if (cmd_is(0, "AES_ENCRYPT", 11)) {
        AES_encrypt(1);
      }
      else if (cmd_is(0, "AES_DECRYPT", 11)) {
        AES_decrypt(1);
      }
      else {
        send_answer("Error: Unknown cmd\r\n", 20);
      }
    }

    HAL_UART_Receive_DMA(uart, Rx_data, RX_LEN);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == timer->Instance) {

    LED_ON();

    if (sram_op_context.configured) {
      if (sram_op_context.is_write) {
        SRAM_write(0);
      }
      else {
        SRAM_read(0);
      }
    }
    else if (flash_op_context.configured) {
      if (flash_op_context.is_erase) {
        FLASH_erase(0);
      }
      else {
        if (flash_op_context.is_write) {
          FLASH_write(0);
        }
        else {
          FLASH_read(0);
        }
      }
    }
    else if (aes_op_context.configured) {
      if (aes_op_context.is_encrypt) {
        AES_encrypt(0);
      }
      else {
        AES_decrypt(0);
      }
    }

    while (htim->Instance->CNT < 5) ;
    LED_OFF();
  }
}

/* Compares cmds_from_UART[cmd_index] with cmd_ref and returns result */
int cmd_is(uint8_t cmd_index, const char *cmd_ref, size_t len) {
  return (0 == strncmp((const char*)cmds[cmd_index], cmd_ref, len));
}

/* Updates the prescaler of the timer used for continuous mode */
void Update_Timer_Prescaler(uint16_t prescaler) {
  // Disables ITs in case they were still enabled
  HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2);
  HAL_TIM_Base_Stop_IT(timer);

  timer->Init.Prescaler = prescaler;
  timer->Init.CounterMode = TIM_COUNTERMODE_UP;
  timer->Init.Period = timer->Instance->ARR;
  timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(timer) != HAL_OK)
  {
    Error_Handler();
  }
}

void Update_Timer_PwmWidth(uint16_t pulsewidth) {
  // Disables ITs in case they were still enabled
  HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2);
  HAL_TIM_Base_Stop_IT(timer);

  TIM_OC_InitTypeDef sConfigOC = {0};

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  // timer counter value after which the pwm output falls to 0 (ex: if autoreload=10 and pulsewidth=5 -> 50% dutycycle)
  sConfigOC.Pulse = pulsewidth;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) // Timer Channel 2
  {
    Error_Handler();
  }
}

void Update_Timer_Autoreload(uint16_t autoreload) {
  // Disables ITs in case they were still enabled
  HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2);
  HAL_TIM_Base_Stop_IT(timer);

  timer->Init.Prescaler = timer->Instance->PSC;
  timer->Init.CounterMode = TIM_COUNTERMODE_UP;
  timer->Init.Period = autoreload;
  timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(timer) != HAL_OK)
  {
    Error_Handler();
  }
}
