// Hugo PERRIN (h.perrin@emse.fr)
// Driver for the Flash Memory. Erase sector and Program (write).
// To Erase / Program more than one sector you need to call the functions in a loop.

#include "FPEC_interface.h"

/* There are 5 Flash mem sectors */
void FPEC_SectorErase(u8 Copy_u8SectorNumber)
{
  FLASH_Erase_Sector(Copy_u8SectorNumber, 0x00000002U);
}

/* Writes a word in Flash memory */
void FPEC_Program(uint32* Copy_u32Address, uint32 Copy_u32Data, u16 use_trigger, uint32 span)
{
  SET1_BIT(FPEC->CR,31); // set lock bit

  // Wait busy flag
  while (GET_BIT(FPEC->SR,16) == 1);

  // Check if FPEC is locked or not
  if ( /* FPEC_CR->BitAccess.LOCK == 1 */ GET_BIT(FPEC->CR,31) == 1 )
  {
    FPEC -> KEYR = 0x45670123;
    FPEC -> KEYR = 0xCDEF89AB;
  }

  // Wait Busy Flag
  while (GET_BIT(FPEC->SR,16) == 1);

  // Write Flash Programming
  SET1_BIT(FPEC->CR,9); // Word write operation mode
  CLR_BIT(FPEC->CR,8); //
  SET1_BIT(FPEC->CR,0); // set PG flag before write operation

  // Use output trigger signal or not
  if(use_trigger == 1)
  {
    /*ASM_TRIGGER_HIGH();
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");*/
    trigger_high(5);

    uint32_t i;
    for (i = 0; i < span; i++)
      *((volatile uint32_t*) (Copy_u32Address + i)) = Copy_u32Data;

    /*__asm("NOP");
    ASM_TRIGGER_LOW();*/
    trigger_low(1);
  }
  else
  {
    uint32_t i;
    for (i = 0; i < span; i++)
      *((volatile uint32_t*) (Copy_u32Address + i)) = Copy_u32Data;
  }

  // Wait Busy Flag
  while (GET_BIT(FPEC->SR,16) == 1);

  // EOP
  //SET_BIT(FPEC->SR,5);
  CLR_BIT(FPEC->CR,0); // disable PG bit if operation successful
}

void FPEC_Lock()
{
	SET1_BIT(FPEC->CR,31);
}
