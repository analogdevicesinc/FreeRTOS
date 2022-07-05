/* Taken from https://www.freertos.org/a00110.html */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#include <defSC598.h>

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      1000000000
#define configSYSTICK_CLOCK_HZ                  1000000
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                256
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   3
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               10
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 1
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION             1
#define configSUPPORT_DYNAMIC_ALLOCATION            1
#define configTOTAL_HEAP_SIZE                       (50 * 1024)
#define configAPPLICATION_ALLOCATED_HEAP            0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP   1

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE * 2)

/* Interrupt nesting behaviour configuration. */
/*
 * The FreeRTOS Cortex-A port implements a full interrupt nesting model.
 *
 * Interrupts that are assigned a priority at or below
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which counter-intuitively in the ARM
 * generic interrupt controller [GIC] means a priority that has a numerical
 * value above configMAX_API_CALL_INTERRUPT_PRIORITY) can call FreeRTOS safe API
 * functions and will nest.
 *
 * Interrupts that are assigned a priority above
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which in the GIC means a numerical
 * value below configMAX_API_CALL_INTERRUPT_PRIORITY) cannot call any FreeRTOS
 * API functions, will nest, and will not be masked by FreeRTOS critical
 * sections (although it is necessary for interrupts to be globally disabled
 * extremely briefly as the interrupt mask is updated in the GIC).
 *
 * FreeRTOS functions that can be called from an interrupt are those that end in
 * "FromISR".  FreeRTOS maintains a separate interrupt safe API to enable
 * interrupt entry to be shorter, faster, simpler and smaller.
 *
 * The ADSP-SC5xx Generic Interrupt Controller 32 unique interrupt priorities.  For the
 * purpose of setting configMAX_API_CALL_INTERRUPT_PRIORITY 32 represents the
 * lowest priority.
 */

#define configKERNEL_INTERRUPT_PRIORITY         18
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    18
#define configMAX_API_CALL_INTERRUPT_PRIORITY   18

/* Define to trap errors during development. */
void vAssertCalled( const char * pcFile, unsigned long ulLine );
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0
#define configTOTAL_MPU_REGIONS                                8 /* Default value. */
#define configTEX_S_C_B_FLASH                                  0x07UL /* Default value. */
#define configTEX_S_C_B_SRAM                                   0x07UL /* Default value. */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY            1

/* ARMv8-M secure side port related definitions. */
#define secureconfigMAX_SECURE_CONTEXTS         5

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              1

/* A header file that defines trace macro can be included here. */

/* If configTASK_RETURN_ADDRESS is not defined then a task that attempts to
return from its implementing function will end up in a "task exit error"
function - which contains a call to configASSERT().  However this can give GCC
some problems when it tries to unwind the stack, as the exit error function has
nothing to return to.  To avoid this define configTASK_RETURN_ADDRESS to 0.  */
#define configTASK_RETURN_ADDRESS   NULL


/****** Hardware specific settings. *******************************************/
void vApplicationIRQHandler( uint32_t ulICCIAR );

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the Zynq MPU.  FreeRTOS_Tick_Handler() must
 * be installed as the peripheral's interrupt handler.
 */
void vConfigureTickInterrupt( void );
#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt();

void vClearTickInterrupt( void );

/* The following constants describe the hardware, and are correct for the ADSP-SC59x */
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS       ( REG_GICDST0_EN )
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET ( REG_GICCPU1_CTL - REG_GICDST0_EN )
#define configUNIQUE_INTERRUPT_PRIORITIES          32

/* Required if FPU is used. */
#if ( configUSE_TASK_FPU_SUPPORT == 2 )
   #include <string.h>
#endif


#endif /* FREERTOS_CONFIG_H */
