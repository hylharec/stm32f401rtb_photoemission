/*
  ******************************************************************************
  * @file           : uart_utils.c
  * @brief          : Implement the functions related to common UART methods.
  * @author         : Raphael Viera (raphael.viera@emse.fr)
  ******************************************************************************
*/

#include <com_utils.h>

void send_answer(const char* msg, size_t len) {
  //while(HAL_DMA_GetState(&hdma_usart2_tx) != HAL_DMA_STATE_READY) ;
  while(HAL_UART_GetState(uart) != HAL_UART_STATE_READY) ;
  // Actually does not use DMA TX request as of now because of
  // errors when chaining DMA UART TX requests.
  HAL_UART_Transmit(uart, (const uint8_t *) msg, len, HAL_MAX_DELAY);
}

void split_cmds(void)
{
	int i = 0;
	strcpy((char *) Rx_data_copy, (char *) Rx_data);  // leave RX_data intact
	char *p = strtok ((char *) Rx_data_copy, ">");

	while (p != NULL)
	{
		cmds[i++] = p;
		p = strtok (NULL, ">");
	}
}

void send_ok(void)
{
  send_answer("OK\r\n", 4);
}

void send_invalid_cmd(void)
{
	char s_tmp[] = "Invalid command: ";
	strcat(s_tmp, cmds[0]);
	strcat(s_tmp, ">");
	strcat(s_tmp, cmds[1]);
	strcat(s_tmp, "\n\r");
	int s_length = strlen(cmds[0]) + 1 + strlen(cmds[1]) + 2;
	send_answer(s_tmp, s_length);
}
