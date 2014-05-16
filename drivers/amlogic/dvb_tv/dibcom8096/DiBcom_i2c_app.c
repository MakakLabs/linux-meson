//////////////////////////////////////
//include the .h file here

#ifdef _ZORAN_SYSTEM_
#include <stdio.h>
#include "os_defs.h"
#include "globals.h"
#include "demod.h"
#include "opentvx.h"
#include "i68dbg.h"
#include "gpio.h"
#include "i2c.h"
#include "Dmx_TsdDataRam.h"
#include "a_post.h"
#include "board_config.h"
#include "gpio.h"
#else


#endif
///////////////////////////////////////

#include "DiBcom_i2c_app.h"
#include "common.h"

extern int dib_get_fe_config(struct dib_fe_config *cfg);

unsigned char DiBcom_i2c_write(int addr, unsigned char *i2c_tx_buf, int txcnt)
{
//    printf("write the dibcom addr is 0x%x\n",addr);

#ifdef _ZORAN_SYSTEM_
    if(i2csw_write_sequence(I2C1_SCL, I2C1_SDA, (unsigned char)addr, i2c_tx_buf, txcnt) != (unsigned)txcnt)
	{
		//if write error, return 1
		return 1;
	}
#else
//replace the i2c write func
	int ret = 0;
	int i;
	struct i2c_msg msg;

	memset(&msg, 0, sizeof(msg));
	msg.addr = addr>>1;
	msg.flags = 0;
	msg.buf = i2c_tx_buf;
	msg.len = txcnt;

	struct  dib_fe_config cfg;
	dib_get_fe_config(&cfg);

	ret = i2c_transfer((struct i2c_adapter *)cfg.i2c_adapter, &msg, 1);
	if(ret<0) 
	{
		printk("DVB---- %s: writereg error, errno is %d \n", __FUNCTION__, ret);
		return 1;
	}	
		
#endif

    //write no error, return 0
    return 0;
}

unsigned char DiBcom_i2c_read(int addr, unsigned char *i2c_tx_buf, int txcnt, unsigned char *i2c_rx_buf, int rxcnt)
{
//    printf("read the dibcom addr is 0x%x\n",addr);
#ifdef _ZORAN_SYSTEM_
    if(i2csw_write_without_stop(I2C1_SCL, I2C1_SDA, (unsigned char)addr, i2c_tx_buf, txcnt) != (unsigned)txcnt)
    {
        //if write without stop error, return 1
        return 1;
    }
    addr = addr | 1;
    //replace the i2c read func
    if(i2csw_read_sequence(I2C1_SCL, I2C1_SDA, (unsigned char)addr, i2c_rx_buf, rxcnt) != (unsigned)rxcnt)
    {
        //if read error, return 1
        return 1;
    }
#else
//replace the i2c write without stop func
	int ret = 0;
	int i;
	struct i2c_msg msg;

	DiBcom_i2c_write(addr, i2c_tx_buf, txcnt);

	memset(&msg, 0, sizeof(msg));
	msg.addr = addr>>1;
	msg.flags |=  I2C_M_RD;
	msg.buf = i2c_rx_buf;
	msg.len = rxcnt;

	struct  dib_fe_config cfg;
	dib_get_fe_config(&cfg);

	ret = i2c_transfer((struct i2c_adapter *)cfg.i2c_adapter, &msg, 1);
	if(ret<0) {
		printk("DVB---- %s: readreg error, errno is %d \n", __FUNCTION__, ret);
		return 1;
	}
	
#endif
    return 0;
}

//return the ChipID // chip_8096, chip_7090
int getChipId(void)
{
	int reg;
	unsigned char tx[2];
    unsigned char rx[2];
		
	//check it is 8096 or not
	reg = 896;
	tx[0] = (reg >> 8) & 0xff;
    tx[1] = reg & 0xff;

	DiBcom_i2c_read(0x10, tx, 2, rx, 2);
	if(rx[0] == 0x01 && rx[1] == 0xb3)
	{
		reg = 897;
		tx[0] = (reg >> 8) & 0xff;
    		tx[1] = reg & 0xff;
		DiBcom_i2c_read(0x10, tx, 2, rx, 2);
		if(rx[0] == 0x80 && rx[1] == 0x90)
			return CHIPID_8096;
	}

	//check it is 7090 or not
	reg = 768;
	tx[0] = (reg >> 8) & 0xff;
    tx[1] = reg & 0xff;

	DiBcom_i2c_read(0x10, tx, 2, rx, 2);
	if(rx[0] == 0x01 && rx[1] == 0xb3)
	{
		reg = 769;
		tx[0] = (reg >> 8) & 0xff;
    		tx[1] = reg & 0xff;
		DiBcom_i2c_read(0x10, tx, 2, rx, 2);
		if(rx[0] == 0x40 && rx[1] == 0x00)
			return CHIPID_7090;
	}

	return CHIPID_ERROR;
	
}

int test_dibcom(void)
{
#if 1
    int reg = 896;
    unsigned char tx[2];
    unsigned char rx[2];
    unsigned char temp;

    printf("test dibcom\n");

    tx[0] = (reg >> 8) & 0xff;
    tx[1] = reg & 0xff;
    //temp = tx[0]; tx[0] = tx[1]; tx[1] = temp;

    //for(; ;)
    {
        DiBcom_i2c_read(0x10, tx, 2, rx, 2);
        printf("tx[0] = %#x, tx[1] = %#x, rx[0] = %#x, rx[1] = %#x\n", tx[0], tx[1], rx[0], rx[1]);
    }
    if(rx[0] == 0x01 && rx[1] == 0xb3)
    {
    	  printk("dibcom read ok!\n");
        return 0;    // ok
    }
    else
        return 1;    // fail
#endif
}

