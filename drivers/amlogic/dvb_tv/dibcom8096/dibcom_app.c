#include "config.h"
#include "host.h"
#include "frontend_tune.h"
#include "sip.h"
#ifdef DIB7090P_USED
#include "dib7000p.h"
#endif
#ifdef DIB8096P_USED
#include "dib8000p.h"
#endif
#include "dibx090p.h"
#include "dib0090.h"
#include "monitor.h"

#include "dibcom_app.h"
#include "DiBcom_i2c_app.h"

struct dibFrontend fe;
struct dibChannel ch;

#define printf printk

//added for amlogic multithreads conflict
#define STATE_TUNE_WORKING 0
#define STATE_TUNE_IDLE 1
static int g_tune_state = STATE_TUNE_IDLE;
int g_chipid = CHIPID_ERROR;


#ifdef DIB7090P_USED
static int dib7090_update_lna(struct dibFrontend *fe, uint16_t agc_global);

static   struct dibx090p_config dib7090_layout_config = {
    1,
    dib7090_update_lna, // update_lna

    DIBX090P_GPIO_DEFAULT_DIRECTIONS,
    DIBX090P_GPIO_DEFAULT_VALUES,
    DIBX090P_GPIO_DEFAULT_PWM_POS,

    0,     // dib0090_freq_offset_khz_uhf
    0,     // dib0090_freq_offset_khz_vhf

    12000, // clock_khz
    0,      // diversity delay
    NULL,   // dib0090_wbd_table
    0,
    4,
    0,
    0,
    0,
    0,
    0x2d98, // dib8k_drives
    0,
    1, //clkouttobamse
    0,
    0,
    0,
};

static int dib7090_update_lna(struct dibFrontend *fe, uint16_t agc_global)
{
    dbgpl(NULL, "update lna, agc_global=%d", agc_global);
    if ((dib7090_layout_config.use_high_level_adjustment & 1) && (agc_global < 16000))
        demod_set_gpio(fe, 8, 0, 0);

    return 0;
}

int dib7090_init()
{
    struct dibDataBusHost *i2c;

#ifdef DIBCOM_DEBUG_INFO
    printf("dibcom star\n");
#endif
	if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

    i2c = host_i2c_interface_attach(NULL);
    if (i2c == NULL)
    {
    	g_tune_state = STATE_TUNE_IDLE;
        return 1;
	}

	frontend_init(&fe); /* initializing the frontend-structure */
	frontend_set_id(&fe, 0); /* assign an absolute ID to the frontend */
	frontend_set_description(&fe, "DVB-T/H Single/Master");

    if ( dib7090p_sip_register(&fe, i2c, 0x10, &dib7090_layout_config) == NULL)
    {
    	g_tune_state = STATE_TUNE_IDLE;
        return DIB_RETURN_ERROR;
	}

    if(DIB_RETURN_SUCCESS != frontend_reset(&fe))
	{
		g_tune_state = STATE_TUNE_IDLE;
        return DIB_RETURN_ERROR;
	}

	g_tune_state = STATE_TUNE_IDLE;

    return DIB_RETURN_SUCCESS;
}

void dib7090_tune(unsigned int frequency, unsigned char bandwidth)
{
#if 1
    struct dibChannel ch;
#ifdef DIBCOM_DEBUG_INFO
    printf("frequency = %d, bandwidth= %d\n", frequency, bandwidth);
#endif

	if(g_tune_state == STATE_TUNE_WORKING)
		return ;

	g_tune_state = STATE_TUNE_WORKING;

    dib7000p_set_gpio(&fe, 8, 0, 1);
#ifdef DIBCOM_DEBUG_INFO
    DibDbgPrint("GPIO 8 ON \n");
#endif
    //tune ....
    INIT_CHANNEL(&ch, STANDARD_DVBT);
    ch.RF_kHz = frequency;
    switch(bandwidth)
    {
        case 6:
            ch.bandwidth_kHz = 6000;
            break;
        case 7:
            ch.bandwidth_kHz = 7000;
            break;
        case 8:
        default:
            ch.bandwidth_kHz = 8000;
            break;
    }
#ifdef DIBCOM_DEBUG_INFO
    printf("Dibcom connect: freq=%dkHz,Bandwidth=%dkHz\n", ch.RF_kHz, ch.bandwidth_kHz);
    printf("-I- dib_tune begin\n");
#endif
    tune_diversity(&fe, 1, &ch);
{
	struct dibDVBSignalStatus status;
	//DibMSleep(20);
	demod_get_signal_status(&fe, &status);
	if(status.tps_data_lock == 1)
	{
		DibMSleep(300);
    }
}
//    set_streaming_dib0700(&fe, &ch, 0, 1);
#ifdef DIBCOM_DEBUG_INFO
	printf("-I- dib_tune end\n");
#endif

#if 0 //def DIBCOM_DEBUG_INFO
{
    struct dibDemodMonitor mon;
	DibZeroMemory(&mon, sizeof(mon));

	printf("-I- dib_tune end\n");

    do{
        DibMSleep(800);
        demod_get_monitoring(&fe, &mon);
        dib7000_print_monitor(&mon, NULL, 0 ,1);
    }while(0);
}
#endif

	g_tune_state = STATE_TUNE_IDLE;
#endif
}

