/*=========================================================================
| Aardvark Interface Library
|--------------------------------------------------------------------------
| Copyright (c) 2002-2004 Total Phase, Inc.
| All rights reserved.
| www.totalphase.com
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions
| are met:
|
| - Redistributions of source code must retain the above copyright
|   notice, this list of conditions and the following disclaimer.
|
| - Redistributions in binary form must reproduce the above copyright
|   notice, this list of conditions and the following disclaimer in the
|   documentation and/or other materials provided with the distribution.
|
| - Neither the name of Total Phase, Inc. nor the names of its
|   contributors may be used to endorse or promote products derived from
|   this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
|--------------------------------------------------------------------------
| To access Aardvark devices through the API:
|
| 1) Use the following shared object:
|      aardvark.so      --  Linux shared object
|          or
|      aardvark.dll     --  Windows dynamic link library
|
| 2) Along with one of the following language modules:
|      aardvark.c/h     --  C/C++ API header file and interface module
|      aardvark_py.py   --  Python API
|      aardvark.bas     --  Visual Basic API
 ========================================================================*/

#ifndef __aardvark_h__
#define __aardvark_h__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char  aa_u08;
typedef unsigned short aa_u16;
typedef unsigned int   aa_u32;
typedef signed   char  aa_s08;
typedef signed   short aa_s16;
typedef signed   int   aa_s32;

/* Set the following macro to '1' for debugging */
#define AA_DEBUG 0


/*=========================================================================
| VERSION
 ========================================================================*/
#define AA_HEADER_VERSION  0x0300   /* v3.00 */


/*=========================================================================
| STATUS CODES
 ========================================================================*/
/*
 * All API functions return an integer which is the result of the
 * transaction, or a status code if negative.  The status codes are
 * defined as follows:
 */
enum AA_STATUS {
    AA_OK                        =    0,
    AA_UNABLE_TO_LOAD_LIBRARY    =   -1,
    AA_UNABLE_TO_LOAD_DRIVER     =   -2,
    AA_UNABLE_TO_LOAD_FUNCTION   =   -3,
    AA_INCOMPATIBLE_LIBRARY      =   -4,
    AA_INCOMPATIBLE_DEVICE       =   -5,
    AA_COMMUNICATION_ERROR       =   -6,
    AA_UNABLE_TO_OPEN            =   -7,
    AA_UNABLE_TO_CLOSE           =   -8,
    AA_INVALID_HANDLE            =   -9,
    AA_CONFIG_ERROR              =  -10,
    AA_I2C_NOT_AVAILABLE         = -100,
    AA_I2C_NOT_ENABLED           = -101,
    AA_I2C_READ_ERROR            = -102,
    AA_I2C_WRITE_ERROR           = -103,
    AA_I2C_SLAVE_BAD_CONFIG      = -104,
    AA_I2C_SLAVE_READ_ERROR      = -105,
    AA_I2C_SLAVE_TIMEOUT         = -106,
    AA_I2C_DROPPED_EXCESS_BYTES  = -107,
    AA_I2C_BUS_ALREADY_FREE      = -108,
    AA_SPI_NOT_AVAILABLE         = -200,
    AA_SPI_NOT_ENABLED           = -201,
    AA_SPI_WRITE_ERROR           = -202,
    AA_SPI_SLAVE_READ_ERROR      = -203,
    AA_SPI_SLAVE_TIMEOUT         = -204,
    AA_SPI_DROPPED_EXCESS_BYTES  = -205,
    AA_GPIO_NOT_AVAILABLE        = -400,
    AA_I2C_MONITOR_NOT_AVAILABLE = -500,
    AA_I2C_MONITOR_NOT_ENABLED   = -501
};
#ifndef __cplusplus
typedef enum AA_STATUS AA_STATUS;
#endif


/*=========================================================================
| GENERAL TYPE DEFINITIONS
 ========================================================================*/
/*
 * Aardvark handle
 */
typedef int Aardvark;

/*
 * Aardvark version matrix.
 * 
 * This matrix describes the various version dependencies
 * of Aardvark components.  It can be used to determine
 * which component caused an incompatibility error.
 * 
 * All version numbers are of the format:
 *   (major << 8) | minor
 * 
 * ex. v1.20 would be encoded as:  0x0114
 */
