/*
  ******************************************************************************
  * @file           : utils.h
  * @brief          : Header for utils.c file.
  * @author         : Raphael Viera (raphael.viera@emse.fr)
  ******************************************************************************
*/

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include "configuration.h"

// Prototypes

void do_NOP(uint32_t ntimes);
void str2num(char *str, uint32_t *dest, const char* type);
void hex2bytes(const uint8_t *src_str, size_t src_len, uint8_t *dest_array, size_t dest_len);
void bytes2hex(const uint8_t *src_array, size_t src_len, uint8_t *dest_str, size_t dest_len);

#endif
