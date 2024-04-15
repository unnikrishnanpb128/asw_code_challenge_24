#include <string.h>
#include <device_interface.h>
#include <magnetometer_driver.h>

/* crete a table to load device operators */
device_operation_st device[DEVICE_TYPE_MAX];

void device_interface_init()
{
    /* clear the device operator table, uninitialized device's table will set to NULL */
    memset(device, 0, sizeof(device_operation_st));
    /* initialize device operator table */
    device[DEVICE_TYPE_MAGNETOMETER] = get_magnetometer_device_operator();
}

device_error_et open(device_type_et const type, DEVICE_HANDLE* handle, void* const param, int32_t const len)
{
    device_error_et ret = DEVICE_ERR_UNKNOWN;

    if(type <= DEVICE_TYPE_INVALID &&  type >= DEVICE_TYPE_MAX) {
        return DEVICE_ERR_INVALID_PARAM;
    }

    /* route to appropriate device's open function */
    if(device[type].open == NULL) {
        return DEVICE_ERR_INVALID_OPERATION;
    }
    ret = device[type].open(handle->device_handle, param, len);
    handle->device_type = type;

    return ret;
}

device_error_et close(DEVICE_HANDLE* handle, void* const param, int32_t const len)
{
    device_error_et ret = DEVICE_ERR_UNKNOWN;

    if(handle == NULL) {
        return DEVICE_ERR_INVALID_PARAM;
    }

    /* route to appropriate device's close function */
    if(device[handle->device_type].close == NULL) {
        return DEVICE_ERR_INVALID_OPERATION;
    }
    ret = device[handle->device_type].close(handle->device_handle, param, len);

    return ret;
}

device_error_et read(DEVICE_HANDLE* handle, void* const param, int32_t const len)
{
    device_error_et ret = DEVICE_ERR_UNKNOWN;

    if(handle == NULL) {
        return DEVICE_ERR_INVALID_PARAM;
    }

    /* route to appropriate device's read function */
    if(device[handle->device_type].read == NULL) {
        return DEVICE_ERR_INVALID_OPERATION;
    }
    ret = device[handle->device_type].read(handle->device_handle, param, len);

    return ret;

}

device_error_et write(DEVICE_HANDLE* handle, void* param, int32_t const len)
{
    device_error_et ret = DEVICE_ERR_UNKNOWN;

    if(handle == NULL) {
        return DEVICE_ERR_INVALID_PARAM;
    }

    /* route to appropriate device's write function */
    if(device[handle->device_type].write == NULL) {
        return DEVICE_ERR_INVALID_OPERATION;
    }
    ret = device[handle->device_type].write(handle->device_handle, param, len);

    return ret;
}

device_error_et ioctl(DEVICE_HANDLE* handle, void* param, int32_t const len)
{
    device_error_et ret = DEVICE_ERR_UNKNOWN;

    if(handle == NULL) {
        return DEVICE_ERR_INVALID_PARAM;
    }

    /* route to appropriate device's ioctl function */
    if(device[handle->device_type].ioctl == NULL) {
        return DEVICE_ERR_INVALID_OPERATION;
    }
    ret = device[handle->device_type].ioctl(handle->device_handle, param, len);

    return ret;
}