struct AardvarkVersion {
    aa_u16 software;
    aa_u16 firmware;
    aa_u16 hardware;
    aa_u16 sw_req_by_fw;
    aa_u16 fw_req_by_sw;
    aa_u16 api_req_by_sw;
};
#ifndef __cplusplus
typedef struct AardvarkVersion AardvarkVersion;
#endif


/*=========================================================================
| GENERAL API
 ========================================================================*/
/*
 * Get a list of ports to which Aardvark devices are attached.
 * 
 * nelem   = maximum number of elements to return
 * devices = array into which the port numbers are returned
 * 
 * Each element of the array is written with the port number.
 * Devices that are in-use are ORed with AA_PORT_NOT_FREE (0x8000).
 *
 * ex.  devices are attached to ports 0, 1, 2
 *      ports 0 and 2 are available, and port 1 is in-use.
 *      array => 0x0000, 0x8001, 0x0002
 * 
 * If the array is NULL, it is not filled with any values.
 * If there are more devices than the array size, only the
 * first nmemb port numbers will be written into the array.
 * 
 * Returns the number of devices found, regardless of the
 * array size.
 */
#define AA_PORT_NOT_FREE 0x8000
int aa_find_devices (
    int      nelem,
    aa_u16 * devices
);


/*
 * Open the Aardvark port.
 * 
 * The port number is a zero-indexed integer.
 *
 * The port number is the same as that obtained from the
 * aa_find_devices() function above.
 * 
 * Returns an Aardvark handle, which is guaranteed to be
 * greater than zero if it is valid.
 * 
 * This function is recommended for use in simple applications
 * where extended information is not required.  For more complex
 * applications, the use of aa_open_ext() is recommended.
 */
Aardvark aa_open (
    int port_number
);


/*
 * Open the Aardvark port, returning extended information
 * in the supplied structure.  Behavior is otherwise identical
 * to aa_open() above.  If 0 is passed as the pointer to the
 * structure, this function is exactly equivalent to aa_open().
 * 
 * The structure is zeroed before the open is attempted.
 * It is filled with whatever information is available.
 * 
 * For example, if the firmware version is not filled, then
 * the device could not be queried for its version number.
 * 
 * This function is recommended for use in complex applications
 * where extended information is required.  For more simple
 * applications, the use of aa_open() is recommended.
 */
struct AardvarkExt {
    AardvarkVersion version;
    int             features;
};
#ifndef __cplusplus
typedef struct AardvarkExt AardvarkExt;
#endif

Aardvark aa_open_ext (
    int           port_number,
    AardvarkExt * aa_ext
);


/*
 * Close the Aardvark port.
 */
int aa_close (
    Aardvark aardvark
);


/*
 * Return the port for this Aardvark handle.
 * 
 * The port number is a zero-indexed integer.
 */
int aa_port (
    Aardvark aardvark
);


/*
 * Return the device features as a bit-mask of values, or
 * an error code if the handle is not valid.
 */
#define AA_FEATURE_SPI 0x00000001
#define AA_FEATURE_I2C 0x00000002
#define AA_FEATURE_GPIO 0x00000008
#define AA_FEATURE_I2C_MONITOR 0x00000010
int aa_features (
    Aardvark aardvark
);


/*
 * Return the status string for the given status code.
 * If the code is not valid or the library function cannot
 * be loaded, return a NULL string.
 */
const char * aa_status_string (
    int status
);


/*
 * Enable logging to a file.  The handle must be standard file
 * descriptor.  In C, a file descriptor can be obtained by using
 * the ANSI C function "open" or by using the function "fileno"
 * on a FILE* stream.  A FILE* stream can be obtained using "fopen"
 * or can correspond to the common "stdout" or "stderr" --
 * available when including stdlib.h
 */
int aa_log (
    Aardvark aardvark,
    int      level,
    int      handle
);


/*
 * Return the version matrix for the device attached to the
 * given handle.  If the handle is 0 or invalid, only the
 * software and required api versions are set.
 */
int aa_version (
    Aardvark          aardvark,
    AardvarkVersion * version
);


/*
 * Configure the device by enabling/disabling I2C, SPI, and
 * GPIO functions.
 */
enum AA_CONFIG {
    AA_CONFIG_GPIO_ONLY = 0x00,
    AA_CONFIG_SPI_GPIO  = 0x01,
    AA_CONFIG_GPIO_I2C  = 0x02,
    AA_CONFIG_SPI_I2C   = 0x03,
    AA_CONFIG_QUERY     = 0x80
};
#ifndef __cplusplus
typedef enum AA_CONFIG AA_CONFIG;
#endif

