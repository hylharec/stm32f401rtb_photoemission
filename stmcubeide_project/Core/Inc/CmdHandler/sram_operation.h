/*
  ******************************************************************************
  * @file           : sram_operation.h
  * @brief          : SRAM related operations
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

#ifndef INC_SRAM_OPERATION_H_
#define INC_SRAM_OPERATION_H_

#include <string.h>
#include <stdio.h>
#include "hal_utils.h"

// Prototypes

void SRAM_continuous_start(void);
void SRAM_oneshot(void);
void SRAM_write(uint8_t verbose);
void SRAM_read(uint8_t verbose);

// Structures

struct SRAM_op_context {
  uint8_t is_write;
  uint32_t span; // on how many adjacent words should the operation be performed
  uint32_t data; // data to write or last read data
  uint32_t *addr; // address to wr/rd
  uint32_t nb_nops; // nops to insert before each operation
  uint8_t do_trig; // trigger sync output or not
  uint8_t configured; // was struct configured (for continuous)
};

// Variables declaration

extern struct SRAM_op_context sram_op_context;

// // ... from outside

extern TIM_HandleTypeDef *timer;
extern char *cmds[10];

#endif /* INC_SRAM_OPERATION_H_ */
