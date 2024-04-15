#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include <driver_err.h>
#include <driver_operator.h>
#include <i2c.h>

#define REG_OFFSET_X_REG_L_M    0x05
#define REG_OFFSET_X_REG_H_M    0x06
#define REG_OFFSET_Y_REG_L_M    0x07
#define REG_OFFSET_Y_REG_H_M    0x08
#define REG_OFFSET_Z_REG_L_M    0x09
#define REG_OFFSET_Z_REG_H_M    0x0A
#define REG_WHO_AM_I            0x0F
#define REG_CTRL_REG1           0x20
#define REG_CTRL_REG2           0x21
#define REG_CTRL_REG3           0x22
#define REG_CTRL_REG4           0x23
#define REG_CTRL_REG5           0x24
#define REG_STATUS_REG          0x27
#define REG_OUT_X_L             0x28
#define REG_OUT_X_H             0x29
#define REG_OUT_Y_L             0x2A
#define REG_OUT_Y_H             0x2B
#define REG_OUT_Z_L             0x2C
#define REG_OUT_Z_H             0x2D
#define REG_TEMP_OUT_L          0x2E
#define REG_TEMP_OUT_H          0x2F
#define REG_INT_CFG             0x30
#define REG_INT_SRC             0x31
#define REG_INT_THS_L           0x32
#define REG_INT_THS_H           0x33

#define WHO_AM_I_VAL            0b00111101

#define REG_ODR_MASK            0b00011100
#define REG_FS_MASK             0b01100000


typedef struct
{
    /* interrupt configuration */
    uint32_t int_config;
    /* callback function when interrupt occur */
    interrpt_callback int_cb;

} context_st;


static int32_t get_output_data_rate(int32_t const operation, uint8_t buf[], int32_t const buf_len);
static int32_t set_output_data_rate(int32_t const operation, uint8_t buf[], int32_t const buf_len);
static int32_t get_full_scale_config(int32_t const operation, uint8_t buf[], int32_t const buf_len);
static int32_t set_full_scale_config(int32_t const operation, uint8_t buf[], int32_t const buf_len);
static int32_t get_axis_output(int32_t const operation, uint8_t buf[], int32_t const buf_len);
static int32_t configure_interrupt(const context_st * const context, uint32_t const int_config);
static driver_error_et init(uint32_t *handle, void* const param, int32_t const len);
static driver_error_et config(uint32_t* handle, void* param, int32_t const len);
static driver_error_et deinit(uint32_t* handle, void* const param, int32_t const len);
static driver_error_et write(uint32_t* handle, void* const param, int32_t const len);
static driver_error_et read(uint32_t* handle, void* const param, int32_t const len);


/* 
    ------------------------------------------------------------------- Function definitions ----------------------------------------------------------------------
    Categorized into below
        Generic interface functions
            ---> These functions are exposed to device interface layer
            ---> Exposes init, deint, read, write and config
        Specific driver functions
            ---> These functions are specific implementation of the device ffunctionalities
            ---> These functions are called by generic interface functions
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/



/* 
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------
    ------------------------------------------------------------------- Generic interface functions --------------------------------------------------------------------
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Functions: These function s are static, this enables all other device drivers to have same function name
        initialization
            ---> init()
        configuration
            ---> config()
        deinit
            ---> deinit()
        read
            ---> read()
        write
            ---> write()
*/

/*
    Purpose: update the device opertor table
            This is to pass to device interface layer
            This allows all generic function name be static and hence the generic
                function names can be reused in all other device drivers too

    return: device operator table
*/
device_operation_st get_magnetometer_device_operator()
{
    device_operation_st operator;

    memset(&operator, 0, sizeof(operator));
    operator.open = init;
    operator.close = deinit;
    operator.ioctl = config;
    operator.read = read;
    operator.write = write;

    printf("Created magnetometer device operator table\n");
    return operator;
}

