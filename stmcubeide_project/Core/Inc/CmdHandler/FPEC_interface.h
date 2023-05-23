/*
  ******************************************************************************
  * @file           : FPEC_interface.h
  * @brief          : Interfaces with the FPEC regs to write/read/erase in FLASH
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

// Initially created by Raphael Viera (raphael.viera@emse.fr)
// Adapted to STM32F446RE by Hugo PERRIN (h.perrin@emse.fr)

#ifndef FPEC_INTERFACE_H
#define FPEC_INTERFACE_H

#include "main.h"
#include "hal_utils.h"
#include "configuration.h"

#define SET1_BIT(REG,BIT) (REG |= (1 << BIT))

#define CLR_BIT(REG,BIT) (REG &= ~(1 << BIT))

#define TOG_BIT(REG,BIT) (REG ^= ( 1 << BIT))

#define GET_BIT(REG,BIT) ((REG>>BIT) & 1)

#define IS_CLR(REG,BIT) (!(REG & (1<<BIT)))

#define IS_SET(REG,BIT) (REG & (1<<BIT))

/*
Description: ERASE SECTOR
Input: Number of THE SECTOR
output: CHECK ERROR
*/
void FPEC_SectorErase(u8 Copy_u8SectorNumber);

/*
Description: Write a data word
Input: address ,data and trigger usage
output:void
*/
void FPEC_Program(uint32* Copy_u32Address, uint32 Copy_u32Data, u16 use_trigger, uint32 span);


void FPEC_Lock();

#endif
