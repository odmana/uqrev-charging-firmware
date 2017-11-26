/*
 * temp.c
 *
 *  Created on: 26Nov.,2017
 *      Author: uqracing
 */

#include "stm32f4xx_hal.h"
#include "temp.h"
#include "debug.h"

extern TIM_HandleTypeDef htim2;

uint8_t mb_comms = 0x3F;
uint8_t mb_temps = 0x00;

void temp_check(can_variables rx_pkt) {
	int8_t mb_val = -1;

	switch (rx_pkt.address) {
		case MB_CAN_BASE + MB_1:
			mb_val = 0;
			break;
		case MB_CAN_BASE + MB_2:
			mb_val = 1;
			break;
		case MB_CAN_BASE + MB_3:
			mb_val = 2;
			break;
		case MB_CAN_BASE + MB_4:
			mb_val = 3;
			break;
		case MB_CAN_BASE + MB_5:
			mb_val = 4;
			break;
		case MB_CAN_BASE + MB_6:
			mb_val = 5;;
			break;
	}

	// ignore if can packet not from motherboards
	if (mb_val < 0)
		return;

	// set or reset overtemp flag
	if (rx_pkt.data.data_16[1] > TEMP_THRESH)
		mb_temps |= 0x01 << mb_val;
	else
		mb_temps &= ~(0x01 << mb_val);

	// set comms recieved flag
	mb_comms &= ~(0x01 << mb_val);

	if (mb_temps) {
		// do warning thing

		if (rx_pkt.data.data_16[1] > TEMP_THRESH)
			debug_log("Motherboard overtemp [Motherboard: 0x%X, Temp: %d]", mb_temps, rx_pkt.data.data_16[1]);
	}
}

void temp_init() {
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) {
		/* Starting Error */
		Error_Handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	static uint8_t first_flag = 1;

	// timer is triggered when it is started
	if (first_flag) {
		first_flag = 0;
		return;
	}

	// check timeout for all motherboards
	if (mb_comms) {
		// do warning thing
		debug_log("Motherboard timeout [0x%X]", mb_comms);
	}

	// reset flag
	mb_comms = 0x3F;
}
