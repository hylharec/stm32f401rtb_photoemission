/*
 * flash_operation.c
 *
 *  Created on: Apr 18, 2023
 *      Author: hugo
 */

#include "flash_operation.h"

struct FLASH_op_context flash_op_context = {0, 0, 0, 0, 0, 0, 0};

void FLASH_continuous_start(void) {
  // configure context structure
  if (cmd_is(2, "erase", 5)) {
    // START>FLASH>erase>sector
    flash_op_context.is_erase = 1;

    flash_op_context.sector = atoi(cmds[3]);
  }
  else {
    if (cmd_is(3, "trig", 4)) {
      flash_op_context.do_trig = 1;
    }
    else {
      flash_op_context.do_trig = 0;
    }

    char *addr_str = cmds[4];
    str2num(addr_str, (uint32_t *) &(flash_op_context.addr), "%8x");

    if (cmd_is(2, "write", 5)) {
      // START>FLASH>write>trig>0xaddr>0xdata>span
      flash_op_context.is_write = 1;
      flash_op_context.is_erase = 0;

      char *data_str = cmds[5];
      str2num(data_str, &(flash_op_context.data), "%8x");
      flash_op_context.span = atoi(cmds[5]);
    }
    else if (cmd_is(2, "read", 4)) {
      // START>FLASH>read>trig>0xaddr>span
      flash_op_context.is_write = 0;
      flash_op_context.is_erase = 0;

      flash_op_context.data = 0;
      flash_op_context.span = atoi(cmds[5]);
    }
  }

  flash_op_context.configured = 1;

  // start timer ITs
  HAL_TIM_PWM_Init(timer);
  HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(timer);
}

void FLASH_oneshot(void) {
  // configure context structure
  flash_op_context.configured = 1;

  if (cmd_is(2, "erase", 5)) {
    // ONESHOT>FLASH>erase>sector
    flash_op_context.sector = atoi(cmds[3]);

    FLASH_erase(1);
  }
  else {
    if (cmd_is(3, "trig", 4)) {
      flash_op_context.do_trig = 1;
    }
    else {
      flash_op_context.do_trig = 0;
    }

    char *addr_str = cmds[4];
    str2num(addr_str, (uint32_t *) &(flash_op_context.addr), "%8x");

    if (cmd_is(2, "write", 5)) {
      // ONESHOT>FLASH>write>trig>0xaddr>0xdata>span
      flash_op_context.is_write = 1;

      char *data_str = cmds[5];
      str2num(data_str, &(flash_op_context.data), "%8x");
      flash_op_context.span = atoi(cmds[6]);

      FLASH_write(1);
    }
    else if (cmd_is(2, "read", 4)) {
      // ONESHOT>FLASH>read>trig>0xaddr>span
      flash_op_context.is_write = 0;

      flash_op_context.data = 0;
      flash_op_context.span = atoi(cmds[5]);

      FLASH_read(1);
    }
  }
}

// ====================================================================

void FLASH_write(uint8_t verbose) {
  if (flash_op_context.configured == 0) {
    if (verbose)
      send_answer("Error: undefined flash op context\r\n", 34);
  }
  else {
    FPEC_Program(flash_op_context.addr, (uint32_t) flash_op_context.data, flash_op_context.do_trig, flash_op_context.span);

    if (verbose)
      send_ok();
  }
}

void FLASH_read(uint8_t verbose) {
  if (flash_op_context.configured == 0) {
    if (verbose)
      send_answer("Error: undefined flash op context\r\n", 34);
  }
  else {
    if (flash_op_context.do_trig)
    {
      // Read back the memory content
      trigger_high(2);

      uint32_t i;
      for (i = 0; i < flash_op_context.span; i++)
        flash_op_context.data = *((volatile uint32_t*) (flash_op_context.addr + i));

      trigger_low(1);
    }
    else
    {
      uint32_t i;
      for (i = 0; i < flash_op_context.span; i++)
        flash_op_context.data = *((volatile uint32_t*) (flash_op_context.addr + i));
    }

    char read_value_str[11];
    sprintf(read_value_str, "%08X\r\n", (unsigned int) flash_op_context.data);

    // Send the value read back to UART
    if (verbose)
      send_answer(read_value_str, sizeof(read_value_str));
  }
}

void FLASH_erase(uint8_t verbose) {
  // sector in [0; 7]
  if (5 < flash_op_context.sector) {
    flash_op_context.sector = 5;
  }

  FPEC_SectorErase(flash_op_context.sector);

  if (verbose)
    send_ok();
}
