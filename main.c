#include <stdio.h>
#include "device_interface.h"
#include "driver_operator.h"

int main()
{
    DEVICE_HANDLE handle;
    mag_device_config_st mag_config;
    mag_device_param_st mag_param;
    device_error_et err;
    uint8_t data[10];

    printf("Test application starts\n");

    /* initnalize the driver table */
    device_interface_init();

    /* open magnetometer device, pass configuration parameters required  */
    printf("  >>>>>>>> magnetometer device open \n");
    mag_config.int_cb = NULL;
    mag_config.int_config = 0;
    err = open(DEVICE_TYPE_MAGNETOMETER, &handle, (void*)&mag_config, sizeof(mag_config));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to open magnetometer device, error %d \n", err);
    }

    /* Set ODR, pass command and data required */
    printf("  >>>>>>>> set ODR \n");
    data[0] = (uint8_t)MAG_DEVICE_ODR_20_HZ;
    mag_param.cmd = MAG_DEVICE_CMD_OUTPUT_DATA_RATE;
    mag_param.subcmd = 0;
    mag_param.data_in = &data[0];
    mag_param.data_in_len = 1;
    mag_param.data_out = 0;
    mag_param.data_out_len = 0;
    err = write(&handle, (void*)&mag_param, sizeof(mag_param));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to set ODR into magnetometer device , error %d \n", err);
    } else {
        printf("ODR has been set\n");
    }

    /* Get ODR, pass command and data required */
    printf("  >>>>>>>> get ODR \n");
    mag_param.cmd = MAG_DEVICE_CMD_OUTPUT_DATA_RATE;
    mag_param.subcmd = 0;
    mag_param.data_in = 0;
    mag_param.data_in_len = 0;
    mag_param.data_out = &data[0];
    mag_param.data_out_len = 1;
    err = read(&handle, (void*)&mag_param, sizeof(mag_param));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to set ODR into magnetometer device , error %d \n", err);
    } else {
        printf("ODR value: %d\n", data[0]); // Prints dummy value
    }

    /* Set FS configuration, pass command and data required */
    printf("  >>>>>>>> set FS configuration \n");
    data[0] = (uint8_t)MAG_DEVICE_FULL_SCALE_12_GAUSS;
    mag_param.cmd = MAG_DEVICE_CMD_FULL_SCALE_CONFIGURATION;
    mag_param.subcmd = 0;
    mag_param.data_in = &data[0];
    mag_param.data_in_len = 1;
    mag_param.data_out = 0;
    mag_param.data_out_len = 0;
    err = write(&handle, (void*)&mag_param, sizeof(mag_param));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to set FS configuration into magnetometer device , error %d \n", err);
    } else {
        printf("FS configuration has been set\n");
    }

    /* Get FS configuration, pass command and data required */
    printf("  >>>>>>>> get FS configuration \n");
    mag_param.cmd = MAG_DEVICE_CMD_FULL_SCALE_CONFIGURATION;
    mag_param.subcmd = 0;
    mag_param.data_in = 0;
    mag_param.data_in_len = 0;
    mag_param.data_out = &data[0];
    mag_param.data_out_len = sizeof(data);
    err = read(&handle, (void*)&mag_param, sizeof(mag_param));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to set FS configuration into magnetometer device , error %d \n", err);
    } else {
        printf("FS configuration value: %d \n", data[0]); // Prints dummy value
    }

    /* Configure int pin, pass configuration parameters */
    printf("  >>>>>>>> configure int pin \n");
    mag_config.int_cb = NULL;
    mag_config.int_config = INT_PIN_ENABLE;
    err = ioctl(&handle, (void*)&mag_config, sizeof(mag_config));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to set int pin in magnetometer device , error %d \n", err);
    } else {
        printf("interrupt pin has been set\n");
    }

    /* Read X axis, pass command and data required */
    printf("  >>>>>>>> read X axis \n");
    mag_param.cmd = MAG_DEVICE_CMD_AXIS_OUTPUT;
    mag_param.subcmd = MAG_DEVICE_AXIS_OUTPUT_X;
    mag_param.data_in = 0;
    mag_param.data_in_len = 0;
    mag_param.data_out = &data[0];
    mag_param.data_out_len = sizeof(data);
    err = read(&handle, (void*)&mag_param, sizeof(mag_param));
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to read x axis value of magnetometer device , error %d \n", err);
    } else {
        /* print 16biut value, high and low register */
        printf("X axis value: %d \n", *((uint16_t*)data)); // Prints dummy value
    }

    /* Close magnetometer device */
    printf("  >>>>>>>> close magnetometer device \n");
    err = close(&handle, NULL, 0);
    if(err != DEVICE_ERR_NONE) {
         printf("Error: Failed to close magnetometer device, error %d \n", err);
    }

    // err = open(DEVICE_TYPE_MAGNETOMETER, handle);

    printf("End of test application\n");
}
