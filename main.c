#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <math.h>

void vAssertCalled(const char* pcFile, unsigned long ulLine);

void vApplicationMallocFailedHook(void)
{
    printf("Malloc failed!\n");
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

// Queue handle for sending temperature readings
static QueueHandle_t xTempQueue = NULL;

// Timer handle for periodic temperature sampling
static TimerHandle_t xTempTimer = NULL;

// PID constants
#define SETPOINT 25.0f   // Target temperature
#define KP 3.0f          // Proportional gain
#define KI 0.1f          // Integral gain
#define KD 1.5f          // Derivative gain

// Global simulated temperature
static float currentTemp = 25.0f;

// PID state
static float integral = 0.0f;
static float lastError = 0.0f;

// Last computed cooling power (0–100%)
static float coolingPower = 0.0f;

// PID function
float computePID(float setpoint, float measured)
{
    // Flip error direction
    float error = measured - setpoint;  

    integral += error;
    float derivative = error - lastError;

    float output = KP * error + KI * integral + KD * derivative;

    lastError = error;

    // Clamp between 0 and 100 (cooling power %)
    if (output < 0) {
        output = 0;
        // Anti-windup: undo last integral update when saturated low
        integral -= error;
    }
    else if (output > 100) {
        output = 100;
        // Anti-windup: undo last integral update when saturated high
        integral -= error;
    }

    return output;
}

// Simulated temperature sensor with environment effects
float readTemperatureSensor()
{
    // Base physics using PID cooling power (0–100%)
    // Stronger effect if power is higher
    currentTemp -= (coolingPower / 100.0f) * 1.0f; // instead of 0.6f
    currentTemp += 0.2f;                            // natural heating

    // Environmental random disturbance
    currentTemp += ((rand() % 100) / 100.0f - 0.5f) * 0.6f; // ±0.3°C noise

    // Clamp
    if (currentTemp < 15.0f) currentTemp = 15.0f;
    if (currentTemp > 40.0f) currentTemp = 40.0f;

    // Simulated sensor noise
    float sensorReading = currentTemp + ((rand() % 100) / 100.0f - 0.5f) * 1.0f; // ±0.5°C

    return sensorReading; // float for display
}

// Task to receive temperature data and run PID controller
void vControlTask(void* pvParameters)
{
    int receivedTemp;
    while (1)
    {
        if (xQueueReceive(xTempQueue, &receivedTemp, portMAX_DELAY) == pdPASS)
        {
            // Compute new cooling power using PID
            coolingPower = computePID(SETPOINT, (float)receivedTemp);

            // Print output
            printf("Temperature received: %d C - Cooling Power: %.1f%%\n",
                   receivedTemp, coolingPower);
        }
    }
}

// Timer callback to read temperature periodically
void vTempTimerCallback(TimerHandle_t xTimer)
{
    int temp = (int)readTemperatureSensor();
    xQueueSend(xTempQueue, &temp, 0);
}

/* Forward declarations for FreeRTOS hook functions */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vAssertCalled(const char *file, unsigned long line);

/* Provide stack size for the timer task */
const uint32_t uxTimerTaskStack = configTIMER_TASK_STACK_DEPTH;

int main(void)
{
    printf("FreeRTOS Temperature Monitoring & Control Simulation (PID Version)\n");

    // Create queue (length 5, int size)
    xTempQueue = xQueueCreate(5, sizeof(int));

    // Create control task
    xTaskCreate(vControlTask, "ControlTask", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);

    // Create software timer for periodic sampling every 1 second
    xTempTimer = xTimerCreate("TempTimer", pdMS_TO_TICKS(1000), pdTRUE, 0, vTempTimerCallback);
    if (xTempTimer != NULL)
    {
        xTimerStart(xTempTimer, 0);
    }

    // Start scheduler
    vTaskStartScheduler();

    for (;;);
    return 0;
}

/*-----------------------------------------------------------
 * FreeRTOS required application hook functions and stubs
 *----------------------------------------------------------*/

void vAssertCalled(const char* pcFile, unsigned long ulLine)
{
    printf("ASSERT FAILED in %s at line %lu\n", pcFile, ulLine);
    fflush(stdout);
    for (;; );
}

void vApplicationIdleHook(void) {}
void vApplicationTickHook(void) {}

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
    StackType_t** ppxTimerTaskStackBuffer,
    uint32_t* pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationDaemonTaskStartupHook(void) {}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("Stack overflow in task: %s\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    for( ;; );
}