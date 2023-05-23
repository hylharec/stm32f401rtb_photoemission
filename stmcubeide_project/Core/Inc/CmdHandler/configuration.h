/*
 * configuration.h
 *
 *  Created on: Apr 18, 2023
 *      Author: hugo
 *
 *  The purpose of this file is to centralize all the target specific pieces of code that
 *  might need to be changed when porting the code to another STM32.
 */

#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_

// =============================================================================================
// Needs to be changed depending on the project's target microcontroller
#include "stm32f4xx_hal.h"
// =============================================================================================
/*
 * Note:
 *    The pins of the microcontroller need to be labeled as:
 *        - LED: LED pin
 *        - TRIG_OUT0: simple gpio output (main trigger output)
 *        - TRIG_OUT1: simple gpio output (secondary trigger output)
 *        - TRIG_OUT_PWM: timer output channel 2 (trigger output signal when in continuous mode)
 */
// =============================================================================================
// Assembler trigger output control
// The 2 MOV instructions need to be changed to point to the right GPIO port and pin

#define ASM_TRIGGER_HIGH() ({\
__asm("MOV r3, 0x4002");      /* Move "&GPIOA >> 16" in register 3 */\
__asm("LSL r3, #16");         /* Transform "&GPIOA >> 16" into "&GPIOA" in reg 3 */\
__asm("MOV r2, #0x80");       /* Move "set pin 7" mask in register 2 */\
__asm("STR r2, [r3, #0x18]"); /* Apply mask on GPIO BSRR register */\
})


#define ASM_TRIGGER_LOW() ({\
__asm("MOV r3, 0x4002");      /* Move "&GPIOA >> 16" in register 3 */\
__asm("LSL r3, #16");         /* Transform "&GPIOA >> 16" into "&GPIOA" in reg 3 */\
__asm("MOV r2, #0x80");       /* Move "set pin 7" mask in register 2 */\
__asm("LSL r2, #16");         /* Transform SET mask into RESET mask (c.f. BSRR register) */\
__asm("STR r2, [r3, #0x18]"); /* Apply mask on GPIO BSRR register */\
})
// =============================================================================================
// Address to sector number
#define SECTOR_0 0x08000000
#define SECTOR_1 0x08004000
#define SECTOR_2 0x08008000
#define SECTOR_3 0x0800C000
#define SECTOR_4 0x08010000
#define SECTOR_5 0x08020000
#define SECTOR_6 0x08040000
#define SECTOR_7 0x08060000
// =============================================================================================
// The following structures and defines are related to the Flash interface registers of the
// target STM32. They might differ from one to the other.
// Refer to the REFERENCE MANUAL of the MCU.
typedef unsigned char      u8;
typedef unsigned short int u16;
typedef unsigned long int  u32;

typedef unsigned long      uint32;  // 0 .. 4294967295
typedef unsigned short     uint16;  // 0 .. 65535
typedef unsigned char      uint8;   // 0 .. 255

typedef union
{
    struct{
        u32  PG    :1 ;
        u32  PER   :1 ;
        u32  MER   :1 ;
        u32  RES1  :1 ;
        u32  OPTPG :1 ;
        u32  OPTER :1 ;
        u32  STRT  :1 ;
        u32  LOCK  :1 ;
        u32  RES2  :1 ;
        u32  OPTWRE:1 ;
        u32  ERRIE :1 ;
        u32  RES3  :1 ;
        u32  EOPIE :1 ;
        u32  RES4  :19;
    }BitAccess;

    u32 WordAccess;
}FPEC_CR_TYPE;

#define FPEC_CR ((FPEC_CR_TYPE*)0x40023C10) // Address of CR register (Flash interface register address + CR register offset)

typedef struct
{
    volatile u32 ACR;
    volatile u32 KEYR;
    volatile u32 OPTKEYR;
    volatile u32 SR;
    volatile u32 CR;
    volatile u32 FLASH_OPTCR;
}FPEC_TYPE;

#define FPEC ((FPEC_TYPE*)0x40023C00) // Start address of Flash interface registers
// =============================================================================================
// Should not have to be modified, written here to centralize
#define RX_LEN 64
// =============================================================================================

#endif /* INC_CONFIGURATION_H_ */
