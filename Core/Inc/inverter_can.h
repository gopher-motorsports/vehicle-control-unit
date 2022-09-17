/*
 * inverter_can.h
 *
 *  Created on: Jun 16, 2022
 *      Author: Cal
 */

#ifndef INVERTER_CAN_H
#define INVERTER_CAN_H

#include "base_types.h"
#include "main_task.h"
#include "stdbool.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_can.h"

// pedal and pressure parameters
#define APPS2_MIN_TRAVEL_mm 7
#define APPS2_MAX_TRAVEL_mm 17
#define BRAKE_PRESSURE_BREAK_THRESH_psi 140

#define INVERTER_TASK_DELAY 1
#define MAX_TORQUE_Nm 140


// important CAN IDs
#define COMMAND_MSG_ID 0x0C0
#define PARAM_RW_CMD_ID 0x0C1
#define PARAM_RW_RESP_ID 0x0C2
#define INTERNAL_STATES_ID 0x0AA

typedef enum
{
	INV_LOCKOUT = 0,
	INV_DISABLED = 1,
	INV_ENABLED = 2
} INV_CTRL_STATE_t;


// standard command messages
typedef struct
{
	U16 torque_cmd; // bytes 0-1
	U16 spd_cmd; // bytes 2-3
	U8 dir_cmd; // byte 4
	U8 inverter_en; // byte 5 bit 0
	U8 discharge_en; // byte 5 bit 1
	U8 speed_en; // byte 5 bit 2
	U16 torque_lim; // bytes 6-7
} CMD_CAN_t;


// defines for how the parameters commands are set up
typedef struct
{
	U16 param_addr; // bytes 0-1
	U8 read_or_write; // byte 2
	U8 reserved; // byte 3
	U16 data; // byte 4-5
	U16 reserved_2; // bytes 6-7
} INV_RW_CMD_CAN_t;

#define READ_CMD 0
#define WRITE_CMD 1

typedef struct
{
	U16 param_addr; // bytes 0-1
	U8 write_success; // byte 2
	U8 reserved; // byte 3
	U16 data; // byte 4-5
	U16 reserved_2; // bytes 6-7
} INV_RW_RESPONCE_CAN_t;


// parameters for the inverter commands
typedef enum
{
	RELAY_CMD = 1, // U16, 0-65535
	FLUX_CMD = 10, // Flux
	RESOLVER_PWM_DELAY_CMD = 11, // U16, 0-6250
	GAMMA_ADJUST_CMD = 12, // Degrees
	FAULT_CLEAR = 20, // Bool, active low
	SET_PWM_FREQ = 21, // U16, 6-24
	AIN_PU_CTRL = 22, // U16, 0-7
	SHUDDER_COMP_GAIN_CTRL = 23, // U16, 0-65535
	DIAG_DATA_TRIGGER = 31, // U16, 0-65535
} INVERTER_PARAMETERS_t;


// internal state stuff
typedef struct
{
	U8 vsm_state; // byte 0
	U8 pwm_freq; // byte 1
	U8 inverter_state; // byte 2
	U8 relay_state; // byte 3

	U8 inverter_run_mode; // byte 4 bit 0
	U8 inv_active_dis_state; // byte 4, bits 5-7

	U8 inv_cmd_mode; // byte 5, bit 0
	U8 rolling_counter_val; // byte 5 bits 4-7

	U8 inv_en_state; // byte 6, bit 0
	U8 start_mode_active; // byte 6, bit 6
	U8 inv_enable_lockout; // byte 6, bit 7

	U8 dir_cmd; // byte 7, bit 0
	U8 bms_active; // byte 7, bit 1
	U8 bms_limit_torque; // byte 7, bit 2
	U8 limit_max_speed; // byte 7, bit 3
	U8 limit_hot_spot; // byte 7, bit 4
	U8 low_speed_limiting; // byte 7, bit 5
	U8 coolant_temp_limiting; // byte 7, bit 6
} INTERNAL_STATES_t;


// all of the different values the Inverter transmits over CAN



typedef enum
{
	POWER_ON_STATE = 0,
	STOP_STATE = 1,
	OPEN_LOOP_STATE = 2,
	CLOSED_LOOP_STATE = 3,
	WAIT_STATE = 4,
	INTERNAL_STATE_5 = 5,
	INTERNAL_STATE_6 = 6,
	INTERNAL_STATE_7 = 7,
	IDLE_RUN_STATE = 8,
	IDLE_STOP_STATE = 9,
	INTERNAL_STATE_10 = 10,
	INTERNAL_STATE_11 = 11,
	INTERNAL_STATE_12 = 12,
} INV_STATE_t;


// function prototypes
INV_CTRL_STATE_t get_inv_state(void);
void handle_inverter(DRIVE_STATE_t* curr_state);
void vcu_custom_can_callback(CAN_HandleTypeDef* hcan, U32 rx_mailbox);
void build_cmd_msg(U8* data, CMD_CAN_t* cmd_struct);
void build_param_cmd_msg(U8* data, INV_RW_CMD_CAN_t* read_cmd);


#endif /* INVERTER_CAN_H */
