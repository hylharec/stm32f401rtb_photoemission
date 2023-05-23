/*
  ******************************************************************************
  * @file           : cmd_handling.h
  * @brief          : Header for cmd_handling.c file.
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

#ifndef INC_CMD_HANDLING_H_
#define INC_CMD_HANDLING_H_

#include <com_utils.h>
#include "stdlib.h"

#include "hal_utils.h"
#include "configuration.h"

#include "sram_operation.h"
#include "flash_operation.h"
#include "aes_operation.h"

// Prototypes

void Init_Cmd_Handling(TIM_HandleTypeDef *p_timer, UART_HandleTypeDef *p_uart, DMA_HandleTypeDef *p_dma_uart_tx, SPI_HandleTypeDef *p_spi);
void Start_Cmd_Reception(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
int cmd_is(uint8_t cmd_index, const char *cmd_ref, size_t len);
void Update_Timer_Prescaler(uint16_t prescaler);
void Update_Timer_PwmWidth(uint16_t pulsewidth);
void Update_Timer_Autoreload(uint16_t autoreload);

// Structures / enumerations

enum State {
  IDLE,
  CONTINUOUS_IN_PROGRESS
};

// Variables declaration

extern SPI_HandleTypeDef *spi;
extern TIM_HandleTypeDef *timer;
extern UART_HandleTypeDef *uart;
extern DMA_HandleTypeDef *dma_uart_tx;
extern char *cmds[10];

#endif /* INC_CMD_HANDLING_H_ */