unsigned char dib7090_lockstatus(void)
{
    struct dibDemodMonitor mon[1];

    if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

    demod_get_monitoring(&fe, &mon[0]);

#ifdef DIBCOM_DEBUG_INFO
    if(mon[0].locks.fec_mpeg == 1)
    {
        printf("MASTER DIBCOM LOCK OK ......\n");
    }
    else
    {
        printf("MASTER DIBCOM LOCK FAIL ....\n");
    }
#endif

	g_tune_state = STATE_TUNE_IDLE;

    return mon[0].locks.fec_mpeg;
}
#endif

#ifdef DIB8096P_USED
static int tfe8096p_update_lna(struct dibFrontend *fe, uint16_t agc_global);

static const struct dibx090p_config tfe8096p_layout_config = {
    1,
    tfe8096p_update_lna, // update_lna
    DIBX090P_GPIO_DEFAULT_DIRECTIONS,
    DIBX090P_GPIO_DEFAULT_VALUES,
    DIBX090P_GPIO_DEFAULT_PWM_POS,
    -143, //freq_offset_khz_uhf;
    -143, //freq_offset_khz_vhf;
    12000,  // clock_khz
    48,   //diversity_delay
    NULL,   // dib0090_wbd_table
    0, 0, 0, 0,
    0, // use high level adjustment
};

static int tfe8096p_update_lna(struct dibFrontend *fe, uint16_t agc_global)
{
    dbgpl(NULL, "update lna, agc_global=%d", agc_global);
    if ((tfe8096p_layout_config.use_high_level_adjustment & 1) && (agc_global < 16000))
        demod_set_gpio(fe, 5, 0, 0);

    return 0;
}

int dib8096_init(void)
{
	struct dibDataBusHost *i2c;

	//test_dibcom();

#ifdef DIBCOM_DEBUG_INFO
	printf("dibcom star\n");
#endif

	if(g_tune_state == STATE_TUNE_WORKING)
		return ;

	g_tune_state = STATE_TUNE_WORKING;

	i2c = host_i2c_interface_attach(NULL);

	if (i2c == NULL)
	{
    	g_tune_state = STATE_TUNE_IDLE;
        return 1;
	}

	frontend_init(&fe); /* initializing the frontend-structure */
	frontend_set_id(&fe, 0); /* assign an absolute ID to the frontend */
	frontend_set_description(&fe, "ISDB-T Single/Master");

	if (dib8090p_sip_register(&fe, i2c, 0x10, &tfe8096p_layout_config) == NULL)
	{
    	g_tune_state = STATE_TUNE_IDLE;
        return DIB_RETURN_ERROR;
	}

	if(DIB_RETURN_SUCCESS != frontend_reset(&fe))
	{
    	g_tune_state = STATE_TUNE_IDLE;
        return DIB_RETURN_ERROR;
	}

	g_tune_state = STATE_TUNE_IDLE;

	return DIB_RETURN_SUCCESS;
}


