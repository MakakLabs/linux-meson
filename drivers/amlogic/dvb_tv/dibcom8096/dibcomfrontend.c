/*****************************************************************
**
**  Copyright (C) 2009 Amlogic,Inc.
**  All rights reserved
**        Filename : itefrontend.c
**
**  comment:
**        Driver for ITE9173 demodulator
**  author :
**	      Shanwu.Hu@amlogic
**  version :
**	      v1.0	 12/5/21
*****************************************************************/

/*
    Driver for GX1001 demodulator
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#ifdef ARC_700
#include <asm/arch/am_regs.h>
#else
#include <mach/am_regs.h>
#endif
#include <linux/i2c.h>
//#include <linux/gpio.h>
//#include <mach/gpio_data.h>
#include <mach/gpio.h>
//#include <mach/gpio_old.h>
#include <linux/amlogic/aml_gpio_consumer.h>

#include "dibcomfrontend.h"
#include "../aml_fe.h"

#include "dibcom_app.h"
#include "frontend.h"
#if 1
#define pr_dbg(fmt, args...) printk( "DVB: " fmt, ## args)
//#define pr_dbg(fmt, args...) printk( KERN_DEBUG"DVB: " fmt, ## args)
#else
#define pr_dbg(fmt, args...)
#endif

#define pr_error(fmt, args...) printk( KERN_ERR"DVB: " fmt, ## args)

#define DIBCOM_POWER_MODULE_NAME   "dibcom_power"

static int frontenddib_reset = -1;
static int frontend_i2c = -1;
static int frontend_tuner_addr = -1;
static int frontend_demod_addr = -1;
static int frontend_power=-1;
static struct i2c_adapter *frontend_i2c_handle=NULL;

static struct mutex dib_lock;

//static StreamType streamType;
//extern DefaultDemodulator demod;
//static Demodulator *pdemod = &demod;

static struct dvb_frontend_ops dib_ops;

static int dib_power_on(struct aml_fe_dev *dev)
{
	pr_dbg("dib_power_on \n");

	amlogic_gpio_request(dev->tuner_power_gpio, DIBCOM_POWER_MODULE_NAME);
	amlogic_gpio_direction_output(dev->tuner_power_gpio, 1, DIBCOM_POWER_MODULE_NAME);
	msleep(50);

	amlogic_gpio_request(dev->reset_gpio, DIBCOM_POWER_MODULE_NAME);
	amlogic_gpio_direction_output(dev->reset_gpio, 1, DIBCOM_POWER_MODULE_NAME);
	msleep(50);
	amlogic_gpio_direction_output(dev->reset_gpio, 0, DIBCOM_POWER_MODULE_NAME);
	msleep(50);
	amlogic_gpio_direction_output(dev->reset_gpio, 1, DIBCOM_POWER_MODULE_NAME);
	msleep(50);

	return 0;
}

static int dib_power_down(struct aml_fe_dev *dev)
{
	pr_dbg("dibcom_sleep chenj\n");
	dibcom_deep_sleep();

	msleep(200);

	pr_dbg("dib_power_down \n");
	amlogic_gpio_request(dev->tuner_power_gpio, DIBCOM_POWER_MODULE_NAME);
	amlogic_gpio_direction_output(dev->tuner_power_gpio, 0, DIBCOM_POWER_MODULE_NAME);

	return 0;
}


static int dibfe_set_frontend(struct dvb_frontend *fe)
{
	struct aml_fe *afe = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	char bandwidth = 8;

	bandwidth=c->bandwidth_hz/1000000;
	if((bandwidth!=8)&&(bandwidth!=7)&&(bandwidth!=6))
		bandwidth=8;

    dib_tune((c->frequency/1000), bandwidth);
	afe->params = *c;
	return  0;
}

static int dibfe_get_frontend(struct dvb_frontend *fe)
{//these content will be writed into eeprom .
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct aml_fe *afe = fe->demodulator_priv;
	
	*c = afe->params;
	return 0;
}

static int dibfe_read_status(struct dvb_frontend *fe, fe_status_t * status)
{
	unsigned char locked = 0;

	locked = dib_lockstatus();

	if(locked==1) {
		*status = FE_HAS_LOCK|FE_HAS_SIGNAL|FE_HAS_CARRIER|FE_HAS_VITERBI|FE_HAS_SYNC;
	} else {
		*status = FE_TIMEDOUT;
	}

	return  0;
}

static int dibfe_read_ber(struct dvb_frontend *fe, u32 * ber)
{
   *ber = dibcom_read_ber();

	return 0;
}

static int dibfe_read_signal_strength(struct dvb_frontend *fe, u16 *strength)
{
	*strength = dibcom_read_signal_strength();

	return 0;
}

static int dibfe_read_snr(struct dvb_frontend *fe, u16 * snr)
{
	*snr = dibcom_read_snr();

	return 0;
}

static int dibfe_read_ucblocks(struct dvb_frontend *fe, u32 * ucblocks)
{

	ucblocks = NULL;

	return 0;
}

int dib_get_fe_config(struct  dib_fe_config *cfg)
{
	cfg->i2c_id = frontend_i2c;
	cfg->demod_addr = frontend_demod_addr;
	cfg->tuner_addr = frontend_tuner_addr;
	cfg->reset_pin = frontenddib_reset;
	cfg->i2c_adapter =frontend_i2c_handle;
	return 1;
}

//EXPORT_SYMBOL(dib_get_fe_config);

int demod_resume()
{
	return 0;
}
EXPORT_SYMBOL(demod_resume);

int demod_suspend()
{
	//return dib_power_down();
	return 0;
}
EXPORT_SYMBOL(demod_suspend);



static int dibcom_fe_get_ops(struct aml_fe_dev *dev, int mode, void *ops)
{
	struct dvb_frontend_ops *fe_ops = (struct dvb_frontend_ops*)ops;

	fe_ops->info.frequency_min = 51000000;
	fe_ops->info.frequency_max = 858000000;
	fe_ops->info.frequency_stepsize = 166667;
	fe_ops->info.frequency_tolerance = 0;
	fe_ops->info.caps = FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
		FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
		FE_CAN_QPSK | FE_CAN_QAM_16 |
		FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
		FE_CAN_TRANSMISSION_MODE_AUTO |
		FE_CAN_GUARD_INTERVAL_AUTO |
		FE_CAN_HIERARCHY_AUTO |
		FE_CAN_RECOVER |
		FE_CAN_MUTE_TS;

	fe_ops->set_frontend = dibfe_set_frontend;
	fe_ops->get_frontend = dibfe_get_frontend;	
	fe_ops->read_status = dibfe_read_status;
	fe_ops->read_ber = dibfe_read_ber;
	fe_ops->read_signal_strength = dibfe_read_signal_strength;
	fe_ops->read_snr = dibfe_read_snr;
	fe_ops->read_ucblocks = dibfe_read_ucblocks;

	fe_ops->asyncinfo.set_frontend_asyncenable = 1;

	
	frontend_i2c = dev->i2c_adap_id;
	frontend_tuner_addr = 0xc0;
	frontend_demod_addr = dev->i2c_addr;

	frontend_power = dev->tuner_power_gpio;
	frontenddib_reset = dev->reset_gpio;
	frontend_i2c_handle = dev->i2c_adap;

	return 0;
}

static int dibcom_fe_enter_mode(struct aml_fe *fe, int mode)
{
	struct aml_fe_dev *dev = fe->dtv_demod;

	pr_dbg("dib_init_FOR_AMLOGIC \n");
	dib_power_on(dev);
	dib_init();

	return 0;
}

static int dibcom_fe_resume(struct aml_fe_dev *dev)
{
	dib_power_on(dev);
	return dib_init();
}

static int dibcom_fe_suspend(struct aml_fe_dev *dev)
{
	return dib_power_down(dev);
}


static struct aml_fe_drv dibcom_dtv_demod_drv = {
.id         = AM_DTV_DEMOD_DIB8096,
.name       = "dibcom8096",
.capability = AM_FE_ISDBT|AM_FE_OFDM,
.get_ops    = dibcom_fe_get_ops,
.enter_mode = dibcom_fe_enter_mode,
.suspend    = dibcom_fe_suspend,
.resume     = dibcom_fe_resume
};

static int __init dibfrontend_init(void)
{
	pr_dbg("register dibcom demod driver\n");
	mutex_init(&dib_lock);
	return aml_register_fe_drv(AM_DEV_DTV_DEMOD, &dibcom_dtv_demod_drv);
}


static void __exit dibfrontend_exit(void)
{
	pr_dbg("unregister dibcom demod driver\n");
	mutex_destroy(&dib_lock);
	aml_unregister_fe_drv(AM_DEV_DTV_DEMOD, &dibcom_dtv_demod_drv);
}


fs_initcall(dibfrontend_init);
module_exit(dibfrontend_exit);


MODULE_DESCRIPTION("DIBCOM ISDB-T demodulator driver");
MODULE_AUTHOR("HSW");
MODULE_LICENSE("GPL");