static driver_error_et init(uint32_t *handle, void* const param, int32_t const len)
{
    uint8_t reg_data;
    context_st* context;

    /* Validate the parameter, configuration parameters are requirted to continue */
    if(param == NULL || len != sizeof(mag_device_config_st)) {
        return DRIVER_ERR_INVALID_PARAM;
    }
    /* allocate device context memory */
    context = (context_st*)malloc(sizeof(context_st));
    if(context == NULL) {
        return DRIVER_ERR_MALLOC_FAILED;
    }
    /* pass handle to application */
    handle = (uint32_t*)context;
    /* configure the device, this calls same function of ioctl/config */
    config(handle, param, len);

    /* init i2c peripheral */
    i2c_init(); // DUMMY CODE
    /* configure interrupt pin ISR */
    // platorm code

#if 0 // to be enabled with real device connected
    /* now read a known register and check if device responds */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_WHO_AM_I, 1, &reg_data) != STATUS_OK) {
        /* error, clear the resource allocated */
        deinit(handle, NULL, 0);
        return DRIVER_ERR_COMM_FAILED;
    }
    if(reg_data != WHO_AM_I_VAL) {
        /* error, clear the resource allocated */
        deinit(handle, NULL, 0);
        return DRIVER_ERR_COMM_FAILED;
    }
#endif
    printf("magnetometer device opened\n");

    return DRIVER_ERR_OK;
}

static driver_error_et config(uint32_t* handle, void* const param, int32_t const len)
{
    mag_device_config_st* config;
    context_st* context;

    if(handle == NULL) {
        return DRIVER_ERR_INVALID_HANDLER;
    }
    /* Validate the parameter, configuration parameters are requirted to continue */
    if(param == NULL || len != sizeof(mag_device_config_st)) {
        return DRIVER_ERR_INVALID_PARAM;
    }

    /* copy the input configuration to device context */
    context = (context_st*)handle;
    config = (mag_device_config_st*)param;
    // context->int_cb = config->int_cb; // todo
    // context->int_config = config->int_config; // todo
    /* enable interrupt */
    if(configure_interrupt(context, context->int_config) != 0) {
        return DRIVER_ERR_INVALID_PARAM;
    }

    printf("magnetometer device configured\n");
    
    return DRIVER_ERR_OK;
}

static driver_error_et deinit(uint32_t* handle, void* const param, int32_t const len)
{
    if(handle == NULL) {
        return DRIVER_ERR_INVALID_HANDLER;
    }

    /* disable interrupt if at all enabled */
    configure_interrupt((context_st*)handle, 0);
    /* clean the resources allocated */
    i2c_deinit(); // DUMMY CODE
    // free(handle);
    handle = NULL;

    printf("magnetometer device closed\n");

    return DRIVER_ERR_OK;
}

static driver_error_et read(uint32_t* handle, void* const param, int32_t const len)
{
    mag_device_param_st* device_param;
    driver_error_et ret = DRIVER_ERR_UNKNOWN;

    if(handle == NULL) {
        return DRIVER_ERR_INVALID_HANDLER;
    }

    if(len != sizeof(mag_device_param_st)) {
        return DRIVER_ERR_INVALID_PARAM;
    }

    device_param = (mag_device_param_st*)param;

    switch(device_param->cmd)
    {
        case MAG_DEVICE_CMD_OUTPUT_DATA_RATE:
        {
            if(get_output_data_rate(device_param->subcmd, device_param->data_out, device_param->data_out_len) != 0) {
                ret = DRIVER_ERR_READ_FAILED;
            } else {
                ret = DRIVER_ERR_OK;
            }
            break;
        }
        case MAG_DEVICE_CMD_FULL_SCALE_CONFIGURATION:
        {
            if(get_full_scale_config(device_param->subcmd, device_param->data_out, device_param->data_out_len) != 0) {
                ret = DRIVER_ERR_READ_FAILED;
            } else {
                ret = DRIVER_ERR_OK;
            }
            break;
        }
        case MAG_DEVICE_CMD_AXIS_OUTPUT:
        {
            if(get_axis_output(device_param->subcmd, device_param->data_out, device_param->data_out_len) != 0) {
                ret = DRIVER_ERR_READ_FAILED;
            } else {
                ret = DRIVER_ERR_OK;
            }
            break;
        }
        default:
        {
            ret = DRIVER_ERR_INVALID_COMMAND;
            break;
        }
    }

    printf("magnetometer device read\n");
    return ret;
}

