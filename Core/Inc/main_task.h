/*
 * main_task.h
 *
 *  Created on: Apr 24, 2022
 *      Author: sebas
 */

#ifndef INC_MAIN_TASK_H_
#define INC_MAIN_TASK_H_

#define BUZZER_TIME_ms 4000

typedef enum
{
	NOT_READY_STATE = 0,
	READY_STATE = 1
} DRIVE_STATE_t;

void inverter_ctrl_task(void);
int main_task();




#endif /* INC_MAIN_TASK_H_ */
