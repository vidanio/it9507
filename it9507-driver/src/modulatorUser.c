#include "modulatorUser.h"

// all for Linux
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>

#include "ADF4351.h"
#include "it950x-core.h"
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"

static u32 Usb2_exitDriver (
    IN  Modulator*    modulator
) {
    u32 error = ModulatorError_NO_ERROR;

    return (error);
}


static u32 Usb2_writeControlBus (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    IN  u8*           buffer
) {
    u32     ret;
    int		  act_len;
	u8 *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

	if (pTmpBuffer) 
		memcpy(pTmpBuffer, buffer, bufferLength);
//deb_data(" ---------Usb2_writeControlBus----------\n", ret);	
	ret = usb_bulk_msg(usb_get_dev( modulator->userData),
			usb_sndbulkpipe(usb_get_dev(modulator->userData), 0x02),
			pTmpBuffer,
			bufferLength,
			&act_len,
			1000000);
   
	if (ret) deb_data(" Usb2_writeControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}


static u32 Usb2_readControlBus (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
) {
	u32     ret;
	int       nu8sRead;
	u8 *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

//deb_data(" ---------Usb2_readControlBus----------\n", ret);			
   ret = usb_bulk_msg(usb_get_dev(modulator->userData),
				usb_rcvbulkpipe(usb_get_dev(modulator->userData),129),
				pTmpBuffer,
				bufferLength,
				&nu8sRead,
				1000000);
	if (pTmpBuffer)
		memcpy(buffer, pTmpBuffer, bufferLength);   
	 
	if (ret) 	deb_data(" Usb2_readControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}

u32 EagleUser_getSystemConfig (
    IN  Modulator*    modulator,
    IN  u8          pcbIndex,
	IN  SystemConfig* Config  
) {

  /* This was originally device type 11 in the ITE driver */
	Config->restSlave		= GPIOH1;
	Config->rfEnable		= GPIOH2;
	Config->loClk			= UNUSED;
	Config->loData		= UNUSED;
	Config->loLe			= UNUSED;
	Config->lnaPowerDown	= UNUSED;
	Config->irDa			= GPIOH7;
	Config->uvFilter		= GPIOH8;
	Config->chSelect0		= UNUSED;
	Config->chSelect1		= UNUSED;		
	Config->chSelect2		= UNUSED;
	Config->chSelect3		= UNUSED;
	Config->muxSelect		= UNUSED;
	Config->uartTxd		= UNUSED;
	Config->powerDownSlave= GPIOH5;
	Config->intrEnable	= UNUSED;
	Config->lnaGain		= UNUSED;

	return 0;
}






u32 EagleUser_setSystemConfig (
    IN  Modulator*    modulator,
	IN  SystemConfig  systemConfig
) {
	u32 error = 0;
	u8 pinCnt = 0;
	
	if(systemConfig.restSlave != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.restSlave+1, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.restSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.restSlave+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
		EagleUser_delay(modulator, 10);
	}

   	if(systemConfig.lnaPowerDown != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.lnaPowerDown+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.lnaPowerDown+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loClk != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loClk+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loClk+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loData != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loData+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loData+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loLe != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loLe+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.loLe+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.muxSelect != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.muxSelect+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.muxSelect+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.powerDownSlave != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.powerDownSlave+1, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.powerDownSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.powerDownSlave+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.rfEnable != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.rfEnable+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.rfEnable+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uartTxd != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.uartTxd+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.uartTxd+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uvFilter != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.uvFilter+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.uvFilter+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.lnaGain != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.lnaGain+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)systemConfig.lnaGain+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(pinCnt>8)
		error = ModulatorError_INVALID_SYSTEM_CONFIG;

	if(error == ModulatorError_NO_ERROR)
		modulator->systemConfig = systemConfig;
exit:
    return (ModulatorError_NO_ERROR);
}

u32 EagleUser_getTsInputType (
	IN  Modulator*    modulator,
	OUT  TsInterface*  tsInStreamType
) {
	u32 error = ModulatorError_NO_ERROR;
	u8 temp = 0;
	*tsInStreamType = (TsInterface)temp;

	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49CA, &temp);
		if(error == ModulatorError_NO_ERROR){
			*tsInStreamType = (TsInterface)temp;
		}
	}
	return (error);
}

u32 EagleUser_getDeviceType (
	IN  Modulator*    modulator,
	OUT  u8*		  deviceType	   
){	
	u32 error = ModulatorError_NO_ERROR;
	u8 temp;

	
	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49D5, &temp);	
		if(error == ModulatorError_NO_ERROR){
			*deviceType = temp;	
		}else if(temp == 0){ // No eeprom 
			*deviceType = EagleUser_DEVICETYPE;
		}
	}
	else
	{
		*deviceType = 1;
	}
	
	return(error);
}