static driver_error_et write(uint32_t* handle, void* const param, int32_t const len)
{
    mag_device_param_st* device_param;
    driver_error_et ret = DRIVER_ERR_UNKNOWN;

    if(handle == NULL) {
        return DRIVER_ERR_INVALID_HANDLER;
    }

    if(len != sizeof(mag_device_param_st)) {
        return DRIVER_ERR_INVALID_PARAM;
    }

    device_param = (mag_device_param_st*)param;

    switch(device_param->cmd)
    {
        case MAG_DEVICE_CMD_OUTPUT_DATA_RATE:
        {
            if(set_output_data_rate(device_param->subcmd, device_param->data_in, device_param->data_in_len) != 0) {
                ret = DRIVER_ERR_WRITE_FAILED;
            } else {
                ret = DRIVER_ERR_OK;
            }
            break;
        }
        case MAG_DEVICE_CMD_FULL_SCALE_CONFIGURATION:
        {
            if(set_output_data_rate(device_param->subcmd, device_param->data_in, device_param->data_in_len) != 0) {
                ret = DRIVER_ERR_WRITE_FAILED;
            } else {
                ret = DRIVER_ERR_OK;
            }
            break;
        }
        default:
        {
            ret = DRIVER_ERR_INVALID_COMMAND;
            break;
        }
    }

     printf("magnetometer device written\n");
    return ret;
}

/* 
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------
    ------------------------------------------------------------------- Specific driver functions ----------------------------------------------------------------------
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Functions are:
        get/set output data rate
            ---> get_output_data_rate()
            ---> set_output_data_rate()
        get/set fullscale configuration
            ---> get_full_scale_config()
            ---> set_full_scale_config()
        get axis data
            ---> get_axis_output()
        interrupt configure
            ---> configure_interrupt()
*/

/*
    purpose: read ODR value from magnetometer register and update the buf with corresponding enum value
 
    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t get_output_data_rate(int32_t const operation, uint8_t buf[], int32_t const buf_len)
{
    uint8_t reg_data;
    uint8_t odr_val;
    status_t i2c_status;

    /* check if buffer is valid, atleast one valid parameter must be present to conitnue */
    if(buf == NULL || buf_len < 1) {
        return -1;
    }
    /* Get the ODR value from device register */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG1, 1, &reg_data) != STATUS_OK) {
        return -2;
    }
    odr_val = (reg_data & REG_ODR_MASK) >> 2;

    /* convert the actual register value to enum */
    switch(odr_val)
    {
        case 0b00000000:
        {
            buf[0] = MAG_DEVICE_ODR_0_625_HZ;
            break;
        }
        case 0b00000001:
        {
            buf[0] = MAG_DEVICE_ODR_1_25_HZ;
            break;
        }
        case 0b00000010:
        {
            buf[0] = MAG_DEVICE_ODR_2_5_HZ;
            break;
        }
        case 0b00000011:
        {
            buf[0] = MAG_DEVICE_ODR_5_HZ;
            break;
        }
        case 0b00000100:
        {
            buf[0] = MAG_DEVICE_ODR_10_HZ;
            break;
        }
        case 0b00000101:
        {
            buf[0] = MAG_DEVICE_ODR_20_HZ;
            break;
        }
        case 0b00000110:
        {
            buf[0] = MAG_DEVICE_ODR_40_HZ;
            break;
        }
        case 0b00000111:
        {
            buf[0] = MAG_DEVICE_ODR_80_HZ;
            break;
        }
        default:
        {
            buf[0] = MAG_DEVICE_ODR_CONFIG_INVLAID;
            break;
        }
    }

    return 0;
}

