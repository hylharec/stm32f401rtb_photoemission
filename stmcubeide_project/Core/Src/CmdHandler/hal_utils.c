/*
  ******************************************************************************
  * @file           : hal_utils.c
  * @brief          : Implement the functions related to common HAL methods.
  * @author         : Raphael Viera (raphael.viera@emse.fr)
  ******************************************************************************
*/

#include "hal_utils.h"

/*
 * hal_utils.c
 *
 *  Created on: May 21, 2021
 *      Author: raphael.viera
 */

/**
  * @brief  This function is responsible for toggling PIN3 (LED).
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void blink_LED(uint32_t ntimes, uint32_t delay_ms)
{
	int i;

	for (i = 1; i <= ntimes*2; ++i)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(delay_ms);
	}
}

/**
  * @brief  This function is responsible for resetting PIN3 (LED).
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void LED_OFF(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  This function is responsible for setting PIN3 (LED).
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void LED_ON(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}


/**
  * @brief  This function is responsible for setting the trigger.
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void trigger_high(uint32_t n_nops)
{
    HAL_GPIO_WritePin(TRIG_OUT0_GPIO_Port, TRIG_OUT0_Pin, GPIO_PIN_SET);
    do_NOP(n_nops);
}

/**
  * @brief  This function is responsible for resetting the trigger.
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void trigger_low(uint32_t n_nops)
{
    do_NOP(n_nops);
    HAL_GPIO_WritePin(TRIG_OUT0_GPIO_Port, TRIG_OUT0_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  This function is responsible for setting up the trigger.
  * @author Raphael Viera (raphael.viera@emse.fr)
  */
void trigger_setup(void)
{
  do_NOP(1);
  HAL_GPIO_WritePin(TRIG_OUT0_GPIO_Port, TRIG_OUT0_Pin, GPIO_PIN_RESET);
}
