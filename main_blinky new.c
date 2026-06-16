/*
 * FreeRTOS V202411.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define mainWorkerOne_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define mainWorkerTwo_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
//TODO define a priority for task three
#define mainWorkerThree_TASK_PRIORITY   ( tskIDLE_PRIORITY + 3 ) // Highest priority to ensure it runs

/* The rate at which data is sent to the queue.  The times are converted from
 * milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainWorkerOne_FREQUENCY_MS      pdMS_TO_TICKS( 1000UL )
#define mainWorkerTwo_FREQUENCY_MS      pdMS_TO_TICKS( 500UL )
//TODO define a period for task three
#define mainWorkerThree_FREQUENCY_MS    pdMS_TO_TICKS( 5000UL ) // 5 seconds

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvWorkerOne( void * pvParameters );
static void prvWorkerTwo( void * pvParameters );
static void prvWorkerThree( void * pvParameters ); // Declaration for the new task

/*
 * The callback function executed when the software timer expires.
 */
static void prvQueueSendTimerCallback( TimerHandle_t xTimerHandle );

/*-----------------------------------------------------------*/

/*user defined variables*/
int uValue;
SemaphoreHandle_t xWorkerMutex;

//TODO define here the array variable so that is visible from task two and three
#define SAMPLE_ARRAY_SIZE 10
int uValueSamples[SAMPLE_ARRAY_SIZE];
int uSampleIndex = 0; // Index to write into the array

// define a semaphore to protect the array access
SemaphoreHandle_t xArrayMutex;


/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky( void )
{
    printf( "\r\nStarting the 3-task demo.\r\n\r\n" );

    uValue = 0;
    // Initialize the samples array
    for(int i = 0; i < SAMPLE_ARRAY_SIZE; i++) {
        uValueSamples[i] = 0;
    }

    // Mutex creation
    xWorkerMutex = xSemaphoreCreateMutex();
    xArrayMutex = xSemaphoreCreateMutex(); // Create the mutex for the array

    if( (xWorkerMutex != NULL) && (xArrayMutex != NULL) )
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate( prvWorkerOne, "WorkerOne", configMINIMAL_STACK_SIZE, NULL, mainWorkerOne_TASK_PRIORITY, NULL );
        xTaskCreate( prvWorkerTwo, "WorkerTwo", configMINIMAL_STACK_SIZE, NULL, mainWorkerTwo_TASK_PRIORITY, NULL );

        //TODO: create a third task called WorkerThree that executes peridically every 5 seconds
        // properly assign a priority to this task and set the period variable
        xTaskCreate( prvWorkerThree, "WorkerThree", configMINIMAL_STACK_SIZE, NULL, mainWorkerThree_TASK_PRIORITY, NULL );

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

static void prvWorkerTwo( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainWorkerTwo_FREQUENCY_MS;
    
    ( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();
    
    for( ; ; )
    {
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );

        // Take mutex for uValue
        if( xSemaphoreTake( xWorkerMutex, portMAX_DELAY ) == pdTRUE )
        {
            uValue++;
            printf( "[TWO] uValue : %d\r\n", uValue );
            fflush( stdout );
            
           +- // Give mutex for uValue
            xSemaphoreGive( xWorkerMutex );
        }

        //TODO add the code to write the current uValue into the array
        // remember to use the array in another critical section
        if( xSemaphoreTake( xArrayMutex, portMAX_DELAY ) == pdTRUE )
        {
            uValueSamples[uSampleIndex] = uValue;
            uSampleIndex++;
            // Wrap the index around if it reaches the end of the array
            if( uSampleIndex >= SAMPLE_ARRAY_SIZE )
            {
                uSampleIndex = 0;
            }
            xSemaphoreGive( xArrayMutex );
        }
    }
}
/*-----------------------------------------------------------*/
  
static void prvWorkerOne( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainWorkerOne_FREQUENCY_MS;

    ( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();
    
    for( ; ; )
    {
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );

        // Take mutex
        if( xSemaphoreTake( xWorkerMutex, portMAX_DELAY ) == pdTRUE )
        {
            uValue++;
            printf( "[ONE] uValue : %d\r\n", uValue );
            fflush( stdout );

            // Give mutex
            xSemaphoreGive( xWorkerMutex );
        }
    }
}
/*-----------------------------------------------------------*/

//TODO: realize task 3 code as a periodic task as one and two
// Every period this task computes the average of the variable uValue that is modified by the other two task
// How to do that?
// uValue samples can be stored by Task Two in an array of size 10
// then task three every period computes the average from the array (in mutual exclusion)
// then print the average value
static void prvWorkerThree( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainWorkerThree_FREQUENCY_MS;

    ( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );

        long sum = 0;
        float average = 0.0f;
        int integerPart = 0;
        int fractionalPart = 0;

        // Take the array mutex to safely read the samples
        if( xSemaphoreTake( xArrayMutex, portMAX_DELAY ) == pdTRUE )
        {
            for( int i = 0; i < SAMPLE_ARRAY_SIZE; i++ )
            {
                sum += uValueSamples[i];
            }
            
            // Give the mutex as soon as we are done with the array
            xSemaphoreGive( xArrayMutex );
        }

        average = (float)sum / (float)SAMPLE_ARRAY_SIZE;

        // --- FIX: Manually handle float printing ---
        // Some minimal printf libraries do not support %f.
        // We calculate the integer and fractional parts separately.
        integerPart = (int)average;
        
        fractionalPart = (int)((average - integerPart) * 100);

        printf( "----------------------------------------\r\n" );
        printf( "[THREE] Average of last %d samples: %d.%02d\r\n", SAMPLE_ARRAY_SIZE, integerPart, fractionalPart );
        printf( "----------------------------------------\r\n" );
        fflush( stdout );
    }
}

/*-----------------------------------------------------------*/
// This part is not used by our three worker tasks but is kept for completeness
// from the original demo file.
static void prvQueueSendTimerCallback( TimerHandle_t xTimerHandle )
{
    // This function is not relevant to our current logic
}

void vBlinkyKeyboardInterruptHandler( int xKeyPressed )
{
    // This function is not relevant to our current logic
}