/*
    Purpose: update the ODR bits provided through buf as enum
            read ctrl reg 1, clear ODR bits and update new value to ODR bits

    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t set_output_data_rate(int32_t const operation, uint8_t buf[], int32_t const buf_len)
{
    uint8_t reg_data;
    uint8_t odr_val = 0;

    /* check if buffer is valid, atleast one valid parameter must be present to conitnue */
    if(buf == NULL || buf_len < 1) {
        return -1;
    }
    
    /* convert the input enum to actual register value */
    switch(buf[0])
    {
        case MAG_DEVICE_ODR_0_625_HZ:
        {
            odr_val = 0b00000000;
            break;
        }
        case MAG_DEVICE_ODR_1_25_HZ:
        {
            odr_val = 0b00000001;
            break;
        }
        case MAG_DEVICE_ODR_2_5_HZ:
        {
            odr_val = 0b00000010;
            break;
        }
        case MAG_DEVICE_ODR_5_HZ:
        {
            odr_val = 0b00000011;
            break;
        }
        case MAG_DEVICE_ODR_10_HZ:
        {
            odr_val = 0b00000100;
            break;
        }
        case MAG_DEVICE_ODR_20_HZ:
        {
            odr_val = 0b00000101;
            break;
        }
        case MAG_DEVICE_ODR_40_HZ:
        {
            odr_val = 0b00000110;
            break;
        }
        case MAG_DEVICE_ODR_80_HZ:
        {
            odr_val = 0b00000111;
            break;
        }
        default:
        {
            break;
        }
    }

    /* Read modify write in order to retain other bits intact */
    /* get the register value first */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG1, 1, &reg_data) != STATUS_OK) {
        return -2;
    }
    /* clear odr bit positions */
    reg_data &= (uint8_t)~REG_ODR_MASK;
    /* move odr bits to appropriate position */
    odr_val <<= 2;
    /* copy odr bits */
    reg_data |= odr_val;

    /* write back the final value */
    if(i2c_write(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG1, 1, &reg_data) != STATUS_OK) {
        return -3;
    }

    return 0;
}

/*
    purpose: read FS value from magnetometer register and update the buf with corresponding enum value
 
    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t get_full_scale_config(int32_t const operation, uint8_t buf[], int32_t const buf_len)
{
    uint8_t reg_data;
    uint8_t fs_val = 0;
    status_t i2c_status;

    /* check if buffer is valid, atleast one valid parameter must be present to conitnue */
    if(buf == NULL || buf_len < 1) {
        return -1;
    }
    /* Get the full scale configuration value from device register */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG2, 1, &reg_data) != STATUS_OK) {
        return -2;
    }
    fs_val = (reg_data & REG_FS_MASK) >> 5;

    /* convert the actual register value to enum */
    switch(fs_val)
    {
        case 0b00000000:
        {
            buf[0] = MAG_DEVICE_FULL_SCALE_4_GAUSS;
            break;
        }
        case 0b00000001:
        {
            buf[0] = MAG_DEVICE_FULL_SCALE_8_GAUSS;
            break;
        }
        case 0b00000010:
        {
            buf[0] = MAG_DEVICE_FULL_SCALE_12_GAUSS;
            break;
        }
        case 0b00000011:
        {
            buf[0] = MAG_DEVICE_FULL_SCALE_16_GAUSS;
            break;
        }
        default:
        {
            buf[0] = MAG_DEVICE_FULL_SCALE_CONFIG_INVLAID;
            break;
        }
    }

    return 0;
}

