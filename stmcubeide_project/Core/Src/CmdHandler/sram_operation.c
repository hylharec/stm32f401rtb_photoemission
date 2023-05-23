/*
 * sram_operation.c
 *
 *  Created on: Apr 18, 2023
 *      Author: hugo
 */

#include "sram_operation.h"

struct SRAM_op_context sram_op_context = {0, 0, 0, 0, 0, 0, 0};

void SRAM_continuous_start(void) {
  // configure context structure
  if (cmd_is(3, "trig", 4)) {
    sram_op_context.do_trig = 1;
  }
  else {
    sram_op_context.do_trig = 0;
  }

  sram_op_context.span = atoi(cmds[4]);
  sram_op_context.nb_nops = atoi(cmds[5]);

  char *addr_str = cmds[6];
  str2num(addr_str, (uint32_t *) &(sram_op_context.addr), "%8x");

  if (cmd_is(2, "write", 5)) {
    // START>SRAM>write>trig>span>nb_nops>0xaddr>0xdata
    sram_op_context.is_write = 1;

    char *data_str = cmds[7];
    str2num(data_str, &(sram_op_context.data), "%8x");
  }
  else if (cmd_is(2, "read", 4)) {
    // START>SRAM>read>trig>span>nb_nops>0xaddr
    sram_op_context.is_write = 0;

    char *data_str = cmds[7];
    str2num(data_str, &(sram_op_context.data), "%8x");
    sram_op_context.data = 0;
  }
  sram_op_context.configured = 1;

  // start timer ITs
  HAL_TIM_PWM_Init(timer);
  HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(timer);
}

void SRAM_oneshot(void) {
  // configure context structure
  if (cmd_is(3, "trig", 4)) {
    sram_op_context.do_trig = 1;
  }
  else {
    sram_op_context.do_trig = 0;
  }

  sram_op_context.span = atoi(cmds[4]);
  sram_op_context.nb_nops = atoi(cmds[5]);

  char *addr_str = cmds[6];
  str2num(addr_str, (uint32_t *) &(sram_op_context.addr), "%8x");

  sram_op_context.configured = 1;

  if (cmd_is(2, "write", 5)) {
    // ONESHOT>SRAM>write>trig>span>nb_nops>0xaddr>0xdata
    sram_op_context.is_write = 1;

    char *data_str = cmds[7];
    str2num(data_str, &(sram_op_context.data), "%8x");

    SRAM_write(1);
  }
  else if (cmd_is(2, "read", 4)) {
    // ONESHOT>SRAM>read>trig>span>nb_nops>0xaddr
    sram_op_context.is_write = 0;

    char *data_str = cmds[7];
    str2num(data_str, &(sram_op_context.data), "%8x");
    sram_op_context.data = 0;

    SRAM_read(1);
  }
}

// ====================================================================

void SRAM_write(uint8_t verbose) {
  uint64_t i;
  uint64_t j;
  if (sram_op_context.configured == 0) {
    if (verbose)
      send_answer("Error: undefined sram op context\r\n", 34);
  }
  else {
    if (sram_op_context.do_trig)
    {
        //D11_GPIO_Port->BSRR = D11_Pin; // Set trigger pin high

      for(i = 0; i < sram_op_context.span; ++i) {
        ASM_TRIGGER_HIGH();
        for(j = 0; j < sram_op_context.nb_nops; ++j) {
          __asm("NOP");
        }
        *(sram_op_context.addr + i) = sram_op_context.data;
        __asm("NOP");
        ASM_TRIGGER_LOW();
      }
    }
    else
    {

      for(i = 0; i < sram_op_context.span; ++i) {
        *(sram_op_context.addr + i) = sram_op_context.data;
      }

    }

    sram_op_context.configured = 0; // clear context
    if (verbose) {
      send_ok();
    }
  }
}

void SRAM_read(uint8_t verbose) {
  int i, j;
  if (sram_op_context.configured == 0) {
    if (verbose)
      send_answer("Error: undefined sram op context\r\n", 34);
  }
  else {
    if (sram_op_context.do_trig)
    {
      // Read back the memory content
      //D11_GPIO_Port->BSRR = D11_Pin; // Set trigger pin high

      for(i = 0; i < sram_op_context.span; ++i) {
        ASM_TRIGGER_HIGH();
        for(j = 0; j < sram_op_context.nb_nops; ++j) {
          __asm("NOP");
        }
        sram_op_context.data = *(sram_op_context.addr + i);
        __asm("NOP");
        ASM_TRIGGER_LOW();
      }

    }
    else
    {
      for(i = 0; i < sram_op_context.span; ++i) {
        sram_op_context.data = *(sram_op_context.addr + i);
      }
    }

    char read_value_str[11];
    sprintf(read_value_str, "%08X\r\n", (unsigned int) sram_op_context.data);

    sram_op_context.configured = 0; // clear context
    // Send the value read back to UART
    if (verbose) {
      send_answer(read_value_str, sizeof(read_value_str));
    }

  }
}
