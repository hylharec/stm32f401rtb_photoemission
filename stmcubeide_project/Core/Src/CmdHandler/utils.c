/*
  ******************************************************************************
  * @file           : utils.c
  * @brief          : Implement useful misc functions.
  * @author         : Raphael Viera (raphael.viera@emse.fr)
  ******************************************************************************
*/

#include "utils.h"

/**
  * @brief  This function is responsible for calling NOP commands.
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void do_NOP(uint32_t ntimes)
{
	int i;

	for (i = 1; i <= ntimes; ++i)
	{
		__asm("NOP");
	}
}

/**
  * @brief  This function is responsible for converting string to number.
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void str2num(char *str, uint32_t *dest, const char* type)
{
	sscanf((const char*) str, type, dest);
}

/**
 * @brief This function is used for AES operation to transform incoming hex string key/plain/cipher into an array of bytes (ex: "FD" -> 253)
 * @author Hugo Perrin (h.perrin@emse.fr)
 */
void hex2bytes(const uint8_t *src_str, size_t src_len, uint8_t *dest_array, size_t dest_len) {
  uint8_t i;
  uint8_t buf[3] = "\0\0\0";

  // Infinite loop in case of length mismatch
  while (src_len != 2 * dest_len) ;

  for (i = 0; i < dest_len; ++i) {
    buf[0] = src_str[2 * i];
    buf[1] = src_str[2 * i + 1];
    dest_array[i] = strtol((const char *) buf, 0, 16);
  }
}

/**
 * @brief This function is used for AES operation to transform incoming hex string key/plain/cipher into an array of bytes (ex: "FD" -> 253)
 * @author Hugo Perrin (h.perrin@emse.fr)
 */
void bytes2hex(const uint8_t *src_array, size_t src_len, uint8_t *dest_str, size_t dest_len) {
  uint8_t i;
  uint8_t buf[3] = "\0\0\0";

  // Infinite loop in case of length mismatch
  while (2 * src_len != dest_len) ;

  for (i = 0; i < src_len; ++i) {
    sprintf((char *) buf, "%02X", (unsigned int) src_array[i]);
    dest_str[2 * i] = buf[0];
    dest_str[2 * i + 1] = buf[1];
  }
}




