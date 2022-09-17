/*
 * display.c
 *
 *  Created on: May 12, 2022
 *      Author: sebas
 */

#include "display.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_can.h"

extern CAN_HandleTypeDef hcan1;

int send_display_data()
{
	uint8_t display_data_1[8] = {0};

	CAN_TxHeaderTypeDef tx_header;
	uint32_t tx_mailbox_num;

	// TODO fill in the display data

	// tx_header for display_data_1
	tx_header.IDE = CAN_ID_STD;
	tx_header.TransmitGlobalTime = DISABLE;
	tx_header.StdId = 0x200;
	tx_header.DLC = 8;
	tx_header.RTR = 0;

	HAL_CAN_AddTxMessage(&hcan1, &tx_header, display_data_1, &tx_mailbox_num);

	return 0;
}




