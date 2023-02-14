/*
 * vishwaModbus.c
 *
 *  Created on: Aug 22, 2022
 *      Author: vishweshgm
 */


#include "vishwaModbus.h"

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mbm.h"
#include "common/mbportlayer.h"
#include "common/mbtypes.h"

typedef union {
	float a;
	uint8_t bytes[4];
} rs485float;

rs485float whreceived;
rs485float vahreceived;
rs485float frequency;
rs485float frequency;
rs485float frequency;

uint32_t modbuserror;
eMBErrorCode eStatus=MB_ENOERR, eStatus2=MB_ENOERR;
volatile unsigned long timeCounterSecondOld=0;

xMBHandle       xMBMMaster;
USHORT          usNRegs[2*3];
UBYTE			ubNRegs[2*3*2];

void vishwaModbusInit(void){
	eStatus = eMBMSerialInit( &xMBMMaster, MB_RTU, MBM_SERIAL_PORT, 9600, MB_PAR_EVEN );
}

void vishwaModbusLoop(void){
	static uint16_t vish;
	eStatus = eMBMReadHoldingRegisters( xMBMMaster, 1, 156,6,usNRegs );
	if(eStatus != MB_ENOERR){
		modbuserror++;
	}
	for(int m=0,n=0;m<2*3;m++)
	{
		ubNRegs[n] = (UBYTE)(usNRegs[m]>>8);
		ubNRegs[n+1] = (UBYTE)usNRegs[m];
		n+=2;
	}
	vish = 0;
	frequency.bytes[0] = ubNRegs[vish+1];
	frequency.bytes[1] = ubNRegs[vish+0];
	frequency.bytes[2] = ubNRegs[vish+3];
	frequency.bytes[3] = ubNRegs[vish+2];
	vish+=4;
	whreceived.bytes[0] = ubNRegs[vish+1];
	whreceived.bytes[1] = ubNRegs[vish+0];
	whreceived.bytes[2] = ubNRegs[vish+3];
	whreceived.bytes[3] = ubNRegs[vish+2];
	vish+=4;
	vahreceived.bytes[0] = ubNRegs[vish+1];
	vahreceived.bytes[1] = ubNRegs[vish+0];
	vahreceived.bytes[2] = ubNRegs[vish+3];
	vahreceived.bytes[3] = ubNRegs[vish+2];
}
