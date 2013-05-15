/*
 *
 * arch/arm/mach-meson/meson.c
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Platform machine definition.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/aml_eth.h>
#include <linux/device.h>
#include <linux/spi/flash.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/memory.h>
#include <mach/clock.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <mach/lm.h>
#include <asm/memory.h>
#include <asm/mach/map.h>
#include <mach/nand.h>
#include <linux/i2c.h>
#include <linux/i2c-aml.h>
#include <mach/power_gate.h>
#include <linux/aml_bl.h>
#include <linux/syscalls.h>
#include <linux/reboot.h>
#include <mach/usbclock.h>

#ifdef CONFIG_AM_UART_WITH_S_CORE 
#include <linux/uart-aml.h>
#endif
#include <mach/card_io.h>
#include <mach/pinmux.h>
#include <mach/gpio.h>
#include <linux/delay.h>
#include <mach/clk_set.h>
#include "board-m3-reff16.h"

#ifdef CONFIG_ANDROID_PMEM
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/android_pmem.h>
#endif

#ifdef CONFIG_AMLOGIC_PM
#include <linux/power_supply.h>
#include <linux/aml_power.h>
#endif

#ifdef CONFIG_SUSPEND
#include <mach/pm.h>
#endif

#ifdef CONFIG_EFUSE
#include <linux/efuse.h>
#endif

#ifdef CONFIG_AML_HDMI_TX
#include <linux/hdmi/hdmi_config.h>
#endif

#ifdef CONFIG_SUSPEND
static int suspend_state=0;
#endif

/* GPIO Defines */
// ETHERNET
#define GPIO_ETH_RESET  GPIO_D(7)
// BUTTONS
#define GPIO_KEY_POWER  ( GPIOAO_bank_bit0_11(3)  << 16 ) | GPIOAO_bit_bit0_11(3)
// SOUND
#define GPIO_SND_MUTE  GPIO_X(29)
// POWERSUPPLIES
#define GPIO_PWR_WIFI  GPIO_C(5)
#define GPIO_PWR_VCCIO  GPIO_AO(2)
#define GPIO_PWR_VCCx2  GPIO_AO(6)
#define GPIO_PWR_HDMI   GPIO_D(6)

#if defined(CONFIG_JPEGLOGO)
static struct resource jpeglogo_resources[] = {
    [0] = {
        .start = CONFIG_JPEGLOGO_ADDR,
        .end   = CONFIG_JPEGLOGO_ADDR + CONFIG_JPEGLOGO_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device jpeglogo_device = {
    .name = "jpeglogo-dev",
    .id   = 0,
    .num_resources = ARRAY_SIZE(jpeglogo_resources),
    .resource      = jpeglogo_resources,
};
#endif

#if defined(CONFIG_KEYPADS_AM)||defined(CONFIG_KEYPADS_AM_MODULE)
static struct resource intput_resources[] = {
    {
        .start = 0x0,
        .end = 0x0,
        .name="8726M3",
        .flags = IORESOURCE_IO,
    },
};

static struct platform_device input_device = {
    .name = "m1-kp",
    .id = 0,
    .num_resources = ARRAY_SIZE(intput_resources),
    .resource = intput_resources,
    
};
#endif

#ifdef CONFIG_SARADC_AM
#include <linux/saradc.h>
static struct platform_device saradc_device = {
    .name = "saradc",
    .id = 0,
    .dev = {
        .platform_data = NULL,
    },
};
#endif

#if defined(CONFIG_ADC_KEYPADS_AM)||defined(CONFIG_ADC_KEYPADS_AM_MODULE)
#include <linux/input.h>
#include <linux/adc_keypad.h>

static struct adc_key adc_kp_key[] = {
    {KEY_MENU,          "menu", CHAN_4, 0, 60},
    {KEY_VOLUMEDOWN,    "vol-", CHAN_4, 140, 60},
    {KEY_VOLUMEUP,      "vol+", CHAN_4, 266, 60},
    {KEY_BACK,          "exit", CHAN_4, 386, 60},
    {KEY_HOME,          "home", CHAN_4, 510, 60},
    {KEY_ENTER,          "enter", CHAN_4, 639, 60},
};

static struct adc_kp_platform_data adc_kp_pdata = {
    .key = &adc_kp_key[0],
    .key_num = ARRAY_SIZE(adc_kp_key),
};

static struct platform_device adc_kp_device = {
    .name = "m1-adckp",
    .id = 0,
    .num_resources = 0,
    .resource = NULL,
    .dev = {
    .platform_data = &adc_kp_pdata,
    }
};
#endif

#if defined(CONFIG_KEY_INPUT_CUSTOM_AM) || defined(CONFIG_KEY_INPUT_CUSTOM_AM_MODULE)
#include <linux/input.h>
#include <linux/input/key_input.h>

int _key_code_list[] = {KEY_POWER};

static inline int key_input_init_func(void)
{
  // Power Button, GPIO AO3, ACTIVE LOW
  gpio_direction_input(GPIO_KEY_POWER);
  return 0;
}
static inline int key_scan(int *key_state_list)
{
    int ret = 0;
	 // GPIOAO_3
	 #ifdef CONFIG_SUSPEND
	 if(suspend_state)
	 	{
	 	// forse power key down
	 	suspend_state--;
	 	key_state_list[0] = 1;
	 	}
	 else
	 #endif
    key_state_list[0] = gpio_get_value(GPIO_KEY_POWER) ? 0 : 1 ;
    return ret;
}

static  struct key_input_platform_data  key_input_pdata = {
    .scan_period = 20,
    .fuzz_time = 60,
    .key_code_list = &_key_code_list[0],
    .key_num = ARRAY_SIZE(_key_code_list),
.scan_func = key_scan,
    .init_func = key_input_init_func,
    .config =  2, 	// 0: interrupt;    	2: polling;
};

static struct platform_device input_device_key = {
    .name = "m1-keyinput",
    .id = 0,
    .num_resources = 0,
    .resource = NULL,
    .dev = {
        .platform_data = &key_input_pdata,
    }
};
#endif

#if defined(CONFIG_AM_IR_RECEIVER)
#include <linux/input/irreceiver.h>

static int ir_init()
{
    unsigned int control_value;
    
    //mask--mux gpioao_7 to remote
    SET_AOBUS_REG_MASK(AO_RTI_PIN_MUX_REG,1<<0);
    
    //max frame time is 80ms, base rate is 20us
    control_value = 3<<28|(0x9c40 << 12)|0x13;
    WRITE_AOBUS_REG(AO_IR_DEC_REG0, control_value);
     
    /*[3-2]rising or falling edge detected
      [8-7]Measure mode
    */
    control_value = 0x8574;
    WRITE_AOBUS_REG(AO_IR_DEC_REG1, control_value);
    
    return 0;
}

static int pwm_init()
{
    CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_7, (1<<16));
	CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_0, (1<<29));
    SET_CBUS_REG_MASK(PERIPHS_PIN_MUX_3, (1<<26));
}

