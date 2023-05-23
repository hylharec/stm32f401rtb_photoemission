/*
 * flash_operation.c
 *
 *  Created on: Apr 18, 2023
 *      Author: hugo
 */

#include "flash_operation.h"

struct FLASH_op_context flash_op_context = {0, 0, 0, 0, 0, 0, 0};

const uint32_t *SECTORS_ADDR_START[8] = {
  (uint32_t*) 0x08000000,
  (uint32_t*) 0x08004000,
  (uint32_t*) 0x08008000,
  (uint32_t*) 0x0800C000,
  (uint32_t*) 0x08010000,
  (uint32_t*) 0x08020000,
  (uint32_t*) 0x08040000,
  (uint32_t*) 0x08060000
};
const uint32_t *SECTORS_ADDR_END[8] = {
  (uint32_t*) 0x08003FFF,
  (uint32_t*) 0x08007FFF,
  (uint32_t*) 0x0800BFFF,
  (uint32_t*) 0x0800FFFF,
  (uint32_t*) 0x0801FFFF,
  (uint32_t*) 0x0803FFFF,
  (uint32_t*) 0x0805FFFF,
  (uint32_t*) 0x0807FFFF
};



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
    // Erase corresponding sector before writing (Warning: Only checks sector of starting address)
    int sector = 0;
    if (flash_op_context.addr > SECTORS_ADDR_START[7]) {
      sector = 7;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[6]) {
      sector = 6;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[5]) {
      sector = 5;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[4]) {
      sector = 4;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[3]) {
      sector = 3;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[2]) {
      sector = 2;
    }
    else if (flash_op_context.addr > SECTORS_ADDR_START[1]) {
      sector = 1;
    }
    FPEC_SectorErase(sector);

    FPEC_Program(flash_op_context.addr, (uint32_t) flash_op_context.data, flash_op_context.do_trig, flash_op_context.span);

    flash_op_context.configured = 0; // clear context
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
      ASM_TRIGGER_HIGH();
      __asm("NOP");
      __asm("NOP");

      uint32_t i;
      for (i = 0; i < flash_op_context.span; i++)
        flash_op_context.data = *((volatile uint32_t*) (flash_op_context.addr + 4 * i));

      __asm("NOP");
      ASM_TRIGGER_LOW();
    }
    else
    {
      uint32_t i;
      for (i = 0; i < flash_op_context.span; i++)
        flash_op_context.data = *((volatile uint32_t*) (flash_op_context.addr + 4 * i));
    }

    char read_value_str[11];
    sprintf(read_value_str, "%08X\r\n", (unsigned int) flash_op_context.data);

    flash_op_context.configured = 0; // clear context

    // Send the value read back to UART
    if (verbose)
      send_answer(read_value_str, sizeof(read_value_str));
  }
}

void FLASH_erase(uint8_t verbose) {
  // sector in [0; 7]
  if (7 < flash_op_context.sector) {
    flash_op_context.sector = 7;
  }

  FPEC_SectorErase(flash_op_context.sector);

  if (verbose)
    send_ok();
}
