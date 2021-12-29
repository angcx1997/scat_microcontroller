/*
 * usb_proxy.h
 *
 *  Created on: Dec 15, 2021
 *      Author: ray
 */


#ifndef USB_PROXY_H
#define USB_PROXY_H

#include "stm32f4xx.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

union uint32uint8_t{
	uint32_t b32;		//original data
	uint8_t b8[4];	//data in byte size
};


union int32uint8_t{
	int b32;		//original data
	uint8_t b8[4];	//data in byte size
};

union int16uint8_t{
	int16_t b16;		//original data
	uint8_t b8[2];	//data in byte size
};

typedef struct
{
  USBD_HandleTypeDef*   husbd;
  /*Tx Message*/
  uint8_t              txBuf[256];
  /*Rx Message*/
  uint8_t*              buf;
  uint32_t*             len;
  char                  rxMsgRaw[264];
  uint8_t               msgDetectStage;
  uint8_t               bytesToRead;
  uint8_t               rxMessageCfrm[256];
  uint8_t               rxMessageLen;
  uint32_t              invalidRxMsgCount;
  uint8_t               ifNewCargo;
  /*Data log*/
  uint8_t               dataLogBytes;
  uint8_t               ifNewDataLogPiece2Send;
  uint8_t               ifDataLogInitiated;
  uint8_t               ifDataLogStarted;
  union int32uint8_t    index;

}USBProxyHandler;

typedef struct
{
	union int16uint8_t voltage;
	union int16uint8_t duty_cycle_1;
	union int16uint8_t duty_cycle_2;
	union int16uint8_t current_1;
	union int16uint8_t current_2;
	union int16uint8_t velocity_1;
	union int16uint8_t velocity_2;
	union int16uint8_t temperature_1;
	union int16uint8_t temperature_2;
}SendFormat;

void USB_Init(USBD_HandleTypeDef *usb_handler);
void USB_Transmit_Cargo(uint8_t* buf, uint8_t size);
void USB_ReceiveCpltCallback(void);
void USB_Receive_Cargo(void);
void USB_DataLogInitialization(void);
void USB_DataLogStart(void);
void USB_DataLogEnd(void);
void DataLog_CargoTransmit(SendFormat *send_format);
void DataLog_Manager(SendFormat *send_format);

extern USBProxyHandler hUSB;
extern CRC_HandleTypeDef hcrc;

#endif