#define AA_CONFIG_SPI_MASK 0x00000001
#define AA_CONFIG_I2C_MASK 0x00000002
int aa_configure (
    Aardvark  aardvark,
    AA_CONFIG config
);


/*
 * Configure the target power pins.
 * This is only supported on hardware versions >= 2.00
 */
#define AA_TARGET_POWER_NONE 0x00
#define AA_TARGET_POWER_BOTH 0x03
#define AA_TARGET_POWER_QUERY 0x80
int aa_target_power (
    Aardvark aardvark,
    aa_u08   power_mask
);


/*
 * Sleep for the specified number of milliseconds
 * Accuracy depends on the operating system scheduler
 * Returns the number of milliseconds slept
 */
aa_u32 aa_sleep_ms (
    aa_u32 milliseconds
);



/*=========================================================================
| ASYNC MESSAGE POLLING
 ==========================================================================
| Polling function to check if there are any asynchronous
| messages pending for processing. The function takes a timeout
| value in units of milliseconds.  If the timeout is < 0, the
| function will block until data is received.  If the timeout is 0,
| the function will perform a non-blocking chec*/
#define AA_ASYNC_NO_DATA 0x00000000
#define AA_ASYNC_I2C_READ 0x00000001
#define AA_ASYNC_I2C_WRITE 0x00000002
#define AA_ASYNC_SPI 0x00000004
#define AA_ASYNC_I2C_MONITOR 0x00000008
int aa_async_poll (
    Aardvark aardvark,
    int      timeout
);



/*=========================================================================
| I2C API
 ========================================================================*/
/*
 * Free the I2C bus.
 */
int aa_i2c_free_bus (
    Aardvark aardvark
);


/*
 * Set the I2C bit rate in kilohertz.  If a zero is passed as the
 * bitrate, the bitrate is unchanged and the current bitrate is
 * returned.
 */
int aa_i2c_bitrate (
    Aardvark aardvark,
    int      bitrate_khz
);


enum AA_I2C_FLAGS {
    AA_I2C_NO_FLAGS     = 0x00,
    AA_I2C_10_BIT_ADDR  = 0x01,
    AA_I2C_COMBINED_FMT = 0x02,
    AA_I2C_NO_STOP      = 0x04
};
#ifndef __cplusplus
typedef enum AA_I2C_FLAGS AA_I2C_FLAGS;
#endif

/*
 * Read a stream of bytes from the I2C slave device.
 */
int aa_i2c_read (
    Aardvark     aardvark,
    aa_u16       slave_addr,
    AA_I2C_FLAGS flags,
    aa_u16       num_bytes,
    aa_u08 *     data_in
);


/*
 * Read a stream of bytes from the I2C slave device.
 * This API function returns the number of bytes read into
 * the num_read variable.  The return value of the function
 * is a status code.
 */
int aa_i2c_read_ext (
    Aardvark     aardvark,
    aa_u16       slave_addr,
    AA_I2C_FLAGS flags,
    aa_u16       num_bytes,
    aa_u08 *     data_in,
    aa_u16 *     num_read
);


/*
 * Write a stream of bytes to the I2C slave device.
 */
int aa_i2c_write (
    Aardvark       aardvark,
    aa_u16         slave_addr,
    AA_I2C_FLAGS   flags,
    aa_u16         num_bytes,
    const aa_u08 * data_out
);


/*
 * Write a stream of bytes to the I2C slave device.
 * This API function returns the number of bytes written into
 * the num_written variable.  The return value of the function
 * is a status code.
 */
int aa_i2c_write_ext (
    Aardvark       aardvark,
    aa_u16         slave_addr,
    AA_I2C_FLAGS   flags,
    aa_u16         num_bytes,
    const aa_u08 * data_out,
    aa_u16 *       num_written
);


/*
 * Enable/Disable the Aardvark as an I2C slave device
 */
int aa_i2c_slave_enable (
    Aardvark aardvark,
    aa_u08   addr,
    aa_u16   maxTxBytes,
    aa_u16   maxRxBytes
);


int aa_i2c_slave_disable (
    Aardvark aardvark
);


/*
 * Set the slave response in the event the Aardvark is put
 * into slave mode and contacted by a Master.
 */