/*
    Purpose: update the FS bits provided through buf as enum
            read ctrl reg 2, clear FS bits and update new value to FS bits

    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t set_full_scale_config(int32_t const operation, uint8_t buf[], int32_t const  buf_len)
{
    uint8_t reg_data;
    uint8_t fs_val = 0;

    /* check if buffer is valid, atleast one valid parameter must be present to conitnue */
    if(buf == NULL || buf_len < 1) {
        return -1;
    }
    
    /* convert the input enum to actual register value */
    switch(buf[0])
    {
        case MAG_DEVICE_FULL_SCALE_4_GAUSS:
        {
            fs_val = 0b00000000;
            break;
        }
        case MAG_DEVICE_FULL_SCALE_8_GAUSS:
        {
            fs_val = 0b00000001;
            break;
        }
        case MAG_DEVICE_FULL_SCALE_12_GAUSS:
        {
            fs_val = 0b00000010;
            break;
        }
        case MAG_DEVICE_FULL_SCALE_16_GAUSS:
        {
            fs_val = 0b00000011;
            break;
        }
        default:
        {
            break;
        }
    }

    /* Read modify write in order to retain other bits intact */
    /* get the register value first */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG2, 1, &reg_data) != STATUS_OK) {
        return -2;
    }
    /* clear FS bit positions */
    reg_data &= (uint8_t)~REG_FS_MASK;
    /* move FS bits to appropriate position */
    fs_val <<= 5;
    /* copy FS bits */
    reg_data |= fs_val;

    /* write back the final value */
    if(i2c_write(MAGNETOMETER_I2C_ADDRESS, REG_CTRL_REG2, 1, &reg_data) != STATUS_OK) {
        return -3;
    }

    return 0;
}

/*
    Purpose: read magnetometer data for the axis specified

    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t get_axis_output(int32_t const operation, uint8_t buf[], int32_t const len)
{
    uint8_t reg_data[2];
    uint8_t axis_val = 0;
    uint8_t reg_addr;

    /* check if input buffer and output buffer are valid */
    if(buf == NULL || len < 1) {
        return -1;
    }

    /* read high and low device register based on the axis */
    switch(operation)
    {
        case MAG_DEVICE_AXIS_OUTPUT_X:
        {
            if( i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_X_H, 1, &reg_data[0]) != STATUS_OK ||
                i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_X_L, 1, &reg_data[1]) != STATUS_OK )
            {
                return -2;
            }
            break;
        }
        case MAG_DEVICE_AXIS_OUTPUT_Y:
        {
            if( i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_Y_H, 1, &reg_data[0]) != STATUS_OK ||
                i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_Y_L, 1, &reg_data[1]) != STATUS_OK )
            {
                return -2;
            }
            break;
        }
        case MAG_DEVICE_AXIS_OUTPUT_Z:
        {
            if( i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_Z_H, 1, &reg_data[0]) != STATUS_OK ||
                i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_OUT_Z_L, 1, &reg_data[1]) != STATUS_OK )
            {
                return -2;
            }
            break;
        }
        default:
        {
            return -3;
            // break;
        }
    }

    /* now copy the data to output buffer */
    buf[0] = reg_data[0];
    buf[1] = reg_data[1];

    return 0;
}

/*
    Purpose: Configure the interrupt on the device
            currently supports only enabl disable the interrupt on INT pin

    return: 0 if success, negative value for failure for debug purpose
*/
static int32_t configure_interrupt(const context_st * const context, uint32_t const int_config)
{
    uint8_t reg_data;

    /* Read modify write in order to retain other bits intact */
    if(i2c_read(MAGNETOMETER_I2C_ADDRESS, REG_INT_CFG, 1, &reg_data) != STATUS_OK)
    {
        return -1;
    }

    /* set/clear interrupt pin enable bit */
    if(int_config & INT_PIN_ENABLE) {
        reg_data |= (uint8_t)0x01;
        /* register callback function in ISR */
        // context->int_cb // DUMMY CODE
    } else {
        reg_data &= (uint8_t)~0x01;
        /* clear callback function from ISR */
        // context->int_cb // DUMMY CODE
    }

    /* now write back the value */
    if(i2c_write(MAGNETOMETER_I2C_ADDRESS, REG_INT_CFG, 1, &reg_data) != STATUS_OK)
    {
        /* clear callback function from ISR */
        // context->int_cb // DUMMY CODE
        return -2;
    }

    return 0;
}

