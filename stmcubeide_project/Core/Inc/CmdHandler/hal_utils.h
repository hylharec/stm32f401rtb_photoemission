/*
  ******************************************************************************
  * @file           : hal_utils.h
  * @brief          : Header for hal_utils.c file.
  * @author         : Raphael Viera (raphael.viera@emse.fr)
  ******************************************************************************
*/

#ifndef HAL_UTILS_H
#define HAL_UTILS_H

#include <stdint.h>
#include "main.h"
#include "configuration.h"

// Prototypes

void blink_LED(uint32_t ntimes, uint32_t delay_ms);
void LED_OFF(void);
void LED_ON(void);
void trigger_high(uint32_t n_nops);
void trigger_low(uint32_t n_nops);
void trigger_setup(void);
void do_NOP(uint32_t ntimes);

#endif
