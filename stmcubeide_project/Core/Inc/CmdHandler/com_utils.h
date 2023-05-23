/*
  ******************************************************************************
  * @file           : uart_utils.h
  * @brief          : Header for uart_utils.c file.
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

#ifndef UART_UTILS_H
#define UART_UTILS_H

#include <string.h>
#include "configuration.h"

// Prototypes

void send_answer(const char* msg, size_t len);
void split_cmds(void);
void send_ok(void);
void send_invalid_cmd(void);

// Variables declaration from outside
extern uint8_t Rx_data[RX_LEN];
extern uint8_t Rx_data_copy[RX_LEN];
extern char *cmds[10];

extern UART_HandleTypeDef *uart;

#endif