void dib8096_tune(unsigned int frequency, unsigned char bandwidth)
{
    struct dibChannel ch;
    struct dibDemodMonitor m[1];
#ifdef DIBCOM_DEBUG_INFO
    printf("frequency = %d, bandwidth= %d\n", frequency, bandwidth);
#endif

	if(g_tune_state == STATE_TUNE_WORKING)
		return ;

	g_tune_state = STATE_TUNE_WORKING;

    DibZeroMemory(&m, sizeof(m));

    demod_set_gpio(&fe, 8, 0, 1);
    DibDbgPrint("GPIO 8 ON \n");
    //tune ....
    INIT_CHANNEL(&ch, STANDARD_ISDBT);
    ch.RF_kHz = frequency;
#ifdef DIBCOM_DEBUG_INFO
	printf("Dibcom connect: freq=%dkHz,Bandwidth=%dMHz\n", ch.RF_kHz, ch.bandwidth_kHz);
 	printf("-I- dib_tune begin\n");
#endif

    tune_diversity(&fe, 1, &ch);
#if 0
	demod_get_monitoring(&fe, &m);
#endif	
	//DibMSleep(50);
#if 0
	if(m[0].locks.tmcc_sync == 1)
	{
    	//DibMSleep(300);
    }
#endif	
//    set_streaming_dib0700(&fe, &ch, 0, 1);

#if 0 //def DIBCOM_DEBUG_INFO
	int k=0;

    printf("-I- dib_tune end\n");
    do{
       // DibMSleep(300);
        demod_get_monitoring(&fe, &m);
        //dib7000_print_monitor(&mon, NULL, 0 ,1);
        if(1)
        {
            if (!m[0].do_not_display_chandec) {
                printk("demod locks:  agc    coff        lmod4  equal      tmcc_sync   dvsy\n");
                printk("              | corm | coff_cpil | pha3 | tmcc_dec | tmcc_data |  vit(a,b,c)  fec_mpeg(a,b,c) \n");
                for (k = 0; k < 1; k++)
                    printk("demod %2d:     %d %d    %d %d         %d %d    %d %d        %d %d         %d  %d%d%d         %d%d%d\n",k,
                            m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,m[k].locks.tmcc_dec, m[k].locks.tmcc_sync, m[k].locks.tmcc_data, m[k].locks.dvsy, m[k].locks.vit, m[k].locks.vit_b,m[k].locks.vit_c, m[k].locks.fec_mpeg,m[k].locks.fec_mpeg_b,m[k].locks.fec_mpeg_c);
                printk("\n");
            }
        }
    }while(0);
#endif

	g_tune_state = STATE_TUNE_IDLE;
}

unsigned char dib8096_lockstatus(void)
{
    struct dibDemodMonitor m[1];
	int k=0;
	
	DibMSleep(300);
	
    if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

    demod_get_monitoring(&fe, &m[0]);
	if(0)
	{
		if (!m[0].do_not_display_chandec) {
			printk("demod locks:  agc	 coff		 lmod4	equal	   tmcc_sync   dvsy\n");
			printk("			  | corm | coff_cpil | pha3 | tmcc_dec | tmcc_data |  vit(a,b,c)  fec_mpeg(a,b,c) \n");
			for (k = 0; k < 1; k++)
				printk("demod %2d:	   %d %d	%d %d		  %d %d    %d %d		%d %d		  %d  %d%d%d		 %d%d%d\n",k,
						m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,m[k].locks.tmcc_dec, m[k].locks.tmcc_sync, m[k].locks.tmcc_data, m[k].locks.dvsy, m[k].locks.vit, m[k].locks.vit_b,m[k].locks.vit_c, m[k].locks.fec_mpeg,m[k].locks.fec_mpeg_b,m[k].locks.fec_mpeg_c);
			printk("\n");
		}
	}

#ifdef DIBCOM_DEBUG_INFO
    if(m[0].locks.fec_mpeg == 1)
    {
        printf("DIBCOM LOCK Lock Layer A OK ......\n");
	    if(m[0].locks.fec_mpeg_b == 1)
	    {
	        printf("DIBCOM LOCK Lock Layer B OK ......\n");
	    }
	    if(m[0].locks.fec_mpeg_c == 1)
	    {
	        printf("DIBCOM LOCK Lock Layer C OK ......\n");
	    }
    }
    else
    {
        printf("MASTER DIBCOM LOCK FAIL ....\n");
    }
#endif

	g_tune_state = STATE_TUNE_IDLE;

    return m[0].locks.fec_mpeg;
}

#endif

int dib_init()
{
	int dvb_demod_mach_id;
	g_chipid = getChipId();
	dvb_demod_mach_id = g_chipid;

	if(g_chipid == CHIPID_7090)
	{
		printf("dib_init CHIP 7090 .....");
		return dib7090_init();
	}
	else if(g_chipid == CHIPID_8096)
	{
		printf("dib_init CHIP 8096 .....");
		return dib8096_init();
	}
	else
		return DIB_RETURN_ERROR;
}

void dib_tune(unsigned int frequency, unsigned char bandwidth)
{
	if(g_chipid == CHIPID_7090)
		return dib7090_tune(frequency, bandwidth);
	else if(g_chipid == CHIPID_8096)
		return dib8096_tune(frequency, bandwidth);
	else
		return DIB_RETURN_ERROR;
}

