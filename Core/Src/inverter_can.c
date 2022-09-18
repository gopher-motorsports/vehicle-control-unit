

#include "inverter_can.h"
#include "cmsis_os.h"
#include "GopherCAN.h"

static void update_inv_state(void);
static S8 send_can_message(U32 id, U8 dlc, U8* data);

INTERNAL_STATES_t inv_states = {0};
CMD_CAN_t CAN_cmd =
{
		.torque_cmd = 0,
		.spd_cmd = 0,
		.dir_cmd = 0,
		.inverter_en = 0,
		.discharge_en = 0,
		.speed_en = 0,
		.torque_lim = MAX_TORQUE_Nm,
};
INV_RW_CMD_CAN_t rw_cmd = {0};
INV_CTRL_STATE_t inv_ctrl_state = INV_LOCKOUT;
extern CAN_HandleTypeDef hcan2;

INV_CTRL_STATE_t get_inv_state(void)
{
	return inv_ctrl_state;
}

void handle_inverter(DRIVE_STATE_t* curr_state)
{
	U8 msg_data[8];
	U16 torque_req = 0;

	update_inv_state();

	// find the state of the inverter based on the status of the ECU
	if (inv_states.inv_enable_lockout)
	{
		inv_ctrl_state = INV_LOCKOUT;
	}
	else if (!inv_states.inv_en_state)
	{
		// not locked out, not enabled
		inv_ctrl_state = INV_DISABLED;

		// disable the state
		*curr_state = NOT_READY_STATE;
	}
	else
	{
		inv_ctrl_state = INV_ENABLED;
	}

	// if the inverter is locked out, we must first disable and enable the motor
	switch (inv_ctrl_state)
	{
	case INV_LOCKOUT:
		// send the disable then enable command with some delay
		CAN_cmd.inverter_en = 0;
		build_cmd_msg(msg_data, &CAN_cmd);
		send_can_message(COMMAND_MSG_ID, 8, msg_data);

		// also clear all faults when first starting up
		rw_cmd.param_addr = FAULT_CLEAR;
		rw_cmd.read_or_write = WRITE_CMD;
		rw_cmd.data = 0;
		build_param_cmd_msg(msg_data, &rw_cmd);
		send_can_message(PARAM_RW_CMD_ID, 8, msg_data);
		break;

	case INV_DISABLED:
		// try to enable the motor
		if (*curr_state == READY_STATE)
		{
			CAN_cmd.inverter_en = 1;
			build_cmd_msg(msg_data, &CAN_cmd);
			send_can_message(COMMAND_MSG_ID, 8, msg_data);
		}
		break;

	case INV_ENABLED:
		// actually run the motor

		// calculate the torque we want from the motor. Right now we are linear
		if (vcu_brake_pressure_front.data <= BRAKE_PRESSURE_BREAK_THRESH_psi)
		{
			float curr_pp = vcu_apps2.data;
			if (curr_pp >= APPS2_MAX_TRAVEL_mm)
			{
				torque_req = MAX_TORQUE_Nm;
			}
			else if (curr_pp <= APPS2_MIN_TRAVEL_mm)
			{
				torque_req = 0;
			}
			else
			{
				// linearly interpolate
				// TODO
			}
		}

		CAN_cmd.torque_cmd = torque_req;
		build_cmd_msg(msg_data, &CAN_cmd);
		send_can_message(COMMAND_MSG_ID, 8, msg_data);
		break;

	default:
		// not sure how we are here
		inv_ctrl_state = INV_LOCKOUT;
		break;
	}
}


static void update_inv_state(void)
{
	// read all of the internal states and fill in the structs
	inv_states.vsm_state = inv_vsm_state.data;
	inv_states.pwm_freq = inv_pwm_freq.data;
	inv_states.inverter_state = inv_inverter_state.data;
	inv_states.relay_state = inv_relay_state.data;

	inv_states.inverter_run_mode = !!(inv_states_byte4.data & 0b00000001);
	inv_states.inv_active_dis_state = (inv_states_byte4.data & 0b01110000) >> 4;

	inv_states.inv_cmd_mode = !!(inv_states_byte5.data & 0b00000001);
	inv_states.rolling_counter_val = (inv_states_byte5.data & 0b01111000) >> 3;

	inv_states.inv_en_state = !!(inv_states_byte6.data & 0b00000001);
	inv_states.start_mode_active = !!(inv_states_byte6.data & 0b01000000);
	inv_states.inv_enable_lockout = !!(inv_states_byte6.data & 0b10000000);

	inv_states.dir_cmd = !!(inv_states_byte7.data & 0b00000001);
	inv_states.bms_active = !!(inv_states_byte7.data & 0b00000010);
	inv_states.bms_limit_torque = !!(inv_states_byte7.data & 0b00000100);
	inv_states.limit_max_speed = !!(inv_states_byte7.data & 0b00001000);
	inv_states.limit_hot_spot = !!(inv_states_byte7.data & 0b00010000);
	inv_states.low_speed_limiting = !!(inv_states_byte7.data & 0b00100000);
	inv_states.coolant_temp_limiting = !!(inv_states_byte7.data & 0b01000000);
}


// build_cmd_msg
//  Builds the data U8[] based on the passed in cmd_struct
void build_cmd_msg(U8* data, CMD_CAN_t* cmd_struct)
{
	// all data is stored in the message as low byte than high byte
	data[0] = cmd_struct->torque_cmd & 0x00FF;
	data[1] = (cmd_struct->torque_cmd & 0xFF00) >> 8;
	data[2] = cmd_struct->spd_cmd & 0x00FF;
	data[3] = (cmd_struct->spd_cmd & 0xFF00) >> 8;
	data[4] = cmd_struct->dir_cmd;

	data[5] = 0;
	data[5] |= (cmd_struct->inverter_en ? 0x01 : 0x00);
	data[5] |= (cmd_struct->discharge_en ? 0x02 : 0x00);
	data[5] |= (cmd_struct->speed_en ? 0x04 : 0x00);

	data[6] = cmd_struct->torque_lim & 0x00FF;
	data[7] = (cmd_struct->torque_lim & 0xFF00) >> 8;
}


void build_param_cmd_msg(U8* data, INV_RW_CMD_CAN_t* read_cmd)
{
	data[0] = read_cmd->param_addr & 0x00FF;
	data[1] = (read_cmd->param_addr & 0xFF00) >> 8;
	data[2] = read_cmd->read_or_write;
	data[3] = 0; // reserved
	data[4] = read_cmd->data & 0x00FF;
	data[5] = (read_cmd->data & 0xFF00) >> 8;
	data[6] = 0; // reserved
	data[7] = 0; // reserved
}


// send_can_message
//  function to build a tx_header and add the message to the BXCan hardware
static S8 send_can_message(U32 id, U8 dlc, U8* data)
{
	CAN_TxHeaderTypeDef tx_header;
	U32 tx_mailbox_num;

	tx_header.IDE = CAN_ID_STD;                                          // 29 bit id
	tx_header.TransmitGlobalTime = DISABLE;                              // do not send a timestamp
	tx_header.StdId = id;
	tx_header.RTR = DATA_MESSAGE;
	tx_header.DLC = dlc;

	HAL_CAN_AddTxMessage(&hcan2, &tx_header, data, &tx_mailbox_num);

	return 0;
}