static struct irreceiver_platform_data irreceiver_data = {
    .pwm_no = PWM_A,
    .freq = 38000, //38k
    .init_pwm_pinmux = pwm_init,
    .init_ir_pinmux = ir_init,
};

static struct platform_device irreceiver_device = {
    .name = "irreceiver",
    .id = 0,
    .num_resources = 0,
    .resource = NULL,
    .dev = {
        .platform_data = &irreceiver_data,
    }
};
#endif

#if defined(CONFIG_FB_AM)
static struct resource fb_device_resources[] = {
    [0] = {
        .start = OSD1_ADDR_START,
        .end   = OSD1_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
#if defined(CONFIG_FB_OSD2_ENABLE)
    [1] = {
        .start = OSD2_ADDR_START,
        .end   = OSD2_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
#endif
};

static struct platform_device fb_device = {
    .name       = "mesonfb",
    .id         = 0,
    .num_resources = ARRAY_SIZE(fb_device_resources),
    .resource      = fb_device_resources,
};
#endif

#ifdef CONFIG_USB_DWC_OTG_HCD
#ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE
static set_vbus_valid_ext_fun(unsigned int id,char val)
{
	unsigned int  reg = (PREI_USB_PHY_A_REG1 + id);
	if(val == 1)
		SET_CBUS_REG_MASK(reg,1<<0);
	else
		CLEAR_CBUS_REG_MASK(reg,1<<0);
}
#endif

/* usb wifi power 1:power on  0:power off */
void extern_usb_wifi_power(int is_power_on)
{
	printk(KERN_INFO "usb_wifi_power %s\n", is_power_on ? "On" : "Off");
	/* USB +3v3 Power Enable internal port, GPIO C5, ACTIVE LOW */ 
       if(is_power_on) {
		gpio_direction_output(GPIO_PWR_WIFI, 1);
	} else {
		gpio_direction_output(GPIO_PWR_WIFI, 0);
	}

}

EXPORT_SYMBOL(extern_usb_wifi_power);


static void set_usb_a_vbus_power(char is_power_on)
{
}

static void set_usb_b_vbus_power(char is_power_on)
{
}

static struct lm_device usb_ld_a = {
    .type = LM_DEVICE_TYPE_USB,
    .id = 0,
    .irq = INT_USB_A,
    .resource.start = IO_USB_A_BASE,
    .resource.end = -1,
    .dma_mask_room = DMA_BIT_MASK(32),
    .port_type = USB_PORT_TYPE_HOST,
    .port_speed = USB_PORT_SPEED_DEFAULT,
    .dma_config = USB_DMA_BURST_INCR16,
    .set_vbus_power = set_usb_a_vbus_power,
    #ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE	
	.set_vbus_valid_ext = set_vbus_valid_ext_fun,
    #endif
};

static struct lm_device usb_ld_b = {
    .type = LM_DEVICE_TYPE_USB,
    .id = 1,
    .irq = INT_USB_B,
    .resource.start = IO_USB_B_BASE,
    .resource.end = -1,
    .dma_mask_room = DMA_BIT_MASK(32),
    .port_type = USB_PORT_TYPE_HOST,
    .port_speed = USB_PORT_SPEED_DEFAULT,
    .dma_config = USB_DMA_BURST_INCR16, //   USB_DMA_DISABLE,
    .set_vbus_power = set_usb_b_vbus_power,
    #ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE	
	.set_vbus_valid_ext = set_vbus_valid_ext_fun,
    #endif	
};
#endif

#if defined(CONFIG_AM_STREAMING)
static struct resource codec_resources[] = {
    [0] = {
        .start =  CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = STREAMBUF_ADDR_START,
	 .end = STREAMBUF_ADDR_END,
	 .flags = IORESOURCE_MEM,
    },
};

static struct platform_device codec_device = {
    .name       = "amstream",
    .id         = 0,
    .num_resources = ARRAY_SIZE(codec_resources),
    .resource      = codec_resources,
};
#endif

#if defined(CONFIG_AM_VIDEO)
static struct resource deinterlace_resources[] = {
    [0] = {
        .start =  DI_ADDR_START,
        .end   = DI_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device deinterlace_device = {
    .name       = "deinterlace",
    .id         = 0,
    .num_resources = ARRAY_SIZE(deinterlace_resources),
    .resource      = deinterlace_resources,
};
#endif

#if defined(CONFIG_AM_NAND)
static struct mtd_partition multi_partition_info[] = 
{ /* Hidden nandboot partition   
     { 0x00 to 0x400000
       	.name = "nandboot",
	.offset = BOOTLOADER_OFFSET,
	.size = BOOTLOADER_SIZE,
     },
*/ 
  {
    .name   = "boot_env",
    .offset = (12)*1024*1024,
    .size   = 4*1024*1024,
  },
  {
    .name   = "aml_logo",
    .offset = (16)*1024*1024,
    .size   = 16*1024*1024,
  },
  {
    .name   = "recovery",
    .offset = (16+16)*1024*1024,
    .size   = 16*1024*1024,
  },
  {
    .name   = "boot",
    .offset = (16+16+16)*1024*1024,
    .size   = 20*1024*1024,
  },
  {
    .name   = "system",
    .offset = (16+16+16+20)*1024*1024,
    .size   = 512*1024*1024,
  },
  {
    .name   = "cache",
    .offset = (16+16+16+20+512)*1024*1024,
    .size   = 192*1024*1024,
  },
  {
    .name = "userdata",
    .offset=MTDPART_OFS_APPEND,
    .size=MTDPART_SIZ_FULL,
  },
};

static void nand_set_parts(uint64_t size, struct platform_nand_chip *chip)
{
    printk("set nand parts for chip %lldMB\n", (size/(1024*1024)));

    chip->partitions = multi_partition_info;
    chip->nr_partitions = ARRAY_SIZE(multi_partition_info);
}

static struct aml_nand_platform aml_nand_mid_platform[] = {
#ifdef CONFIG_AML_NAND_ENV
{
		.name = NAND_BOOT_NAME,
		.chip_enable_pad = AML_NAND_CE0,
		.ready_busy_pad = AML_NAND_CE0,
		.platform_nand_data = {
			.chip =  {
				.nr_chips = 1,
				.options = (NAND_TIMING_MODE5 | NAND_ECC_BCH60_1K_MODE),
			},
    	},
			.T_REA = 20,
			.T_RHOH = 15,
	},
#endif
{
		.name = NAND_MULTI_NAME,
		.chip_enable_pad = (AML_NAND_CE0 | (AML_NAND_CE1 << 4) | (AML_NAND_CE2 << 8) | (AML_NAND_CE3 << 12)),
		.ready_busy_pad = (AML_NAND_CE0 | (AML_NAND_CE0 << 4) | (AML_NAND_CE1 << 8) | (AML_NAND_CE1 << 12)),
		.platform_nand_data = {
			.chip =  {
				.nr_chips = 4,
				.nr_partitions = ARRAY_SIZE(multi_partition_info),
				.partitions = multi_partition_info,
				.set_parts = nand_set_parts,
				.options = (NAND_TIMING_MODE5 | NAND_ECC_BCH60_1K_MODE | NAND_TWO_PLANE_MODE),
			},
    	},
			.T_REA = 20,
			.T_RHOH = 15,
	}
};

struct aml_nand_device aml_nand_mid_device = {
	.aml_nand_platform = aml_nand_mid_platform,
	.dev_num = ARRAY_SIZE(aml_nand_mid_platform),
};

static struct resource aml_nand_resources[] = {
    {
        .start = 0xc1108600,
        .end = 0xc1108624,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device aml_nand_device = {
    .name = "aml_m3_nand",
    .id = 0,
    .num_resources = ARRAY_SIZE(aml_nand_resources),
    .resource = aml_nand_resources,
    .dev = {
		.platform_data = &aml_nand_mid_device,
    },
};
#endif

#if defined(CONFIG_CARDREADER)
static struct resource amlogic_card_resource[] = {
    [0] = {
        .start = 0x1200230,   //physical address
        .end   = 0x120024c,
        .flags = 0x200,
    }
};

static struct aml_card_info  amlogic_card_info[] = {
    [0] = {
        .name = "sd_card",
        .work_mode = CARD_HW_MODE,
        .io_pad_type = SDIO_B_CARD_0_5,
        .card_ins_en_reg = CARD_GPIO_ENABLE,
        .card_ins_en_mask = PREG_IO_29_MASK,
        .card_ins_input_reg = CARD_GPIO_INPUT,
        .card_ins_input_mask = PREG_IO_29_MASK,
        .card_power_en_reg = CARD_GPIO_ENABLE,
        .card_power_en_mask = PREG_IO_31_MASK,
        .card_power_output_reg = CARD_GPIO_OUTPUT,
        .card_power_output_mask = PREG_IO_31_MASK,
        .card_power_en_lev = 0,
        .card_wp_en_reg = CARD_GPIO_ENABLE,
        .card_wp_en_mask = PREG_IO_30_MASK,
        .card_wp_input_reg = CARD_GPIO_INPUT,
        .card_wp_input_mask = PREG_IO_30_MASK,
        .card_extern_init = 0,
    },
};

static struct aml_card_platform amlogic_card_platform = {
    .card_num = ARRAY_SIZE(amlogic_card_info),
    .card_info = amlogic_card_info,
};

static struct platform_device amlogic_card_device = { 
    .name = "AMLOGIC_CARD", 
    .id    = -1,
    .num_resources = ARRAY_SIZE(amlogic_card_resource),
    .resource = amlogic_card_resource,
    .dev = {
        .platform_data = &amlogic_card_platform,
    },
};
#endif

#if defined(CONFIG_AML_AUDIO_DSP)
static struct resource audiodsp_resources[] = {
    [0] = {
        .start = AUDIODSP_ADDR_START,
        .end   = AUDIODSP_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device audiodsp_device = {
    .name       = "audiodsp",
    .id         = 0,
    .num_resources = ARRAY_SIZE(audiodsp_resources),
    .resource      = audiodsp_resources,
};
#endif

static struct resource aml_m3_audio_resource[] = {
    [0] =   {
        .start  =   0,
        .end        =   0,
        .flags  =   IORESOURCE_MEM,
    },
};

#if defined(CONFIG_SND_AML_M3)
static struct platform_device aml_audio = {
    .name               = "aml_m3_audio",
    .id                     = -1,
    .resource       =   aml_m3_audio_resource,
    .num_resources  =   ARRAY_SIZE(aml_m3_audio_resource),
};

int aml_m3_is_hp_pluged(void)
{
	return 0; //return 1: hp pluged, 0: hp unpluged.
}

void mute_spk(void* codec, int flag)
{
#ifdef _AML_M3_HW_DEBUG_
	printk("***Entered %s:%s\n", __FILE__,__func__);
#endif
	if ( flag ) {
	gpio_direction_output(GPIO_SND_MUTE,1);
	} else {
	gpio_direction_output(GPIO_SND_MUTE,0);
	}
}

#define APB_BASE 0x4000
void mute_headphone(void* codec, int flag)
{
	int reg_val;
#ifdef _AML_M3_HW_DEBUG_
	printk("***Entered %s:%s\n", __FILE__,__func__);
#endif
	reg_val = READ_APB_REG(APB_BASE+(0x18<<2));
    if(flag){
		reg_val |= 0xc0;
		WRITE_APB_REG((APB_BASE+(0x18<<2)), reg_val);			// mute headphone
	}else{
		reg_val &= ~0xc0;
		WRITE_APB_REG((APB_BASE+(0x18<<2)), reg_val);			// unmute headphone
	}
}
/* --------------------------------------------------------------------------*/
/**
 * * @brief  set_audio_codec_pinmux
 * *
 * * @return
 * */
/* --------------------------------------------------------------------------*/
static void __init set_audio_codec_pinmux(void)
{
    /* for gpiox_17~20 I2S_AMCLK I2S_AOCLK I2S_LRCLK I2S_OUT */
    clear_mio_mux(7, (1 << 18) | (1 << 19) | (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23));
    set_mio_mux(8, (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24));
}
#endif

#if defined(CONFIG_ANDROID_PMEM) || defined(CONFIG_CMEM)
static struct android_pmem_platform_data pmem_data =
{
    .name = "pmem",
    .start = PMEM_START,
    .size = PMEM_SIZE,
    .no_allocator = 1,
    .cached = 0,
};

static struct platform_device android_pmem_device =
{
    .name = "android_pmem",
    .id = 0,
    .dev = {
        .platform_data = &pmem_data,
    },
};
#endif

#if defined(CONFIG_AML_RTC)
static  struct platform_device aml_rtc_device = {
            .name            = "aml_rtc",
            .id               = -1,
    };
#endif

#if defined(CONFIG_SUSPEND)

typedef struct {
	char name[32];
	unsigned bank;
	unsigned bit;
	gpio_mode_t mode;
	unsigned value;
	unsigned enable;
} gpio_data_t;

typedef struct {
	char name[32];
	unsigned reg;
	unsigned bits;
	unsigned enable;
} pinmux_data_t;


#define MAX_PINMUX	1

pinmux_data_t pinmux_data[MAX_PINMUX] = {
	{"HDMI", 	0, (1<<2)|(1<<1)|(1<<0), 						1},
};

static unsigned pinmux_backup[6];

static void save_pinmux(void)
{
	int i;
	for (i=0;i<6;i++)
		pinmux_backup[i] = READ_CBUS_REG(PERIPHS_PIN_MUX_0+i);
	for (i=0;i<MAX_PINMUX;i++){
		if (pinmux_data[i].enable){
			printk("%s %x\n", pinmux_data[i].name, pinmux_data[i].bits);
			clear_mio_mux(pinmux_data[i].reg, pinmux_data[i].bits);
		}
	}
}

static void restore_pinmux(void)
{
	int i;
	for (i=0;i<6;i++)
		 WRITE_CBUS_REG(PERIPHS_PIN_MUX_0+i, pinmux_backup[i]);
}

static void set_vccx2(int power_on)
{
		if (power_on){
		printk(KERN_INFO "set_vcc power up\n");
		restore_pinmux();
		#ifdef CONFIG_AML_SUSPEND
		suspend_state=10;
		#endif
  		} else {
    		printk(KERN_INFO "set_vcc power down\n");
		save_pinmux();
    		}
}

extern void hdmi_wr_reg(unsigned long addr, unsigned long data);

static void set_gpio_suspend_resume(int power_on)
{
    if(power_on)
    	{
    	printk("set gpio resume.\n");
        hdmi_wr_reg(0x8005, 2); 
	udelay(50);
        hdmi_wr_reg(0x8005, 1); 
    	}
	else
	{
    	printk("set gpio suspend.\n");
	}
}

static struct meson_pm_config aml_pm_pdata = {
    .pctl_reg_base = (void __iomem *)IO_APB_BUS_BASE,
    .mmc_reg_base = (void __iomem *)APB_REG_ADDR(0x1000),
    .hiu_reg_base = (void __iomem *)CBUS_REG_ADDR(0x1000),
    .power_key = (1<<8),
    .ddr_clk = 0x00110820,
    .sleepcount = 128,
    .set_vccx2 = set_vccx2,
    .core_voltage_adjust = 7,  //5,8
    .set_exgpio_early_suspend = set_gpio_suspend_resume,
};

static struct platform_device aml_pm_device = {
    .name           = "pm-meson",
    .dev = {
        .platform_data  = &aml_pm_pdata,
    },
    .id             = -1,
};
#endif

#if defined(CONFIG_I2C_SW_AML)
#define MESON3_I2C_PREG_GPIOX_OE CBUS_REG_ADDR(PREG_PAD_GPIO4_EN_N)
#define MESON3_I2C_PREG_GPIOX_OUTLVL CBUS_REG_ADDR(PREG_PAD_GPIO4_O)
#define MESON3_I2C_PREG_GPIOX_INLVL CBUS_REG_ADDR(PREG_PAD_GPIO4_I)

static struct aml_sw_i2c_platform aml_sw_i2c_plat = {
    .sw_pins = {
    .scl_reg_out = MESON3_I2C_PREG_GPIOX_OUTLVL,
    .scl_reg_in = MESON3_I2C_PREG_GPIOX_INLVL,
    .scl_bit = 26,
    .scl_oe = MESON3_I2C_PREG_GPIOX_OE,
    .sda_reg_out = MESON3_I2C_PREG_GPIOX_OUTLVL,
    .sda_reg_in = MESON3_I2C_PREG_GPIOX_INLVL,
    .sda_bit = 25,
    .sda_oe = MESON3_I2C_PREG_GPIOX_OE,
    },
    .udelay = 2,
    .timeout = 100,
};

static struct platform_device aml_sw_i2c_device = {
    .name = "aml-sw-i2c",
    .id = 0,
    .dev = {
        .platform_data = &aml_sw_i2c_plat,
    },
};
#endif

#ifdef CONFIG_AMLOGIC_PM

static int is_ac_connected(void)
{
    return 1;
}

static void set_charge(int flags)
{
}

#ifdef CONFIG_SARADC_AM
extern int get_adc_sample(int chan);
#endif
static int get_bat_vol(void)
{
#ifdef CONFIG_SARADC_AM
    return get_adc_sample(5);
#else
        return 0;
#endif
}

static int get_charge_status(void)
{
    return 0;
}

static void power_off(void)
{
}

static void set_bat_off(void)
{
    //VCCx2 power down
    #if defined(CONFIG_SUSPEND)
    set_vccx2(0);
    #endif
}

static struct aml_power_pdata power_pdata = {
	.is_ac_online	= is_ac_connected,
	//.is_usb_online	= is_usb_connected,
	//.set_charge = set_charge,
	//.get_bat_vol = get_bat_vol,
	.get_charge_status = get_charge_status,
	.set_bat_off = set_bat_off,
	//.bat_value_table = bat_value_table,
	//.bat_charge_value_table = bat_charge_value_table,
	//.bat_level_table = bat_level_table,
	//.bat_table_len = 37,		
	.is_support_usb_charging = 0,
	//.supplied_to = supplicants,
	//.num_supplicants = ARRAY_SIZE(supplicants),
};

static struct platform_device power_dev = {
    .name       = "aml-power",
    .id     = -1,
    .dev = {
        .platform_data  = &power_pdata,
    },
};
#endif

#if defined(CONFIG_AM_UART_WITH_S_CORE)
static struct aml_uart_platform aml_uart_plat = {
    .uart_line[0]       =   UART_AO,
    .uart_line[1]       =   UART_A,
    .uart_line[2]       =   UART_B,
    .uart_line[3]       =   UART_C
};

static struct platform_device aml_uart_device = {
    .name         = "am_uart",  
    .id       = -1, 
    .num_resources    = 0,  
    .resource     = NULL,   
    .dev = {        
                .platform_data = &aml_uart_plat,
           },
};
#endif

#ifdef CONFIG_EFUSE
static bool efuse_data_verify(unsigned char *usid)
{  int len;
  
    len = strlen(usid);
    if((len > 8)&&(len<31) )
        return true;
		else
				return false;
}

static struct efuse_platform_data aml_efuse_plat = {
    .pos = 337,
    .count = 30,
    .data_verify = efuse_data_verify,
};

static struct platform_device aml_efuse_device = {
    .name	= "efuse",
    .id	= -1,
    .dev = {
                .platform_data = &aml_efuse_plat,
           },
};
#endif


#if  defined(CONFIG_AM_TV_OUTPUT)||defined(CONFIG_AM_TCON_OUTPUT)
static struct resource vout_device_resources[] = {
    [0] = {
        .start = 0,
        .end   = 0,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device vout_device = {
    .name       = "mesonvout",
    .id         = 0,
    .num_resources = ARRAY_SIZE(vout_device_resources),
    .resource      = vout_device_resources,
};
#endif
#if  defined(CONFIG_AM_TV_OUTPUT2)
static struct resource vout2_device_resources[] = {
    [0] = {
        .start = 0,
        .end   = 0,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device vout2_device = {
    .name       = "mesonvout2",
    .id         = 0,
    .num_resources = ARRAY_SIZE(vout2_device_resources),
    .resource      = vout2_device_resources,
};
#endif

 #ifdef CONFIG_POST_PROCESS_MANAGER
static struct resource ppmgr_resources[] = {
    [0] = {
        .start =  PPMGR_ADDR_START,
        .end   = PPMGR_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};
static struct platform_device ppmgr_device = {
    .name       = "ppmgr",
    .id         = 0,
    .num_resources = ARRAY_SIZE(ppmgr_resources),
    .resource      = ppmgr_resources,
};
#endif
#ifdef CONFIG_FREE_SCALE
static struct resource freescale_resources[] = {
    [0] = {
        .start = FREESCALE_ADDR_START,
        .end   = FREESCALE_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device freescale_device =
{
    .name           = "freescale",
    .id             = 0,
    .num_resources  = ARRAY_SIZE(freescale_resources),
    .resource       = freescale_resources,
};
#endif

#if defined(CONFIG_AM_DVB)
static struct resource amlogic_dvb_resource[]  = {
	[0] = {
		.start = INT_DEMUX,                   //demux 0 irq
		.end   = INT_DEMUX,
		.flags = IORESOURCE_IRQ,
		.name  = "demux0_irq"
	},
	[1] = {
		.start = INT_DEMUX_1,                    //demux 1 irq
		.end   = INT_DEMUX_1,
		.flags = IORESOURCE_IRQ,
		.name  = "demux1_irq"
	},
	[2] = {
		.start = INT_DEMUX_2,                    //demux 2 irq
		.end   = INT_DEMUX_2,
		.flags = IORESOURCE_IRQ,
		.name  = "demux2_irq"
	},	
	[3] = {
		.start = INT_ASYNC_FIFO_FLUSH,                   //dvr 0 irq
		.end   = INT_ASYNC_FIFO_FLUSH,
		.flags = IORESOURCE_IRQ,
		.name  = "dvr0_irq"
	},
	[4] = {
		.start = INT_ASYNC_FIFO2_FLUSH,          //dvr 1 irq
		.end   = INT_ASYNC_FIFO2_FLUSH,
		.flags = IORESOURCE_IRQ,
		.name  = "dvr1_irq"
	},	
};

static  struct platform_device amlogic_dvb_device = {
	.name             = "amlogic-dvb",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(amlogic_dvb_resource),
	.resource         = amlogic_dvb_resource,
};
#endif

#ifdef CONFIG_AM_DVB
static struct resource mxl101_resource[]  = {

	[0] = {
		.start = 0,                                    //frontend  i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[1] = {
		.start = 0xc0,                                 //frontend 0 demod address
		.end   = 0xc0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[2] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_pin"
	},
	[3] = {
		.start = 0, //reset enable value
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_value_enable"
	},
	[4] = {
		.start = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bit_bit0_15(3),  //power enable pin
		.end   = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bit_bit0_15(3),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	//	.name  = "frontend0_power_pin"
	},	
	[5] = {
	.start = 0xc0,                                 //is mxl101
	.end   = 0xc0,
	.flags = IORESOURCE_MEM,
	.name  = "frontend0_tuner_addr"
	},	
};

static  struct platform_device mxl101_device = {
	.name             = "mxl101",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(mxl101_resource),
	.resource         = mxl101_resource,
};

static struct resource avl6211_resource[]  = {

	[0] = {
		.start = 0,                                    //frontend  i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[1] = {
		.start = 0xc0,                                 //frontend 0 demod address
		.end   = 0xc0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[2] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_pin"
	},
	[3] = {
		.start = 0, //reset enable value
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_value_enable"
	},
	[4] = {
		.start = (GPIOB_bank_bit0_23(23)<<16)|GPIOB_bit_bit0_23(23),  //DVBS2 LNBON/OFF pin
		.end   = (GPIOB_bank_bit0_23(23)<<16)|GPIOB_bit_bit0_23(23),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_LNBON/OFF"
	},	
	[5] = {
	.start = 0xc0,                                 //is avl6211
	.end   = 0xc0,
	.flags = IORESOURCE_MEM,
	.name  = "frontend0_tuner_addr"
	},	
	[6] = {
	.start = (GPIOB_bank_bit0_23(21)<<16)|GPIOB_bit_bit0_23(21),        //is avl6211
	.end   = (GPIOB_bank_bit0_23(21)<<16)|GPIOB_bit_bit0_23(21),
	.flags = IORESOURCE_MEM,
	.name  = "frontend0_POWERON/OFF"
	},	
	[7] = {
	.start = (GPIOB_bank_bit0_23(20)<<16)|GPIOB_bit_bit0_23(20),
	.end   = (GPIOB_bank_bit0_23(20)<<16)|GPIOB_bit_bit0_23(20),
	.flags = IORESOURCE_MEM,
	.name  = "frontend0_ANTOVERLOAD"
	},		
};

static  struct platform_device avl6211_device = {
	.name             = "avl6211",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(avl6211_resource),
	.resource         = avl6211_resource,
};




static struct resource gx1001_resource[]  = {
	[0] = {
		.start = (GPIOX_bank_bit0_31(31)<<16)|GPIOX_bit_bit0_31(31),                           //frontend 0 reset pin
		.end   = (GPIOX_bank_bit0_31(31)<<16)|GPIOX_bit_bit0_31(31),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0xC0,                                 //frontend 0 tuner address
		.end   = 0xC0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start = 0x18,                                 //frontend 0 demod address
		.end   = 0x18,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
};

static  struct platform_device gx1001_device = {
	.name             = "gx1001",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(gx1001_resource),
	.resource         = gx1001_resource,
};


static struct resource ite9173_resource[]  = {
	[0] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0x9E,                                 //frontend 0 tuner address
		.end   = 0x9E,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start =  0x38,                                 //frontend 0 demod address
		.end   =  0x38,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[4] = {
		.start = (GPIOB_bank_bit0_23(21)<<16)|GPIOB_bit_bit0_23(21),  // TUNER_POWERC pin
		.end   = (GPIOB_bank_bit0_23(21)<<16)|GPIOB_bit_bit0_23(21),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_TUNER_POWER"
	},
	[5] = {
		.start = (GPIOB_bank_bit0_23(20)<<16)|GPIOB_bit_bit0_23(20),  // ANT_OVERLOAD pin
		.end   = (GPIOB_bank_bit0_23(20)<<16)|GPIOB_bit_bit0_23(20),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_ANT_OVERLOAD"
	},
	[6] = {
		.start = (GPIOB_bank_bit0_23(23)<<16)|GPIOB_bit_bit0_23(23),  //ANT_POWER pin
		.end   = (GPIOB_bank_bit0_23(23)<<16)|GPIOB_bit_bit0_23(23),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_ANT_POWER"
	},	
};

static  struct platform_device ite9173_device = {
	.name             = "ite9173",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(ite9173_resource),
	.resource         = ite9173_resource,
};

static struct resource ite9133_resource[]  = {
	[0] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0x98,                                 //frontend 0 tuner address
		.end   = 0x98,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start =  0x38,                                 //frontend 0 demod address
		.end   =  0x38,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[4] = {
		.start = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bank_bit0_15(3),  //// tuner_enable
		.end   = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bank_bit0_15(3),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_power"
	},
};

static  struct platform_device ite9133_device = {
	.name             = "ite9133",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(ite9133_resource),
	.resource         = ite9133_resource,
};



static struct resource dib7090p_resource[]  = {
	[0] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_pin"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0x98,                                 //frontend 0 tuner address
		.end   = 0x98,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start =  0x10,                                 //frontend 0 demod address
		.end   =  0x10,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[4] = {
		.start = (GPIOAO_bank_bit0_11(6)<<16)|GPIOAO_bank_bit0_11(6), //(GPIOC_bank_bit0_15(3)<<16)|GPIOC_bank_bit0_15(3),  //// tuner_enable
		.end   = (GPIOAO_bank_bit0_11(6)<<16)|GPIOAO_bank_bit0_11(6), //(GPIOC_bank_bit0_15(3)<<16)|GPIOC_bank_bit0_15(3),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_power_pin"
	},
	[5] = {
		.start = 0x1,                                 
		.end   = 0x1,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_value_enable"
	},
	[6] = {
		.start =  0x0,                                 
		.end   =  0x0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_power_value_enable"
	},
};

static  struct platform_device dib7090p_device = {
	.name             = "DiB7090P",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(dib7090p_resource),
	.resource         = dib7090p_resource,
};
static struct resource rtl2830_resource[]  = {

	[0] = {
		.start = 0,                                    //frontend  i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[1] = {
		.start = 0x20,                                 //frontend 0 demod address
		.end   = 0x20,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[2] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8), //reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_pin"
	},
	[3] = {
		.start = 0, //reset enable value
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset_value_enable"
	},
	[4] = {
		.start = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bit_bit0_15(3),  //power enable pin
		.end   = (GPIOC_bank_bit0_15(3)<<16)|GPIOC_bit_bit0_15(3),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	//	.name  = "frontend0_power_pin"
	},	
	[5] = {
	.start = 0x34,                                 
	.end   = 0x34,
	.flags = IORESOURCE_MEM,
	.name  = "frontend0_tuner_addr"
	},	
};

static  struct platform_device rtl2830_device = {
	.name             = "rtl2830",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(rtl2830_resource),
	.resource         = rtl2830_resource,
};

static struct resource ds3000_resource[] = {
	[0] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),                           //frontend 0 reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0xC0,                                 //frontend 0 tuner address
		.end   = 0xC0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start = 0xD0,                                 //frontend 0 demod address
		.end   = 0xD0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
};

static  struct platform_device ds3000_device = {
	.name             = "ds3000",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(ds3000_resource),
	.resource         = ds3000_resource,
};
#endif

#if defined(CONFIG_TH_SONY_T2)

static struct resource cxd2834_resource[]  = {

	[0] = {
		.start = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),                           //frontend 0 reset pin
		.end   = (GPIOD_bank_bit0_9(8)<<16)|GPIOD_bit_bit0_9(8),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend  i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0xC0,                                 //frontend 0 tuner address
		.end   = 0xC0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},	
	[3] = {
		.start = 0xD8,                                 //frontend 0 demod address
		.end   = 0xD8,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},

};

static  struct platform_device cxd2834_device = {
	.name             = "cxd2834",
	.id               = 0,
	.num_resources    = ARRAY_SIZE(cxd2834_resource),
	.resource         = cxd2834_resource,
};



#endif

#if defined(CONFIG_AM_SMARTCARD)
static struct resource amlogic_smc_resource[]  = {
	[0] = {
		.start = ((GPIOD_bank_bit2_24(11)<<16) | GPIOD_bit_bit2_24(11)),                          //smc POWER gpio
		.end   = ((GPIOD_bank_bit2_24(11)<<16) | GPIOD_bit_bit2_24(11)),
		.flags = IORESOURCE_MEM,
		.name  = "smc0_power"
	},
	[1] = {
		.start = INT_SMART_CARD,                   //smc irq number
		.end   = INT_SMART_CARD,
		.flags = IORESOURCE_IRQ,
		.name  = "smc0_irq"
	},

};

static  struct platform_device amlogic_smc_device = {
	.name             = "amlogic-smc",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(amlogic_smc_resource),
	.resource         = amlogic_smc_resource,
};
#endif

#if defined(CONFIG_AML_WATCHDOG)
static struct platform_device aml_wdt_device = {
    .name = "aml_wdt",
    .id   = -1,
    .num_resources = 0,
};
#endif

#if defined(CONFIG_AML_HDMI_TX)
static struct hdmi_phy_set_data brd_phy_data[] = {
//    {27, 0xf7, 0x0},    // an example: set Reg0xf7 to 0 in 27MHz
    {-1,   -1},         //end of phy setting
};
static struct hdmi_config_platform_data aml_hdmi_pdata ={
    .hdmi_5v_ctrl = NULL,
    .hdmi_3v3_ctrl = NULL,
    .hdmi_pll_vdd_ctrl = NULL,
    .hdmi_sspll_ctrl = NULL,
    .phy_data = brd_phy_data,
};

static struct platform_device aml_hdmi_device = {
    .name = "amhdmitx",
    .id   = -1,
    .dev  = {
        .platform_data = &aml_hdmi_pdata,
    }
};
#endif
#define ETH_PM_DEV
#if defined(ETH_PM_DEV)
#define ETH_MODE_RMII_EXTERNAL
static void meson_eth_clock_enable(int flag)
{
printk("meson_eth_clock_enable: %x\n", (unsigned int)flag );
}

static void meson_eth_reset(void)
{
    printk("meson_eth_reset\n");
    // Ethernet Reset, GPIO D7, ACTIVE LOW
    gpio_direction_output(GPIO_ETH_RESET, 0);
    mdelay(100);
    gpio_set_value(GPIO_ETH_RESET, 1); 
}

static struct aml_eth_platform_data  aml_pm_eth_platform_data ={
    .clock_enable = meson_eth_clock_enable,
    .reset = meson_eth_reset,
};

struct platform_device meson_device_eth = {
	.name   = "ethernet_pm_driver",
	.id     = -1,
	.dev    = {
		.platform_data = &aml_pm_eth_platform_data,
	}
};
#endif
static struct platform_device __initdata *platform_devs[] = {
#if defined(ETH_PM_DEV)
    &meson_device_eth,
#endif
#if defined(CONFIG_AML_HDMI_TX)
    &aml_hdmi_device,
#endif
#if defined(CONFIG_JPEGLOGO)
    &jpeglogo_device,
#endif
#if defined (CONFIG_AMLOGIC_PM)
    &power_dev,
#endif  

#if defined(CONFIG_FB_AM)
    &fb_device,
#endif
#if defined(CONFIG_AM_STREAMING)
    &codec_device,
#endif
#if defined(CONFIG_AM_VIDEO)
    &deinterlace_device,
#endif
#if defined(CONFIG_AML_AUDIO_DSP)
    &audiodsp_device,
#endif
#if defined(CONFIG_SND_AML_M3)
    &aml_audio,
#endif
#if defined(CONFIG_CARDREADER)
    &amlogic_card_device,
#endif
#if defined(CONFIG_KEYPADS_AM)||defined(CONFIG_VIRTUAL_REMOTE)||defined(CONFIG_KEYPADS_AM_MODULE)
    &input_device,
#endif
#ifdef CONFIG_SARADC_AM
&saradc_device,
#endif
#if defined(CONFIG_ADC_KEYPADS_AM)||defined(CONFIG_ADC_KEYPADS_AM_MODULE)
    &adc_kp_device,
#endif
#if defined(CONFIG_KEY_INPUT_CUSTOM_AM) || defined(CONFIG_KEY_INPUT_CUSTOM_AM_MODULE)
    &input_device_key,  //changed by Elvis
#endif
#if defined(CONFIG_AM_IR_RECEIVER)
    &irreceiver_device,
#endif
#ifdef CONFIG_AM_NAND
    &aml_nand_device,
#endif
#if defined(CONFIG_AML_RTC)
    &aml_rtc_device,
#endif
#if defined(CONFIG_SUSPEND)
    &aml_pm_device,
#endif
#if defined(CONFIG_ANDROID_PMEM)
    &android_pmem_device,
#endif
#if defined(CONFIG_I2C_SW_AML)
    &aml_sw_i2c_device,
#endif
#if defined(CONFIG_AM_UART_WITH_S_CORE)
    &aml_uart_device,
#endif
#if defined(CONFIG_AM_TV_OUTPUT)||defined(CONFIG_AM_TCON_OUTPUT)
    &vout_device,   
#endif
#if defined(CONFIG_AM_TV_OUTPUT2)
    &vout2_device,   
#endif
#ifdef CONFIG_POST_PROCESS_MANAGER
    &ppmgr_device,
#endif
#ifdef CONFIG_FREE_SCALE
        &freescale_device,
#endif   
#ifdef CONFIG_EFUSE
	&aml_efuse_device,
#endif
#ifdef CONFIG_AM_DVB
	&amlogic_dvb_device,
	&mxl101_device,
	&gx1001_device,
	&avl6211_device,
	&ite9173_device,
	&rtl2830_device,
	&ds3000_device,
	&ite9133_device,
	& dib7090p_device,
#endif
#ifdef CONFIG_TH_SONY_T2
        &cxd2834_device,
#endif
#if defined(CONFIG_AM_SMARTCARD)	
	&amlogic_smc_device,
#endif
#if defined(CONFIG_AML_WATCHDOG)
        &aml_wdt_device,
#endif
};

static struct i2c_board_info __initdata aml_i2c_bus_info[] = {

#ifdef CONFIG_IR810_POWEROFF
	{
		I2C_BOARD_INFO("ir810_poweroff",  0x60),
	},
#endif
};

static int __init aml_i2c_init(void)
{
    i2c_register_board_info(0, aml_i2c_bus_info,
        ARRAY_SIZE(aml_i2c_bus_info));
    return 0;
}

#if defined(CONFIG_AM_ETHERNET_EXT_CLK)
	#define NET_EXT_CLK 1
#endif
static void __init eth_pinmux_init(void)
{
	
   CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_6,(3<<17));//reg6[17/18]=0
   #ifdef NET_EXT_CLK
       eth_set_pinmux(ETH_BANK0_GPIOY1_Y9, ETH_CLK_IN_GPIOY0_REG6_18, 0);
   #else
       eth_set_pinmux(ETH_BANK0_GPIOY1_Y9, ETH_CLK_OUT_GPIOY0_REG6_17, 0);
   #endif
	
   CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);           // Disable the Ethernet clocks
	// ---------------------------------------------
	// Test 50Mhz Input Divide by 2
	// ---------------------------------------------
	// Select divide by 2
    CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1<<3));     // desc endianess "same order" 
    CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1<<2));     // ata endianess "little"
    SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1 << 1));     // divide by 2 for 100M
    SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);            // enable Ethernet clocks
    udelay(100);

    // ethernet reset
    meson_eth_reset();
}

static void __init device_pinmux_init(void )
{
    clearall_pinmux();
    /*other deivce power on*/
    /*GPIOA_200e_bit4..usb/eth/YUV power on*/
    //set_gpio_mode(PREG_EGPIO,1<<4,GPIO_OUTPUT_MODE);
    //set_gpio_val(PREG_EGPIO,1<<4,1);
    //uart_set_pinmux(UART_PORT_A,PINMUX_UART_A);
    //uart_set_pinmux(UART_PORT_B,PINMUX_UART_B);
    /*pinmux of eth*/
    eth_pinmux_init();
    aml_i2c_init();
    
    printk("SPDIF output.\n");
		CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_0,(1<<19));
		CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_3,(1<<25));
		CLEAR_CBUS_REG_MASK(PERIPHS_PIN_MUX_7,(1<<17));
		SET_CBUS_REG_MASK(PERIPHS_PIN_MUX_3, (1<<24));
//    set_audio_pinmux(AUDIO_OUT_TEST_N);
   // set_audio_pinmux(AUDIO_IN_JTAG);


	
#ifdef CONFIG_AM_MXL101
	//for mxl101

	//set_mio_mux(3, 0x3F);
	//clear_mio_mux(6, 0x1F<<19);

	set_mio_mux(3, 0x3F<<6);
	clear_mio_mux(0, 0xF);
	clear_mio_mux(5, 0x1<<23);

	clear_mio_mux(0, 1<<6);
	//pwr pin;
	clear_mio_mux(0, 1<<13);
	clear_mio_mux(1, 1<<8);
	//rst pin;
	clear_mio_mux(0, 1<<28);
	clear_mio_mux(1, 1<<20);
/*	set_mio_mux(3, 1<<0);
	set_mio_mux(3, 1<<1);
	set_mio_mux(3, 1<<2);
	set_mio_mux(3, 1<<3);
	set_mio_mux(3, 1<<4);
	clear_mio_mux(0, 1<<6);*/
#endif


#ifdef CONFIG_AM_AVL6211

//for avl6211
	printk("CONFIG_AM_AVL6211 set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x7);


#endif


#ifdef CONFIG_AM_ITE9173
//for ite9173
	printk("CONFIG_AM_ITE9173 set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x7);
#endif

#ifdef CONFIG_AM_RTL2830
//for rtl2830
	printk("CONFIG_AM_RTL2830 set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x7);
#endif

#ifdef CONFIG_AM_DS3000
//for rtl2830
	printk("CONFIG_AM_DS3000 set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x7);
#endif

#ifdef CONFIG_TH_SONY_T2
//for rtl2830
	printk("CONFIG_AM_DS3000 set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x7);
#endif

#ifdef CONFIG_AM_ITE9133

//for ite9133
	printk("CONFIG_AM_ITE9133 set pinmux\n");
	set_mio_mux(3, 0xFFF<<6);
//	clear_mio_mux(0, 1<<4);
	clear_mio_mux(0, 0x3F);


#endif
#ifdef CONFIG_AM_DIB7090P
	printk("CONFIG_AM_DIB7090P set pinmux\n");
	set_mio_mux(3, 0x3F<<6);
	clear_mio_mux(0, 0xF);
	clear_mio_mux(5, 0x1<<23);

/*	clear_mio_mux(0, 1<<6);
	//pwr pin;
	clear_mio_mux(0, 1<<13);
	clear_mio_mux(1, 1<<8);
	//rst pin;
	clear_mio_mux(0, 1<<28);
	clear_mio_mux(1, 1<<20);*/
#endif
}

static void __init  device_clk_setting(void)
{
    /*Demod CLK for eth and sata*/
    //demod_apll_setting(0,1200*CLK_1M);
    /*eth clk*/
    #ifdef NET_EXT_CLK
		eth_clk_set(7, (50 * CLK_1M), (50 * CLK_1M), 1);
	#else    
    	eth_clk_set(ETH_CLKSRC_MISC_CLK, get_misc_pll_clk(), (50 * CLK_1M), 0);
    #endif
    //eth_clk_set(1, get_system_clk(), (50 * CLK_1M), 0);
}

static void disable_unused_model(void)
{
    CLK_GATE_OFF(VIDEO_IN);
    CLK_GATE_OFF(BT656_IN);
  //  CLK_GATE_OFF(ETHERNET);
//    CLK_GATE_OFF(SATA);
//    CLK_GATE_OFF(WIFI);
//    video_dac_disable();
 }
static void __init power_hold(void)
{

	printk(KERN_INFO "power hold set high!\n");

	printk(KERN_INFO "set_vccio and set_vccx2 power up\n");
	// VCCIO +3V3 -- GPIO AO2, ACTIVE HIGH
	gpio_direction_output( GPIO_PWR_VCCIO, 1);

	// VCCx2 +5V -- GPIO AO6, ACTIVE low.
	gpio_direction_output( GPIO_PWR_VCCx2, 0);
	
	printk(KERN_INFO "set_hdmi power up\n");
	// HDMI Power +5V -- GPIO D6, ACTIVE HIGH
	gpio_direction_output( GPIO_PWR_HDMI, 1);

	// Turn On Wifi Power. So the wifi-module can be detected.
	extern_usb_wifi_power(1);

}

#ifdef CONFIG_AML_SUSPEND
extern int (*pm_power_suspend)(void);
#endif

static __init void m1_init_machine(void)
{
    meson_cache_init();
#ifdef CONFIG_AML_SUSPEND
		pm_power_suspend = meson_power_suspend;
#endif /*CONFIG_AML_SUSPEND*/
    
    power_hold();
//    pm_power_off = power_off;		//Elvis fool
    device_clk_setting();
    device_pinmux_init();
//    LED_PWM_REG0_init();

    platform_add_devices(platform_devs, ARRAY_SIZE(platform_devs));

#ifdef CONFIG_USB_DWC_OTG_HCD
    printk("***m1_init_machine: usb set mode.\n");
    set_usb_phy_clk(USB_PHY_CLOCK_SEL_XTAL_DIV2);
	set_usb_phy_id_mode(USB_PHY_PORT_A, USB_PHY_MODE_SW_HOST);
    lm_device_register(&usb_ld_a);
  	set_usb_phy_id_mode(USB_PHY_PORT_B,USB_PHY_MODE_SW_HOST);
    lm_device_register(&usb_ld_b);
#endif
    disable_unused_model();
}

/*VIDEO MEMORY MAPING*/
static __initdata struct map_desc meson_video_mem_desc[] = {
    {
        .virtual    = PAGE_ALIGN(__phys_to_virt(RESERVED_MEM_START)),
        .pfn        = __phys_to_pfn(RESERVED_MEM_START),
        .length     = RESERVED_MEM_END-RESERVED_MEM_START+1,
        .type       = MT_DEVICE,
    },
#ifdef CONFIG_AML_SUSPEND
    {
        .virtual    = PAGE_ALIGN(0xdff00000),
        .pfn        = __phys_to_pfn(0x1ff00000),
        .length     = SZ_1M,
        .type       = MT_MEMORY,
    },
#endif
// #ifdef CONFIG_AML_SECURE_DRIVER
#ifdef CONFIG_AM_IPTV_SECURITY
    {
        .virtual    = PAGE_ALIGN(0xdfe00000),
        .pfn        = __phys_to_pfn(0x9fe00000),
        .length     = SZ_1M,
        .type       = MT_MEMORY,
    },
#endif

};

static __init void m1_map_io(void)
{
    meson_map_io();
    iotable_init(meson_video_mem_desc, ARRAY_SIZE(meson_video_mem_desc));
}

static __init void m1_irq_init(void)
{
    meson_init_irq();
}

static __init void m1_fixup(struct machine_desc *mach, struct tag *tag, char **cmdline, struct meminfo *m)
{
    struct membank *pbank;
    unsigned size;
    
    m->nr_banks = 0;
    pbank=&m->bank[m->nr_banks];
    pbank->start = PAGE_ALIGN(PHYS_MEM_START);
    pbank->size  = SZ_64M & PAGE_MASK;
    pbank->node  = PHYS_TO_NID(PHYS_MEM_START);
    m->nr_banks++;    
    pbank=&m->bank[m->nr_banks];
    pbank->start = PAGE_ALIGN(RESERVED_MEM_END+1);
   size = PHYS_MEM_END-RESERVED_MEM_END;
#ifdef CONFIG_AML_SUSPEND
    pbank->size  = (PHYS_MEM_END-RESERVED_MEM_END-SZ_1M) & PAGE_MASK;
#else
    pbank->size  = (PHYS_MEM_END-RESERVED_MEM_END) & PAGE_MASK;
#endif
// #ifdef CONFIG_ENCRYPT
#ifdef CONFIG_AM_IPTV_SECURITY
	size -= SZ_1M;
#endif
    pbank->size  = size & PAGE_MASK;

    pbank->node  = PHYS_TO_NID(RESERVED_MEM_END+1);
    m->nr_banks++;
}

MACHINE_START(MESON3_8726M_SKT, "AMLOGIC MESON3 8726M SKT SH")
    .phys_io        = MESON_PERIPHS1_PHYS_BASE,
    .io_pg_offst    = (MESON_PERIPHS1_PHYS_BASE >> 18) & 0xfffc,
    .boot_params    = BOOT_PARAMS_OFFSET,
    .map_io         = m1_map_io,
    .init_irq       = m1_irq_init,
    .timer          = &meson_sys_timer,
    .init_machine   = m1_init_machine,
    .fixup          = m1_fixup,
    .video_start    = RESERVED_MEM_START,
    .video_end      = RESERVED_MEM_END,
MACHINE_END