unsigned char dib_lockstatus(void)
{
	if(g_chipid == CHIPID_7090)
		return dib7090_lockstatus();
	else if(g_chipid == CHIPID_8096)
		return dib8096_lockstatus();
	else
		return DIB_RETURN_ERROR;
}

void dib_signal_strenth_quality(unsigned char *signal_quality, unsigned char *signal_strength)
{
#if 0
	return ;

    int i;
    struct dibDVBSignalStatus status;
 //   static struct dibDemodMonitor demodmon;

 	if(g_tune_state == STATE_TUNE_WORKING)
		return ;

	g_tune_state = STATE_TUNE_WORKING;

    if (fe.demod->demod_info->ops.get_signal_status(fe.demod, &status) != 0)
    {
		DibDbgPrint("-E-  Get signal status failed\n");
    }
    //printf("status.bit_error_rate = %d, status.signal_strength = %d\n", status.bit_error_rate, status.signal_strength);

    *signal_quality = (unsigned char)(((float)(2097151 - status.bit_error_rate)/2097151.0)*255) ;
    //*signal_strength = (unsigned char)(153 + (UINT8)((255- 153)* (((float)status.signal_strength)/255.0)));
    *signal_strength = (unsigned char)(255* (((float)status.signal_strength)/255.0));

    if(*signal_quality < 1)
        *signal_quality = 1;
    else if (*signal_quality >255)
        *signal_quality =255;

    if(*signal_strength < 1)
        *signal_strength = 1;
    else if (*signal_strength >255)
        *signal_strength =255;

    //printf("signal_quality = %d signal_strength = %d\n", *signal_quality, *signal_strength);

	g_tune_state = STATE_TUNE_IDLE;

#else

   int i;
   struct dibDVBSignalStatus status;
//	 static struct dibDemodMonitor demodmon;

   if(g_tune_state == STATE_TUNE_WORKING)
	   return ;

   g_tune_state = STATE_TUNE_WORKING;

   if (fe.demod->demod_info->ops.get_signal_status(fe.demod, &status) != 0)
   {
	   DibDbgPrint("-E-  Get signal status failed\n");
   }
   //printf("status.bit_error_rate = %d, status.signal_strength = %d\n", status.bit_error_rate, status.signal_strength);

//   *signal_quality = status.V_agc_1;
   *signal_strength = status.signal_strength;

   //printf("signal_quality = %d signal_strength = %d\n", *signal_quality, *signal_strength);

   g_tune_state = STATE_TUNE_IDLE;

#endif

}

void dibcom_release(void)
{
	frontend_unregister_components(&fe);
}

void dibcom_sleep(void)
{
	frontend_sleep(&fe);
}

void dibcom_deep_sleep(void)
{
	frontend_deep_sleep(&fe);
}



unsigned int dibcom_read_ber(void)
{
   struct dibDVBSignalStatus status;

   if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

   if (demod_get_signal_status(&fe, &status) != 0)
   {
	   DibDbgPrint("-E-  Get signal status failed\n");
   }


   g_tune_state = STATE_TUNE_IDLE;

   return status.bit_error_rate;
}

/*
unsigned int dibcom_read_signal_quality(struct dibcom_signal_quality* params)
{
	struct dibDVBSignalStatus status;

	if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

	if (demod_get_signal_status(&fe, &status) != 0)
	{
		DibDbgPrint("-E-  Get signal status failed\n");
	}

	g_tune_state = STATE_TUNE_IDLE;

	params->equal_noise_exp = status.equal_noise_exp;
	params->equal_noise_mant = status.equal_noise_mant;
	params->equal_signal_exp = status.equal_signal_exp;
	params->equal_signal_mant = status.equal_signal_mant;

	return 0;
}
*/
unsigned int dibcom_read_signal_strength(void)
{
	struct dibDVBSignalStatus status;

	if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

	if (demod_get_signal_status(&fe, &status) != 0)
	{
		DibDbgPrint("-E-  Get signal status failed\n");
	}

	g_tune_state = STATE_TUNE_IDLE;

	return status.signal_strength;
}

unsigned int dibcom_read_snr(void)
{
	struct dibDemodMonitor mon[1];

	if(g_tune_state == STATE_TUNE_WORKING)
		return 0;

	g_tune_state = STATE_TUNE_WORKING;

//	demod_get_monitoring(&fe, &mon[0]);

//	return mon[0].CoN;

	g_tune_state = STATE_TUNE_IDLE;

	return 0;
}

/********************************************************************************************/
//
//
//    OVER
//
//
/********************************************************************************************/

