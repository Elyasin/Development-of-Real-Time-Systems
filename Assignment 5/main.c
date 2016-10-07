/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* This demo uses heap_5.c, and these constants define the sizes of the regions
 that make up the total heap.  This is only done to provide an example of heap_5
 being used as this demo could easily create one large heap region instead of
 multiple smaller heap regions - in which case heap_4.c would be the more
 appropriate choice. */
#define mainREGION_1_SIZE	3001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	1107

/*
 * This demo uses heap_5.c, so start by defining some heap regions.  This is
 * only done to provide an example as this demo could easily create one large
 * heap region instead of multiple smaller heap regions - in which case heap_4.c
 * would be the more appropriate choice.  No initialisation is required when
 * heap_4.c is used.
 */
static void prvInitialiseHeap(void);

// Handles for tasks and queues
xTaskHandle matrix_handle = 0;
xTaskHandle reader_handle = 0;
QueueHandle_t matrix_queue = 0;

/*
 * Queue message
 */
struct Message {
	double **c;
} xMessage;

/*
 * Matrix task
 */
#define SIZE 10
#define ROW SIZE
#define COL SIZE
static void matrix_task() {
	int i;
	double **a = (double **) pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++)
		a[i] = (double *) pvPortMalloc(COL * sizeof(double));
	double **b = (double **) pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++)
		b[i] = (double *) pvPortMalloc(COL * sizeof(double));
	double **c = (double **) pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++)
		c[i] = (double *) pvPortMalloc(COL * sizeof(double));

	double sum = 0.0;
	int j, k, l;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			a[i][j] = 1.5;
			b[i][j] = 2.6;
		}
	}

	while (1) {
		/*
		 * In an embedded systems, matrix multiplication would block the CPU for a long time
		 * but since this is a PC simulator we must add one additional dummy delay.
		 */
		long simulationdelay;
		for (simulationdelay = 0; simulationdelay < 1000000000;
				simulationdelay++)
			;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				c[i][j] = 0.0;
			}
		}

		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				sum = 0.0;
				for (k = 0; k < SIZE; k++) {
					for (l = 0; l < 10; l++) {
						sum = sum + a[i][k] * b[k][j];
					}
				}
				c[i][j] = sum;
			}
		}
		struct Message *pxMessage;
		xMessage.c = c;
		pxMessage = &xMessage;
		if (xQueueSend(matrix_queue, &pxMessage, (TickType_t ) 0) != pdPASS) {
			printf("Failed to post the message.\n");
			fflush(stdout);
		}
		vTaskDelay(100);
	}
}

// Reader task to receive from the queue
void reader_task(void *pvParameters) {

	while (1) {
		if (matrix_queue != 0) {

			if (uxQueueMessagesWaiting(matrix_queue)) {
				// Receive a message on the created queue.
				// Don't block if a message is not immediately
				// available.
				struct Message *pxRxedMessage;
				if (xQueueReceive(matrix_queue, &pxRxedMessage,
						(TickType_t ) 0)) {
					int i, j;
					for (i = 0; i < SIZE; i++) {
						for (j = 0; j < SIZE; j++) {
							printf("[%d][%d] = %.2f ", i, j,
									pxRxedMessage->c[i][j]);
							fflush(stdout);
						}
						printf("\n");
						fflush(stdout);
					}
					printf("\n");
					fflush(stdout);
				}
			}
		}
	}
}

/* The main function is the starting point for the program. The two tasks
 (task1 and task2) are created here. The program runs until it is halted.*/
int main(void) {
	/* This demo uses heap_5.c, so start by defining some heap regions.  This
	 is only done to provide an example as this demo could easily create one
	 large heap region instead of multiple smaller heap regions  */
	prvInitialiseHeap();

	/*FreeRTOS scheduling 1*/
	/*Create the matrix task */
	xTaskCreate((pdTASK_CODE) matrix_task, (signed char * ) "Matrix", 1000,
			NULL, 3, &matrix_handle);
	/*Create the reader task */
	xTaskCreate((pdTASK_CODE) reader_task, (signed char * ) "Reader", 1000,
			NULL, 3, &reader_handle);
	/*Create the matrix queue */
	matrix_queue = xQueueCreate(3, sizeof(struct Message *));

	if (matrix_queue == NULL)
		printf("Failed to create matrix queue.\n");

//This starts the real-time scheduler
	vTaskStartScheduler();
	for (;;)
		;
	return 0;
}

/*-----------------------------------------------------------*/

static void prvInitialiseHeap(void) {
	/* This demo uses heap_5.c, so start by defining some heap regions.  This is
	 only done to provide an example as this demo could easily create one large heap
	 region instead of multiple smaller heap regions - in which case heap_4.c would
	 be the more appropriate choice.  No initialisation is required when heap_4.c is
	 used.  The xHeapRegions structure requires the regions to be defined in order,
	 so this just creates one big array, then populates the structure with offsets
	 into the array - with gaps in between and messy alignment just for test
	 purposes. */
	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	const HeapRegion_t xHeapRegions[] = {
	/* Start address with dummy offsets						Size */
	{ ucHeap + 1, mainREGION_1_SIZE }, { ucHeap + 15 + mainREGION_1_SIZE,
	mainREGION_2_SIZE }, { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,
	mainREGION_3_SIZE }, { NULL, 0 } };

	vPortDefineHeapRegions(xHeapRegions);
}
/*-----------------------------------------------------------*/
