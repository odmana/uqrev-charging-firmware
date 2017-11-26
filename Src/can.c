/*
 * can.c
 *
 *  Created on: 17Nov.,2017
 *      Author: uqracing
 */

#include "can.h"
#include "debug.h"
#include "stm32f4xx_hal.h"

extern CAN_HandleTypeDef hcan1;

void can_send(can_variables* can) {
	CanTxMsgTypeDef TxMsg;
	HAL_StatusTypeDef status;

	hcan1.pTxMsg = &TxMsg;
	hcan1.pTxMsg->StdId = can->address;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->IDE = CAN_ID_STD;
	hcan1.pTxMsg->DLC = can->length;
	hcan1.pTxMsg->Data[0] = can->data.data_8[0];
	hcan1.pTxMsg->Data[1] = can->data.data_8[1];
	hcan1.pTxMsg->Data[2] = can->data.data_8[2];
	hcan1.pTxMsg->Data[3] = can->data.data_8[3];
	hcan1.pTxMsg->Data[4] = can->data.data_8[4];
	hcan1.pTxMsg->Data[5] = can->data.data_8[5];
	hcan1.pTxMsg->Data[6] = can->data.data_8[6];
	hcan1.pTxMsg->Data[7] = can->data.data_8[7];

	status = HAL_CAN_Transmit(&hcan1, 100);
	if (status != HAL_OK) {
		/* Transmition Error */
		// Error_Handler();
		debug_log("CAN transmit error");
	} else if (HAL_CAN_GetState(&hcan1) != HAL_CAN_STATE_READY) {
		// Error_Handler();
		debug_log("CAN ready error");
	} else {
		debug_log("CAN packet sent");
	}
}

/**
 * @brief  Transmission complete callback in non blocking mode
 * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
 *         the configuration information for the specified CAN.
 * @retval None
 */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
	debug_log_n("CAN packet received from 0x%X. Data ", hcan->pRxMsg->StdId);
	for (int i = 0; i < 8; i++)
		debug_log_n("%X ", hcan->pRxMsg->Data[i]);
	debug_log_n("\n");

	/* Receive */
	if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
	}
}

void can_recieve_init() {
	CAN_FilterConfTypeDef sFilterConfig;
	static CanRxMsgTypeDef RxMessage;

	hcan1.pRxMsg = &RxMessage;

	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = MB_CAN_BASE << 5;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = RX_MASK << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;

	if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
	}
}

void can_test() {
	can_variables rx_pkt;

	rx_pkt.address = 0x600;
	rx_pkt.length = 8;
	rx_pkt.data.data_16[0] = 0xCAFE;

	can_send(&rx_pkt);
}