int aa_i2c_slave_set_response (
    Aardvark       aardvark,
    aa_u08         num_bytes,
    const aa_u08 * data_out
);


/*
 * Return number of bytes written from a previous
 * Aardvark->I2C_master transmission.  Since the transmission is
 * happening asynchronously with respect to the PC host
 * software, there could be responses queued up from many
 * previous write transactions.
 */
int aa_i2c_slave_write_stats (
    Aardvark aardvark
);


/*
 * Read the bytes from an I2C slave reception
 */
int aa_i2c_slave_read (
    Aardvark aardvark,
    aa_u08 * addr,
    aa_u16   num_bytes,
    aa_u08 * data_in
);


/*
 * Extended functions that return status code    
 */
int aa_i2c_slave_write_stats_ext (
    Aardvark aardvark,
    aa_u16 * num_written
);


int aa_i2c_slave_read_ext (
    Aardvark aardvark,
    aa_u08 * addr,
    aa_u16   num_bytes,
    aa_u08 * data_in,
    aa_u16 * num_read
);


/*
 * Enable the I2C bus monitor
 * This disables all other functions on the Aardvark adapter
 */
int aa_i2c_monitor_enable (
    Aardvark aardvark
);


/*
 * Disable the I2C bus monitor
 */
int aa_i2c_monitor_disable (
    Aardvark aardvark
);


/*
 * Read the data collected by the bus monitor
 */
#define AA_I2C_MONITOR_DATA 0x00ff
#define AA_I2C_MONITOR_NACK 0x0100
#define AA_I2C_MONITOR_CMD_START 0xff00
#define AA_I2C_MONITOR_CMD_STOP 0xff01
int aa_i2c_monitor_read (
    Aardvark aardvark,
    aa_u16   num_bytes,
    aa_u16 * data
);


/*
 * Configure the I2C pullup resistors.
 * This is only supported on hardware versions >= 2.00
 */
#define AA_I2C_PULLUP_NONE 0x00
#define AA_I2C_PULLUP_BOTH 0x03
#define AA_I2C_PULLUP_QUERY 0x80
int aa_i2c_pullup (
    Aardvark aardvark,
    aa_u08   pullup_mask
);



/*=========================================================================
| SPI API
 ========================================================================*/
/*
 * Set the SPI bit rate in kilohertz.  If a zero is passed as the
 * bitrate, the bitrate is unchanged and the current bitrate is
 * returned.
 */
int aa_spi_bitrate (
    Aardvark aardvark,
    int      bitrate_khz
);


/*
 * These configuration parameters specify how to clock the
 * bits that are sent and received on the Aardvark SPI
 * interface.
 * 
 *   The polarity option specifies which transition
 *   constitutes the leading edge and which transition is the
 *   falling edge.  For example, AA_SPI_POL_RISING_FALLING
 *   would configure the SPI to idle the SCK clock line low.
 *   The clock would then transition low-to-high on the
 *   leading edge and high-to-low on the trailing edge.
 * 
 *   The phase option determines whether to sample or setup on
 *   the leading edge.  For example, AA_SPI_PHASE_SAMPLE_SETUP
 *   would configure the SPI to sample on the leading edge and
 *   setup on the trailing edge.
 * 
 *   The bitorder option is used to indicate whether LSB or
 *   MSB is shifted first.
 * 
 * See the diagrams in the Aardvark datasheet for
 * more details.
 */
enum AA_SPI_POLARITY {
    AA_SPI_POL_RISING_FALLING = 0,
    AA_SPI_POL_FALLING_RISING = 1
};
#ifndef __cplusplus
typedef enum AA_SPI_POLARITY AA_SPI_POLARITY;
#endif

enum AA_SPI_PHASE {
    AA_SPI_PHASE_SAMPLE_SETUP = 0,
    AA_SPI_PHASE_SETUP_SAMPLE = 1
};
#ifndef __cplusplus
typedef enum AA_SPI_PHASE AA_SPI_PHASE;
#endif

enum AA_SPI_BITORDER {
    AA_SPI_BITORDER_MSB = 0,
    AA_SPI_BITORDER_LSB = 1
};
#ifndef __cplusplus
typedef enum AA_SPI_BITORDER AA_SPI_BITORDER;
#endif

/*
 * Configure the SPI master or slave interface
 */
int aa_spi_configure (
    Aardvark        aardvark,
    AA_SPI_POLARITY polarity,
    AA_SPI_PHASE    phase,
    AA_SPI_BITORDER bitorder
);