u32 EagleUser_memoryCopy (
    IN  Modulator*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  u32           count
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  memcpy(dest, src, (size_t)count);
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}

u32 EagleUser_delay (
    IN  Modulator*    modulator,
    IN  u32           dwMs
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  delay(dwMs);
     *  return (0);
     */
	msleep(dwMs); 
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_enterCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_leaveCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_mpegConfig (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_busTx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    IN  u8*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr);
     *  ack();
     *  for (i = 0; i < bufferLength; i++) {
     *      write_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_writeControlBus(modulator,bufferLength,buffer);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
    return (error);
}


u32 EagleUser_busRx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr | 0x01);
     *  ack();
     *  for (i = 0; i < bufferLength - 1; i++) {
     *      read_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  read_i2c(*(ucpBuffer + bufferLength - 1));
     *  nack();
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_readControlBus(modulator,bufferLength,buffer);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
	return (error);
}


u32 EagleUser_setBus (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    u32 error = ModulatorError_NO_ERROR;

    return(error);
}


 u32 EagleUser_Initialization  (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	 error = EagleUser_setSystemConfig(modulator, modulator->systemConfig);
	 if (error) goto exit;

	if(modulator->tsInterfaceType != StreamType_DVBT_DATAGRAM){
		
		if(modulator->systemConfig.restSlave != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, (u32)modulator->systemConfig.restSlave+1, 1); //RX(IT9133) rest 
			if (error) goto exit;
		}

		EagleUser_delay(modulator, 10);
		if(modulator->systemConfig.powerDownSlave != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, (u32)modulator->systemConfig.powerDownSlave+1, 0); //RX(IT9133) power up
			if (error) goto exit;
		}


	}

	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)modulator->systemConfig.rfEnable+1, 0); //RF out power down
		if (error) goto exit;
	}
	if(modulator->systemConfig.lnaGain != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, (u32)modulator->systemConfig.lnaGain+1, 0); //lna Gain
		if (error) goto exit;
	}
	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_busInit(modulator);
exit:
    return (error);

 }


u32 EagleUser_Finalize  (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_exitDriver(modulator);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
    return (error);

 }
 

u32 EagleUser_acquireChannel (
	IN  Modulator*    modulator,
	IN  u16          bandwidth,
	IN  u32         frequency
){

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;

	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_setFrequency(modulator, frequency);//External Lo control

	if(frequency <= 300000){ // <=300000KHz v-filter gpio set to Lo
		 if(modulator->systemConfig.uvFilter != UNUSED){
			 error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 0); 
			 if (error) goto exit;
		 }

	}else{
		 if(modulator->systemConfig.uvFilter != UNUSED){
			 error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 1); 
			 if (error) goto exit;
		 }
	}	
exit:
	return (error);
}

u32 EagleUser_setTxModeEnable (
	IN  Modulator*            modulator,
	IN  u8                    enable	
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = ModulatorError_NO_ERROR;
	if(enable){
		if(modulator->systemConfig.rfEnable != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 1); //RF power up 
			if (error) goto exit;
		}	
	}else{
		if(modulator->systemConfig.rfEnable != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 0); //RF power down 
			if (error) goto exit;
		}
		
	}
exit :
	return (error);
}


u32 EagleUser_getChannelIndex (
	IN  Modulator*            modulator,
	IN  u8*                    index	
) {

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = ModulatorError_NO_ERROR;
	u8 Freqindex = 0;
	u8 temp = 0;
	// get HW setting
	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 1); //MUX
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect0 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+3, 1);//gpiox_on
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect1 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect2 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect3 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+3, 1);//gpiox_on
		if (error) goto exit;					
	}
	//--- get HW freq setting
	if(modulator->systemConfig.chSelect0 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp);

	if(modulator->systemConfig.chSelect1 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1, &temp); 
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<1);

	if(modulator->systemConfig.chSelect2 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<2);

	if(modulator->systemConfig.chSelect3 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<3);

	error = IT9507_readRegister (modulator, Processor_LINK, 0x49E5, &temp);
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<4);
	//--------------------


	error = EagleUser_setSystemConfig (modulator, modulator->systemConfig);
	if (error) goto exit;


	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 0); //MUX					
	}

	*index = Freqindex;
exit :
	return (error);
}

