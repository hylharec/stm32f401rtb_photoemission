/*
  ******************************************************************************
  * @file           : aes_operation.h
  * @brief          : AES related operations
  * @author         : Hugo Perrin (h.perrin@emse.fr)
  ******************************************************************************
*/

#ifndef INC_AES_OPERATION_H_
#define INC_AES_OPERATION_H_

#include <com_utils.h>
#include "aes.h"
#include "utils.h"
#include "hal_utils.h"

// Structures

struct AES_op_context {
  struct AES_ctx aes_ctx_struct;
  uint8_t key_configured;
  uint8_t encrypt_configured;
  uint8_t decrypt_configured;
  uint8_t key[16];
  uint8_t plain[16];
  uint8_t cipher[16];
  uint8_t is_encrypt;
  uint8_t do_trig; // trigger sync output or not
  uint8_t configured; // was struct configured (for continuous)
};

enum AES_data_type {
  KEY,
  PLAIN,
  CIPHER
};

// Prototypes

void AES_setup(enum AES_data_type data_type);
uint8_t AES_continuous_start(void);
void AES_encrypt(uint8_t verbose);
void AES_decrypt(uint8_t verbose);

// Variables declaration

extern struct AES_op_context aes_op_context;

// // ... from outside

extern char *cmds[10];

#endif /* INC_AES_OPERATION_H_ */
