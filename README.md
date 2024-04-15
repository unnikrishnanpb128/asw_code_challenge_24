# AAC-Clydespace Avionics Software Challenge

Design consideration:
    * modular
    * simplified the modules where ever possible
    * scalable, should be able to accomodate similar devices parallelly
    * to use in bare metal system

Module design:
                                               ______________________
                                              |                      |
               |----------------------------->|    Application       |                              ==========================> main.c
               |                              |______________________|
               |                                        |
               |                                        |
               |                             ___________V____________
               |                            |                        |
               |--------------------------->| Device interface layer |                              ==========================> device_interface/device_interface.c
               |                            |________________________|
               |                                        |
               |                                        |
         ______________                                 |
        |              |                                |
        |   Driver     |                                |                                           ==========================> device_driver/driver_operator.h
        |  operator    |                                |
        |______________|                                |
               |                                        |
               |                    --------------------------------------------------
               |                   |               |                 |                |
               |                   |               |                 |                |
               |               ____V____       ____V____         ____V____        ____V____
               |              |         |     |         |       |         |      |         |
               |------------->| Device  |     | Device  |       | Device  |      | Device  |        ==========================> device_driver/magnetometer_driver.c
                              | Driver1 |     | Driver2 |       | Driver2 |      | Driver2 |                                    device_driver/xxxxxxxxxxxx_driver.c
                              |_________|     |_________|       |_________|      |_________|
                                   |               |                 |                |
                                   |               |                 |                |
                             ______V______   ______V______     ______V______    ______V______
                            |             | |             |   |             |  |             |
                            | peripheral  | | peripheral  |   | peripheral  |  | peripheral  |      ==========================> port/i2c.c
                            |_____________| |_____________|   |_____________|  |_____________|                                  port/yyy.c





Limitation:
    * to use in bare metal system only
    * must not be called form ISR
    * driver functions are implemented as blocking (synchronous) calls


How to build:
    * use Makefile
    * build output name: TestApplication


Usage:
    * Application needs to intialize device interface layer
        device interface layer collectes driver operating functions from all drivers implemented
        driver operating functions are put into structure of pointers in device interface layer
        these functions provide generic way of accessing open, close, read, write and ioctl calls from application
    * Application should use open, close, read, write and ioctl calls to interact with a specific device
        open:   application should specify the type of device to open
                application gets handler once opned successfully
                use this handler for further calls
        close:  cleans up the resources allocated for the device
        read:   gets data from the device
                application must provide required commands and parameters
                driver_operator.h provide commands and parameters for the device
        write:  sets data to the device
                application must provide required commands and parameters
                driver_operator.h provide commands and parameters for the device
        ioctl:  application can use ioctl call to dynamically modify device settings
                this is to override the settings done during opening the device


Design details:
    * Device_interface layer unifies and simplifies the access to the device drivers from application
    * Device_interface layer gathers all device operating function like open, close, read, write and ioctl from all the device drivers
    * Device_interface layer directs the calls to appropriate device driver funcion
    * Device driver implements the generic functions which is called from device_interface layer
    * The generic functions in the device driver makes call to specific drvier functions
    * The generic functions in the device driver are scalable to accomodate more features
    * The generic functions are static (not exposed to outside), this helps all device driver files to have same name for generic functions (init, read, write etc...)
    * Application should use driver_operator.h to pass required commands and parameters to devive driver
    * Inorder to have flexibility with multiple other devices, parameters are specific for any device. Hence it need to be updated in driver_operator.h for every device during its implemetaion
    * port folder has platform specific codes which need to be adapted to a hardware


To adapt to RTOS:
    * Device dreiver should implemet mutex in order to have a locking mechanism
    * Each device dreiver should have seperate mutex, which it should lock and there by block any other calls to same device driver


Note; Due to lack of time, only minimal testing have been performed on the implementation
        platrform codes are not implemented, includeing ISR routines


