#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <windows.h>

/*-----------------------------------------------------------
 * Basic FreeRTOS settings
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             1
#define configUSE_TICK_HOOK             1
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 1000 )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES            ( 7 )
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 512 )
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 64 * 1024 ) )
#define configMAX_TASK_NAME_LEN         ( 16 )
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1

/*-----------------------------------------------------------
 * Software timer definitions
 *----------------------------------------------------------*/
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       ( 2 )
#define configTIMER_QUEUE_LENGTH        5
#define configTIMER_TASK_STACK_DEPTH    ( configMINIMAL_STACK_SIZE * 2 )

/*-----------------------------------------------------------
 * Optional features
 *----------------------------------------------------------*/
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_APPLICATION_TASK_TAG  0

/* Hook function related definitions */
#define configUSE_DAEMON_TASK_STARTUP_HOOK 1

/*-----------------------------------------------------------
 * Memory allocation support
 *----------------------------------------------------------*/
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1

/*-----------------------------------------------------------
 * Assert handling
 *----------------------------------------------------------*/
#define configASSERT(x) if ((x) == 0) vAssertCalled(__FILE__, __LINE__)

/*-----------------------------------------------------------
 * API function inclusion
 * (optional — keeps binary smaller if unused functions disabled)
 *----------------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_xTaskResumeFromISR      1
#define INCLUDE_xTaskGetSchedulerState  1
#define INCLUDE_xTaskGetIdleTaskHandle  1
#define INCLUDE_xTaskGetHandle          1
#define INCLUDE_uxTaskGetStackHighWaterMark 1

#endif /* FREERTOS_CONFIG_H */