#pragma once

#include <stdio.h>
#include <stdint.h>

// typedef uint32_t* DEVICE_HANDLE;

typedef enum {
    DEVICE_TYPE_INVALID = -1,
    DEVICE_TYPE_MAGNETOMETER,
    DEVICE_TYPE_MAX,
} device_type_et;

typedef enum {
    DEVICE_ERR_NONE = 0,
    DEVICE_ERR_COULD_NOT_OPEN,
    DEVICE_ERR_INVALID_DEVICE,
    DEVICE_ERR_COULD_NOT_READ,
    DEVICE_ERR_COULD_NOT_WRITE,
    DEVICE_ERR_COULD_NOT_IOCTL,
    DEVICE_ERR_INVALID_PARAM,
    DEVICE_ERR_INVALID_OPERATION,
    DEVICE_ERR_UNKNOWN
} device_error_et;

typedef struct{
    device_type_et device_type;
    uint32_t* device_handle;
} DEVICE_HANDLE;

void device_interface_init();

device_error_et open(device_type_et const type, DEVICE_HANDLE* handle, void* const param, int32_t const len);

device_error_et close(DEVICE_HANDLE* handle, void* const param, int32_t const len);

device_error_et read(DEVICE_HANDLE* const handle, void* param, int32_t const len);

device_error_et write(DEVICE_HANDLE* const handle, void* param, int32_t const len);

device_error_et ioctl(DEVICE_HANDLE* const handle, void* param, int32_t const len);
