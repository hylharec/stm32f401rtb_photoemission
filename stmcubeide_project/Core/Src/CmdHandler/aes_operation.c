/*
 * aes_operation.c
 *
 *  Created on: Apr 18, 2023
 *      Author: hugo
 */

#include "aes_operation.h"

struct AES_op_context aes_op_context = {
    {},
    0,
    0,
    0,
    "0000000000000000",
    "0000000000000000",
    "0000000000000000",
    0,
    0,
    0
};



void AES_setup(enum AES_data_type data_type) {
  switch(data_type) {
  case KEY:
    if (strlen(cmds[2]) != 32) {
      send_answer("Error: wrong key size\r\n", 23);
    }
    else {
      hex2bytes((const uint8_t *) cmds[2], 32, aes_op_context.key, 16);
      AES_init_ctx(&(aes_op_context.aes_ctx_struct), aes_op_context.key);
      aes_op_context.key_configured = 1;
      send_ok();
    }
    break;

  case PLAIN:
    if (cmd_is(3, "trig", 4)) {
      aes_op_context.do_trig = 1;
    }
    else {
      aes_op_context.do_trig = 0;
    }

    if (strlen(cmds[2]) != 32) {
      send_answer("Error: wrong plaintext size\r\n", 29);
    }
    else {
      hex2bytes((const uint8_t *) cmds[2], 32, aes_op_context.plain, 16);
      aes_op_context.encrypt_configured = 1;
      aes_op_context.decrypt_configured = 0; // for continuous op detection
      send_ok();
    }
    break;

  case CIPHER:
    if (cmd_is(3, "trig", 4)) {
      aes_op_context.do_trig = 1;
    }
    else {
      aes_op_context.do_trig = 0;
    }

    if (strlen(cmds[2]) != 32) {
      send_answer("Error: wrong ciphertext size\r\n", 30);
    }
    else {
      hex2bytes((const uint8_t *) cmds[2], 32, aes_op_context.cipher, 16);
      aes_op_context.decrypt_configured = 1;
      aes_op_context.encrypt_configured = 0; // for continuous op detection
      send_ok();
    }
    break;
  }
}

uint8_t AES_continuous_start(void) {
  // configure context structure
  if (cmd_is(2, "trig", 4)) {
    aes_op_context.do_trig = 1;
  }
  else {
    aes_op_context.do_trig = 0;
  }

  if (aes_op_context.key_configured) {
    if (aes_op_context.encrypt_configured) {
      aes_op_context.is_encrypt = 1;
      aes_op_context.configured = 1;

      // start timer ITs
      HAL_TIM_PWM_Init(timer);
      HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_1);
      HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
      HAL_TIM_Base_Start_IT(timer);
      return 1;
    }
    else if (aes_op_context.decrypt_configured) {
      aes_op_context.is_encrypt = 0;
      aes_op_context.configured = 1;

      // start timer ITs
      HAL_TIM_PWM_Init(timer);
      HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_1);
      HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
      HAL_TIM_Base_Start_IT(timer);
      return 1;
    }
  }
  return 0; // Wrong configuration sequence
}

void AES_encrypt(uint8_t verbose) {
  uint8_t i;
  uint8_t buf[17];

  if (aes_op_context.key_configured == 0) {
    if (verbose)
      send_answer("Error: unknown key\r\n", 20);
  }
  else {
    for(i = 0; i < 16;++i)
      buf[i] = aes_op_context.plain[i];

    trigger_high(5);
    AES_ECB_encrypt(&(aes_op_context.aes_ctx_struct), buf);
    trigger_low(1);

    for(i = 0; i < 16; ++i)
        aes_op_context.cipher[i] = buf[i];

    if (verbose) {
      char answer[34];
      bytes2hex(buf, 16, (uint8_t *) answer, 32);
      answer[32] = '\r';
      answer[33] = '\n';

      send_answer(answer, 16);
      send_answer(answer+16, 16);
      send_answer(answer+32, 2);
    }
  }
}

void AES_decrypt(uint8_t verbose) {
  uint8_t i;
  uint8_t buf[16];

  if (aes_op_context.key_configured == 0) {
    if (verbose)
      send_answer("Error: unknown key\r\n", 20);
  }
  else {
    for(i = 0; i < 16;++i)
      buf[i] = aes_op_context.cipher[i];

    trigger_high(5);
    AES_ECB_decrypt(&(aes_op_context.aes_ctx_struct), buf);
    trigger_low(1);

    for(i = 0; i < 16; ++i)
      aes_op_context.plain[i] = buf[i];

    if (verbose) {
      char answer[34];
      bytes2hex(buf, 16, (uint8_t *) answer, 32);
      answer[32] = '\r';
      answer[33] = '\n';
      send_answer(answer, 16);
      send_answer(answer+16, 16);
      send_answer(answer+32, 2);
    }
  }
}
