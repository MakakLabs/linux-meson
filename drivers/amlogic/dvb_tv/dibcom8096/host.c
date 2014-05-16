#include "common.h"
#include "busdescriptor.h"
#include "busadapter.h"
#include "DiBcom_i2c_app.h"

extern int tune_num;
unsigned char g_DebutI2cData = 0;

struct i2c_state {
    void *priv;
};
/* transmit txcnt bytes to device */
static int I2C_backend_Write(struct i2c_state *i2c, int addr, unsigned char *i2c_tx_buf, int txcnt)
{
    int i;
    int err;

    if (!i2c_tx_buf && txcnt)     /* check tx buffer */
    return 1;

    if(g_DebutI2cData)
    {
        printk("-W- ");
        printk("0x%02x] ", addr);
        printk("[%ld]", txcnt);
        for(i=0;i<txcnt;i++)
        {
            printk(" 0x%02x ", i2c_tx_buf[i]);
        }

        if(txcnt==3)
        {
            printk(" [%d: %d]", i2c_tx_buf[0], (i2c_tx_buf[1]<<8)|i2c_tx_buf[2]);
        }
        else
        {
            printk(" [%d: %d]", (i2c_tx_buf[0]<<8)|i2c_tx_buf[1], (i2c_tx_buf[2]<<8)|i2c_tx_buf[3]);
        }
        printk("\n");
        //printk("\n");
    }
    addr = addr & 0xfe;
    //transmit ...
    if(DiBcom_i2c_write((unsigned char)addr, i2c_tx_buf, txcnt))
    {
        printk("-W- ");
        printk("0x%02x] ", addr);
        printk("DiBcom_i2c_write error\n");
        return 1;
    }
    return 0;
}


/* transmit txcnt bytes to device then read rxcnt bytes */
static int I2C_backend_WriteRead(struct i2c_state *i2c, int addr,  unsigned char  *i2c_tx_buf, int txcnt,
             unsigned char *i2c_rx_buf, int rxcnt)
{
    int i;
    int err;
    int ack;

    if (!i2c_tx_buf && txcnt)     /* check tx buffer */
    return 1;

    if (!i2c_rx_buf && rxcnt)     /* check rx buffer */
    return 1;

    for (i=0 ; i<rxcnt ; i++)     /* initialize receive buffer */
    i2c_rx_buf[i] = 0xfe;

    addr = addr & 0xfe;
    if(DiBcom_i2c_read((unsigned char)addr, i2c_tx_buf, txcnt, i2c_rx_buf, rxcnt))
    {
        printk("-R- ");
        printk("0x%02x] ", addr);
        printk("DiBcom_i2c_read error\n");
        return 1;
    }

    if(g_DebutI2cData)
    {
        printk("-R- ");
        printk("0x%02x] ", addr);
        for(i=0;i<txcnt;i++)
        {
            printk("[w 0x%02x]", i2c_tx_buf[i]);
        }
        for(i=0;i<rxcnt;i++)
        {
            printk(" [r 0x%02x]", i2c_rx_buf[i]);
        }

        if(txcnt == 1)
        {
            printk(" [%d: ", i2c_tx_buf[0]);
        printk(" %d]", (i2c_rx_buf[0]<<8)|i2c_rx_buf[1]);
        }
        else
        {
            printk(" [%d: ", (i2c_tx_buf[0]<<8)|i2c_tx_buf[1]);
            printk(" %d]", (i2c_rx_buf[0]<<8)|i2c_rx_buf[1]);
        }
            printk("\n");
            //printk("\n");
    }

    return 0;
}

static int host_i2c_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *msg)
{
    struct i2c_state *st = client->host->priv;
    int ret = 0;
    msg->address = data_bus_access_device_id(client, msg);
    //printk("dibcom_i2c_master_xfer is called msg->address = 0x%x\n", msg->address);
    if (msg->rx == NULL || msg->rxlen == 0) {
        // implement here the write function and return DIB_RETURN_SUCCESS in case of success
        // return DIB_RETURN_SUCCESS
        ret = I2C_backend_Write(st, msg->address, (unsigned char *)msg->tx, msg->txlen);
    }
    else {
        // implement here the read function and return DIB_RETURN_SUCCESS in case of success
        // return DIB_RETURN_SUCCESS
        ret = I2C_backend_WriteRead(st, msg->address, (unsigned char *)msg->tx, msg->txlen, msg->rx, msg->rxlen);
    }
    return ret;
}

void host_i2c_release(struct dibDataBusHost *i2c_adap)
{
    struct i2c_state *state = i2c_adap->priv;
    DibDbgPrint("-I-  closing I2C\n");

    MemFree(state, sizeof(struct i2c_state));
}

struct dibDataBusHost * host_i2c_interface_attach(void *priv)
{
    struct i2c_state *state = MemAlloc(sizeof(struct i2c_state));
    struct dibDataBusHost *i2c_adap = MemAlloc(sizeof(struct dibDataBusHost));

    data_bus_host_init(i2c_adap, DATA_BUS_I2C, host_i2c_xfer, state);

    state->priv = priv;

    return i2c_adap;
    goto free_mem;
free_mem:
    MemFree(state,sizeof(struct i2c_state));
    return NULL;
}