/*
 * Write a stream of bytes to the downstream SPI slave device.
 */
int aa_spi_write (
    Aardvark       aardvark,
    aa_u16         num_bytes,
    const aa_u08 * data_out,
    aa_u08 *       data_in
);


/*
 * Enable/Disable the Aardvark as an SPI slave device
 */
int aa_spi_slave_enable (
    Aardvark aardvark
);


int aa_spi_slave_disable (
    Aardvark aardvark
);


/*
 * Set the slave response in the event the Aardvark is put
 * into slave mode and contacted by a Master.
 */
int aa_spi_slave_set_response (
    Aardvark       aardvark,
    aa_u08         num_bytes,
    const aa_u08 * data_out
);


/*
 * Read the bytes from an SPI slave reception
 */
int aa_spi_slave_read (
    Aardvark aardvark,
    aa_u16   num_bytes,
    aa_u08 * data_in
);


/*
 * Change the output polarity on the SS line.
 * 
 * Note: When configured as an SPI slave, the Aardvark will
 * always be setup with SS as active low.  Hence this function
 * only affects the SPI master functions on the Aardvark.
 */
enum AA_SPI_SS_POLARITY {
    AA_SPI_SS_ACTIVE_LOW  = 0,
    AA_SPI_SS_ACTIVE_HIGH = 1
};
#ifndef __cplusplus
typedef enum AA_SPI_SS_POLARITY AA_SPI_SS_POLARITY;
#endif

int aa_spi_master_ss_polarity (
    Aardvark           aardvark,
    AA_SPI_SS_POLARITY polarity
);



/*=========================================================================
| GPIO API
 ========================================================================*/
/*
 * The following enumerated type maps the named lines on the
 * Aardvark I2C/SPI line to bit positions in the GPIO API.
 * All GPIO API functions will index these lines through an
 * 8-bit masked value.  Thus, each bit position in the mask
 * can be referred back its corresponding line through the
 * enumerated type.
 */
enum AA_GPIO_BITS {
    AA_GPIO_SCL  = 0x01,
    AA_GPIO_SDA  = 0x02,
    AA_GPIO_MISO = 0x04,
    AA_GPIO_SCK  = 0x08,
    AA_GPIO_MOSI = 0x10,
    AA_GPIO_SS   = 0x20
};
#ifndef __cplusplus
typedef enum AA_GPIO_BITS AA_GPIO_BITS;
#endif

/*
 * Configure the GPIO, specifying the direction of each bit.
 * 
 * A call to this function will not change the value of the pullup
 * mask in the Aardvark.  This is illustrated by the following
 * example:
 *   (1) Direction mask is first set to 0x00
 *   (2) Pullup is set to 0x01
 *   (3) Direction mask is set to 0x01
 *   (4) Direction mask is later set back to 0x00.
 * 
 * The pullup will be active after (4).
 * 
 * On Aardvark power-up, the default value of the direction
 * mask is 0x00.
 */
#define AA_GPIO_DIR_INPUT 0
#define AA_GPIO_DIR_OUTPUT 1
int aa_gpio_direction (
    Aardvark aardvark,
    aa_u08   direction_mask
);


/*
 * Enable an internal pullup on any of the GPIO input lines.
 * 
 * Note: If a line is configured as an output, the pullup bit
 * for that line will be ignored, though that pullup bit will
 * be cached in case the line is later configured as an input.
 * 
 * By default the pullup mask is 0x00.
 */
#define AA_GPIO_PULLUP_OFF 0
#define AA_GPIO_PULLUP_ON 1
int aa_gpio_pullup (
    Aardvark aardvark,
    aa_u08   pullup_mask
);


/*
 * Read the current digital values on the GPIO input lines.
 * 
 * The bits will be ordered as described by AA_GPIO_BITS.  If a
 * line is configured as an output, its corresponding bit
 * position in the mask will be undefined.
 */
int aa_gpio_get (
    Aardvark aardvark
);


/*
 * Set the outputs on the GPIO lines.
 * 
 * Note: If a line is configured as an input, it will not be
 * affected by this call, but the output value for that line
 * will be cached in the event that the line is later
 * configured as an output.
 */
int aa_gpio_set (
    Aardvark aardvark,
    aa_u08   value
);




#ifdef __cplusplus
}
#endif

#endif  /* __aardvark_h__ */
