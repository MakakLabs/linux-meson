#ifndef DIB7000_XXXXXXXX
#define DIB7000_XXXXXXXX

int dib_init(void);
void dib_tune(unsigned int frequency,unsigned char bandwidth);
unsigned char dib_lockstatus(void);
void dib_signal_strenth_quality(unsigned char *signal_quality, unsigned char *signal_strength);
void dibcom_release(void);
void dibcom_sleep(void);
void dibcom_deep_sleep(void);
unsigned int dibcom_read_ber(void);
unsigned int dibcom_read_signal_strength(void);
unsigned int dibcom_read_snr(void);



#endif

