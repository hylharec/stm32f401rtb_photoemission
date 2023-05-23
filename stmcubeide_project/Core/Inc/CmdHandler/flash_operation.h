/*
  ******************************************************************************
  * @file           : flash_operation.h
  * @brief          : FLASH related operations
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

#ifndef INC_FLASH_OPERATION_H_
#define INC_FLASH_OPERATION_H_

#include <string.h>
#include <stdio.h>
#include "FPEC_interface.h"
#include "hal_utils.h"
#include "configuration.h"

// Prototypes

void FLASH_continuous_start(void);
void FLASH_oneshot(void);
void FLASH_write(uint8_t verbose);
void FLASH_read(uint8_t verbose);
void FLASH_erase(uint8_t verbose);

// Structures

struct FLASH_op_context {
  uint8_t is_write; // write or read
  uint8_t is_erase; // erase or wr/rd
  uint32_t span; // on how many adjacent words should the operation be performed
  uint32_t data; // data to write or last read data
  uint32_t *addr; // address to wr/rd
  uint8_t sector; // sector to erase [0;7]
  uint8_t do_trig; // trigger sync output or not
  uint8_t configured; // was struct configured (for continuous)
};

// Variables declaration

extern struct FLASH_op_context flash_op_context;

extern const uint32_t SECTORS_ADDR_START[8];
extern const uint32_t SECTORS_ADDR_END[8];

// // ... from outside

extern TIM_HandleTypeDef *timer;
extern char *cmds[10];

#endif /* INC_FLASH_OPERATION_H_ */
