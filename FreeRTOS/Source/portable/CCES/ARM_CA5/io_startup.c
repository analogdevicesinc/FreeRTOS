/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libio/device.h>
#include <drivers/uart/adi_uart.h>
#include <services/pwr/adi_pwr.h>
#include "semphr.h"

#include "io_startup.h"

static ADI_UART_HANDLE  deviceHandle;
static uint8_t          interruptModeMemory[ADI_UART_BIDIR_INT_MEMORY_SIZE];

static ADI_UART_RESULT uart_result;
static SemaphoreHandle_t xSemaphore = NULL;

/* Called once to initialize and configure the UART I/O device. Returns either
 * _DEV_IS_THREADSAFE or !_DEV_IS_THREADSAFE to indicate whether the device is
 * thread-safe or not.
 */
int adi_freertos_uart_init(struct DevEntry *d) {

    ADI_PWR_RESULT pwr_result = adi_pwr_Init(0u, UART_ADI_FREERTOS_CLKIN);
    if (pwr_result != ADI_PWR_SUCCESS
        && pwr_result != ADI_PWR_DEVICE_IN_USE) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_Open(ADI_FREERTOS_UART_DEV_ID,
                           ADI_UART_DIR_BIDIRECTION,
                           interruptModeMemory,
                           ADI_UART_BIDIR_INT_MEMORY_SIZE,
                           &deviceHandle);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_SetMode(deviceHandle, ADI_UART_MODE_UART);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_SetBaudRate(deviceHandle, ADI_FREERTOS_UART_BAUD_RATE);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_SetNumStopBits(deviceHandle, ADI_UART_ONE_STOPBIT);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_SetParity (deviceHandle, ADI_UART_NO_PARITY);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    uart_result = adi_uart_SetWordLen(deviceHandle, ADI_UART_WORDLEN_8BITS);
    if (uart_result != ADI_UART_SUCCESS) return ADI_FREERTOS_UART_IO_ERR;

    xSemaphore = xSemaphoreCreateMutex();
    configASSERT( xSemaphore != NULL );

    return !_DEV_IS_THREADSAFE;
}

/* Instructs the device to copy len bytes from the file fd into the character
 * array pointed to by buf.
 * Returns zero on success or the number of bytes not yet read on failure.
 */
int adi_freertos_uart_read(int fd, unsigned char *buf, int len) {
	uart_result = adi_uart_Read(deviceHandle, buf, len);
    return (uart_result == ADI_UART_SUCCESS) ? 0 : len;
}

/* Instructs the device to copy len bytes from the character array pointed
 * to by buf into the file fd.
 * Returns zero on success or the number of bytes not yet written on failure.
 */
int adi_freertos_uart_write(int fd, unsigned char *buf, int len) {
	char *buf_save = ( char * ) pvPortMalloc( sizeof( char ) * len );
	configASSERT( buf_save != NULL );
	memcpy(buf_save, buf, len);
	xSemaphoreTake( xSemaphore, portMAX_DELAY  );
	adi_osal_EnterCriticalRegion();
	uart_result = adi_uart_Write(deviceHandle, buf_save, len);
	adi_osal_ExitCriticalRegion();
	xSemaphoreGive( xSemaphore );
	vPortFree( buf_save );
    return (uart_result == ADI_UART_SUCCESS) ? 0 : len;
}

/* Returns the last error to occur from this device interface, or zero if no
 * error has occurred.
 */
int adi_freertos_uart_get_errno(void) {
    return uart_result;
}

/* If a device does not support some DevEntry functions, dummy functions
 * that simply return failure must be provided.
 */
int  adi_freertos_stub_open(const char *path, int flags)         { return -1;  }
int  adi_freertos_stub_close(int fd)                             { return -1;  }
long adi_freertos_stub_seek(int fd, long offset, int whence)     { return -1L; }
int  adi_freertos_stub_unlink(const char *filename)              { return -1;  }
int  adi_freertos_stub_rename(
                       const char *oldname, const char *newname) { return -1;  }
int  adi_freertos_stub_system(const char *cmd)                   { return -1;  }
int  adi_freertos_stub_isatty(int fd)                            { return -1;  }
clock_t adi_freertos_stub_times(void)                            { return -1;  }
void adi_freertos_stub_gettimeofday(
                                 struct timeval *tp, void *tzvp) { return;     }
int  adi_freertos_stub_kill(int pID, int signal)                 { return -1;  }


/* Overwrite default I/O devices table at build time to register UART peripheral
 * as default I/O device.
 */
DevEntry uart_io_device = {
    ADI_FREERTOS_UART_DEV_ID,
    NULL,
    adi_freertos_uart_init,
    adi_freertos_stub_open,
    adi_freertos_stub_close,
    adi_freertos_uart_write,
    adi_freertos_uart_read,
    adi_freertos_stub_seek,
	STDIN_FILENO,
	STDOUT_FILENO,
	STDERR_FILENO,
    adi_freertos_stub_unlink,
    adi_freertos_stub_rename,
    adi_freertos_stub_system,
    adi_freertos_stub_isatty,
    adi_freertos_stub_times,
    adi_freertos_stub_gettimeofday,
    adi_freertos_stub_kill,
    adi_freertos_uart_get_errno
};
int __default_io_device = ADI_FREERTOS_UART_DEV_ID;
DevEntry_t DevDrvTable[] = { &uart_io_device, NULL };
