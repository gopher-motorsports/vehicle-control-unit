/*
 * main_task.c
 *
 *  Created on: Apr 24, 2022
 *      Author: sebas
 */
#include "main_task.h"
#include "DAM.h"
#include "inverter_can.h"
#include "display.h"
#include "cmsis_os.h"

extern CAN_HandleTypeDef hcan1;
DRIVE_STATE_t curr_state = NOT_READY_STATE;
U32 rtd_press_time = 0;


void inverter_ctrl_task(void)
{
	while(1)
	{
		handle_inverter(&curr_state);
		osDelay(INVERTER_TASK_DELAY);
	}
}


// DO NOT DELETE
int main_task()
{
	// TODO be smart about when turing on pump and fan
	HAL_GPIO_WritePin(PUMP_CTRL_GPIO_Port, PUMP_CTRL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FAN_CTRL_GPIO_Port, FAN_CTRL_Pin, GPIO_PIN_SET);

	// Update the GCAN params for gsense??
	// TODO

	// handle the state machine with resets and RTD and such
	if (curr_state == NOT_READY_STATE)
	{
		// stop buzzing
		HAL_GPIO_WritePin(RTD_BZZR_GPIO_Port, RTD_BZZR_Pin, GPIO_PIN_RESET);

		if (!HAL_GPIO_ReadPin(RTD_BTN_GPIO_Port, RTD_BTN_Pin)
				&& vcu_apps2.data <= APPS2_MIN_TRAVEL_mm)
		{
 			curr_state = READY_STATE;
			rtd_press_time = HAL_GetTick();
		}
	}
	else
	{
		// driving state
		if (HAL_GetTick() - rtd_press_time >= BUZZER_TIME_ms)
		{
			// we dont need to buzz anymore
			HAL_GPIO_WritePin(RTD_BZZR_GPIO_Port, RTD_BZZR_Pin, GPIO_PIN_RESET);
		}
		else
		{
			if (get_inv_state() == INV_ENABLED)
			{
				// start buzzing
				HAL_GPIO_WritePin(RTD_BZZR_GPIO_Port, RTD_BZZR_Pin, GPIO_PIN_SET);
			}
		}
	}

	// check for brake pressure to run the brake light
	if (vcu_brake_pressure_front.data >= BRAKE_PRESSURE_BREAK_THRESH_psi)
	{
		HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);
	}


	// Heartbeat LED
	static uint32_t last_heartbeat;
	if (HAL_GetTick() - last_heartbeat > 500)
	{
		last_heartbeat = HAL_GetTick();
		HAL_GPIO_TogglePin(HEARTBEAT_GPIO_Port, HEARTBEAT_Pin);
	}

	// Update Display
	static uint32_t last_display_update;
	if (HAL_GetTick() - last_display_update > 100)
	{
		last_display_update = HAL_GetTick();
		send_display_data();
	}

	return 0;
}

