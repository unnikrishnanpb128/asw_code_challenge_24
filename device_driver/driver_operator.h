#pragma once

#include <stdint.h>
#include <driver_err.h>

#define X_AXIS_INT_ENABLE       0x01
#define Y_AXIS_INT_ENABLE       0x02
#define Z_AXIS_INT_ENABLE       0x04
#define INT_PIN_ENABLE          0x08


typedef enum {
    MAG_DEVICE_CMD_INVLAID = -1,
    MAG_DEVICE_CMD_OUTPUT_DATA_RATE,
    MAG_DEVICE_CMD_FULL_SCALE_CONFIGURATION,
    MAG_DEVICE_CMD_AXIS_OUTPUT,
    MAG_DEVICE_CMD_MAX
} mag_device_cmd_et;

typedef enum {
    MAG_DEVICE_SUBCMD_INVLAID = -1,
    MAG_DEVICE_SUBCMD_MAX
} mag_device_subcmd_et;

typedef enum {
    MAG_DEVICE_ODR_CONFIG_INVLAID = -1,
    MAG_DEVICE_ODR_0_625_HZ,
    MAG_DEVICE_ODR_1_25_HZ,
    MAG_DEVICE_ODR_2_5_HZ,
    MAG_DEVICE_ODR_5_HZ,
    MAG_DEVICE_ODR_10_HZ,
    MAG_DEVICE_ODR_20_HZ,
    MAG_DEVICE_ODR_40_HZ,
    MAG_DEVICE_ODR_80_HZ,
    MAG_DEVICE_ODR_CONFIG_MAX
} mag_device_odr_config_et;

typedef enum {
    MAG_DEVICE_FULL_SCALE_CONFIG_INVLAID = -1,
    MAG_DEVICE_FULL_SCALE_4_GAUSS,
    MAG_DEVICE_FULL_SCALE_8_GAUSS,
    MAG_DEVICE_FULL_SCALE_12_GAUSS,
    MAG_DEVICE_FULL_SCALE_16_GAUSS,
    MAG_DEVICE_FULL_SCALE_CONFIG_MAX
} mag_device_full_scale_config_et;

typedef enum {
    MAG_DEVICE_AXIS_OUTPUT_INVLAID = -1,
    MAG_DEVICE_AXIS_OUTPUT_X,
    MAG_DEVICE_AXIS_OUTPUT_Y,
    MAG_DEVICE_AXIS_OUTPUT_Z,
    MAG_DEVICE_AXIS_OUTPUT_MAX
} mag_device_axis_output_et;

typedef struct {
    int32_t cmd;
    int32_t subcmd;
    uint8_t *data_in;
    int32_t data_in_len;
    uint8_t *data_out;
    int32_t data_out_len;
} mag_device_param_st;

typedef void (*interrpt_callback)(void* arg);

typedef struct {
   uint32_t int_config; 
   interrpt_callback int_cb;
} mag_device_config_st;


typedef driver_error_et (*device_operation)(uint32_t *handle, void* const param, int32_t const len);

typedef struct {
    device_operation open;
    device_operation close;
    device_operation read;
    device_operation write;
    device_operation ioctl;
} device_operation_st;

