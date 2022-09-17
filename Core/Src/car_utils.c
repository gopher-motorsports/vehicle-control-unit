/*
 * car_utils.c
 *
 *  Created on: Apr 24, 2022
 *      Author: sebas
 */

#include <math.h>
#include "car_utils.h"
#include "DAM.h"
#include "GopherCAN.h"

extern TIM_HandleTypeDef htim2;

#define TRANS_ARR_SIZE 100


uint32_t trans_speed_arr[TRANS_ARR_SIZE] = {0};
uint32_t trans_idx = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	// TODO - ensure that this is the correct way to ensure that the timer is correct
	if (htim == &htim2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			trans_speed_arr[trans_idx++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			trans_idx = trans_idx % TRANS_ARR_SIZE;
		}

		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			// TODO use WSFR too
		}
	}
}

// Return trans_speed from input capture in Hz. If no hall pulse within the last 5ms returns 0 speed
uint32_t get_trans_speed(void)
{
	// Disable IC IT to prevent concurrent write/read
	HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
	// 96 MHz APB1 CLK prescaled to 16 MHz. 5/1000 * 16,000,000 = 80,000 ticks
	uint32_t five_ms_in_timer = 80000;

	// We are storing the timer 2 value with each transmission pulse. To find the time difference
	// subtract arr[n] - arr[n-1] keeping in mind loop around.
	// Average the last 5 ms of transmission speed data, if there is no data within the last 5 ms then return 0
	int32_t idx_earlier, idx_later;
	uint32_t sum_of_differences = 0, num_elems = 0;
	uint32_t curr_tim = __HAL_TIM_GET_COUNTER(&htim2);
	for (uint32_t i = 0; i < TRANS_ARR_SIZE - 1; i++)
	{
		// Get two consecutive elements from array
		// Time between sensor pulses is then trans_speed_arr[idx_later] - trans_speed_arr[idx_earlier]
		// Subtract 2 since 1 before idx_later
		idx_earlier = (trans_idx - i) - 2;
		// Subtract 1 since trans_idx contains next available index
		idx_later = (trans_idx - i) - 1;
		// Handle negative rollover
		if (idx_earlier < 0) idx_earlier += TRANS_ARR_SIZE;
		if (idx_later < 0) idx_later += TRANS_ARR_SIZE;
		uint32_t difference = trans_speed_arr[idx_later] - trans_speed_arr[idx_earlier];
		// If the value that we are looking at is older than 5 ms then return
		if (curr_tim - trans_speed_arr[idx_later] > five_ms_in_timer)
		{
			// Re-enable interrupt upon completion
			HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
			// Prevent div by 0 and return 16MHz clock divided by average tim diff
			return num_elems == 0 || sum_of_differences == 0 ? 0 : 16000000/(sum_of_differences/num_elems);
		}
		// Item is newer than 5ms. Add it to sum and increment num_elems for averaging
		sum_of_differences += difference;
		num_elems++;
	}
	// Re-enable interrupt upon completion
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	// Prevent div by 0 and return 16MHz clock divided by average tim diff
	return num_elems == 0 || sum_of_differences == 0 ? 0 : 16000000/(sum_of_differences/num_elems);
}

