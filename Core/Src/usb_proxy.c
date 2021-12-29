/*
 * usb_proxy.c
 *
 *  Created on: Dec 15, 2021
 *      Author: ray
 */

#include "usb_proxy.h"

USBProxyHandler hUSB;
CRC_HandleTypeDef hcrc;

void USB_Init(USBD_HandleTypeDef *usb_handler) {
	hUSB.husbd = usb_handler;
	hUSB.invalidRxMsgCount = 0;
	hUSB.ifNewCargo = 0;
	hUSB.ifNewDataLogPiece2Send = 0;
	hUSB.index.b32 = 0;
	hUSB.ifDataLogInitiated = 0;
	hUSB.ifDataLogStarted = 0;
}

void USB_Transmit_Cargo(uint8_t *buf, uint8_t size) {
	uint8_t tx_buf[size + 8];
	tx_buf[0] = 0xAA;
	tx_buf[1] = 0xCC;
	tx_buf[2] = size;
	memcpy(&tx_buf[3], buf, size);

	union int32uint8_t crc;
	uint32_t buf32bit[size + 1]; //Include the byte number one
	for (uint8_t i = 0; i <= size; i++) {
		buf32bit[i] = (uint32_t) tx_buf[i + 2];
	}
	crc.b32 = HAL_CRC_Calculate(&hcrc, buf32bit, size + 1);
	tx_buf[size + 3] = crc.b8[0];
	tx_buf[size + 4] = crc.b8[1];
	tx_buf[size + 5] = crc.b8[2];
	tx_buf[size + 6] = crc.b8[3];
//	tx_buf[size + 3] = 0x11;
//	tx_buf[size + 4] = 0x22;
//	tx_buf[size + 5] = 0x33;
//	tx_buf[size + 6] = 0x44;
	tx_buf[size + 7] = 0x55;
	CDC_Transmit_FS(tx_buf, sizeof(tx_buf));
}

void USB_ReceiveCpltCallback(void) {
	memset(hUSB.rxMsgRaw, 0, sizeof(hUSB.rxMsgRaw));
	memcpy(hUSB.rxMsgRaw, hUSB.buf, *hUSB.len);
	USB_Receive_Cargo();
}

void USB_Receive_Cargo(void) {
	if (hUSB.rxMsgRaw[0] == (char) 0xBB && hUSB.rxMsgRaw[1] == (char) 0xCC
			&& hUSB.rxMsgRaw[*hUSB.len - 1] == (char) 0x88) {
		if (hUSB.rxMsgRaw[2] == *hUSB.len - 8) {
			uint32_t crcCalculatedResult;
			union int32uint8_t crcReceive;

			hUSB.rxMessageLen = hUSB.rxMsgRaw[2];
			uint32_t crcCalculate[hUSB.rxMessageLen + 1];
			for (uint8_t i = 0; i <= hUSB.rxMessageLen; i++)
				crcCalculate[i] = (uint32_t) hUSB.rxMsgRaw[2 + i];
			crcCalculatedResult = HAL_CRC_Calculate(&hcrc, crcCalculate,
					hUSB.rxMessageLen + 1);

			crcReceive.b8[0] = hUSB.rxMsgRaw[*hUSB.len - 5];
			crcReceive.b8[1] = hUSB.rxMsgRaw[*hUSB.len - 4];
			crcReceive.b8[2] = hUSB.rxMsgRaw[*hUSB.len - 3];
			crcReceive.b8[3] = hUSB.rxMsgRaw[*hUSB.len - 2];

			if (crcCalculatedResult == crcReceive.b32) {
				memcpy(hUSB.rxMessageCfrm, &hUSB.rxMsgRaw[3],
						hUSB.rxMessageLen);
				hUSB.ifNewCargo = 1;
			}
		} else {
			hUSB.invalidRxMsgCount++;
			return;
		}
	} else {
		hUSB.invalidRxMsgCount++;
		return;
	}
}

void USB_DataLogInitialization(void) {
	hUSB.dataLogBytes = 0;
	hUSB.ifNewDataLogPiece2Send = 0;
	hUSB.ifDataLogInitiated = 1;
	hUSB.index.b32 = 0;
}

void DataLog_CargoTransmit(SendFormat *send_format) {
	uint8_t i = 0;
	union uint32uint8_t sysTick;
	sysTick.b32 = HAL_GetTick();
	//Index
	hUSB.txBuf[i++] = hUSB.index.b8[0];
	hUSB.txBuf[i++] = hUSB.index.b8[1];
	hUSB.txBuf[i++] = hUSB.index.b8[2];
	hUSB.txBuf[i++] = hUSB.index.b8[3];
	hUSB.index.b32++;
	//Time stamp
	hUSB.txBuf[i++] = sysTick.b8[0];
	hUSB.txBuf[i++] = sysTick.b8[1];
	hUSB.txBuf[i++] = sysTick.b8[2];
	hUSB.txBuf[i++] = sysTick.b8[3];
	//Position
	hUSB.txBuf[i++] = send_format->voltage.b8[0];
	hUSB.txBuf[i++] = send_format->voltage.b8[1];
	//Duty cycle
	hUSB.txBuf[i++] = send_format->duty_cycle_1.b8[0];
	hUSB.txBuf[i++] = send_format->duty_cycle_1.b8[1];
	////Duty cycle
	hUSB.txBuf[i++] = send_format->duty_cycle_2.b8[0];
	hUSB.txBuf[i++] = send_format->duty_cycle_2.b8[1];
	//Current
	hUSB.txBuf[i++] = send_format->current_1.b8[0];
	hUSB.txBuf[i++] = send_format->current_1.b8[1];
	//Current
	hUSB.txBuf[i++] = send_format->current_2.b8[0];
	hUSB.txBuf[i++] = send_format->current_2.b8[1];
	//Velocity
	hUSB.txBuf[i++] = send_format->velocity_1.b8[0];
	hUSB.txBuf[i++] = send_format->velocity_1.b8[1];
	//Velocity
	hUSB.txBuf[i++] = send_format->velocity_2.b8[0];
	hUSB.txBuf[i++] = send_format->velocity_2.b8[1];
	//Temperature
	hUSB.txBuf[i++] = send_format->temperature_1.b8[0];
	hUSB.txBuf[i++] = send_format->temperature_1.b8[1];
	//Temperature
	hUSB.txBuf[i++] = send_format->temperature_2.b8[0];
	hUSB.txBuf[i++] = send_format->temperature_2.b8[1];

	hUSB.dataLogBytes = 26;
	USB_Transmit_Cargo(hUSB.txBuf, hUSB.dataLogBytes);
}

void DataLog_Manager(SendFormat *send_format) {
	if (hUSB.ifDataLogStarted)
		DataLog_CargoTransmit(send_format);
}

void USB_DataLogStart(void) {
	USB_DataLogInitialization();
	hUSB.ifDataLogStarted = 1;
	char start_msg[] = "Datalog start";
	USB_Transmit_Cargo((uint8_t*) start_msg, sizeof(start_msg));
}

void USB_DataLogEnd(void) {
	hUSB.ifDataLogStarted = 0;
	char end_msg[] = "Datalog end";
	USB_Transmit_Cargo((uint8_t*) end_msg, sizeof(end_msg));
}
