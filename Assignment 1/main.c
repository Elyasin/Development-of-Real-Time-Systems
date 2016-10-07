/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"


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
static void  prvInitialiseHeap( void );


/* The tasks (task1 and task2) to be run according to assignment's
 * instruction notes.
 */
static void task2(){
	while(1){
		printf("This is Task2\n");
		fflush( stdout );
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
static void task1(){
	while(1){
		printf("This is Task1\n");
		fflush( stdout );
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

/* The main function is the starting point for the program. The two tasks
(task1 and task2) are created here. The program runs until it is halted.*/
int main( void )
{
	/* This demo uses heap_5.c, so start by defining some heap regions.  This
	is only done to provide an example as this demo could easily create one
	large heap region instead of multiple smaller heap regions  */
	prvInitialiseHeap();

	/*FreeRTOS scheduling 1*/
	/*Create the tasks task1 and task2 */
	xTaskCreate((pdTASK_CODE)task1, (signed char *)"Task1",1000, NULL, 3, NULL);
	xTaskCreate((pdTASK_CODE)task2, (signed char *)"Task2",100, NULL, 1, NULL);

	//This starts the real-time scheduler
	vTaskStartScheduler();
	for( ;; );
	return 0;
}

/*-----------------------------------------------------------*/

static void  prvInitialiseHeap( void )
{
/* This demo uses heap_5.c, so start by defining some heap regions.  This is
only done to provide an example as this demo could easily create one large heap
region instead of multiple smaller heap regions - in which case heap_4.c would
be the more appropriate choice.  No initialisation is required when heap_4.c is
used.  The xHeapRegions structure requires the regions to be defined in order,
so this just creates one big array, then populates the structure with offsets
into the array - with gaps in between and messy alignment just for test
purposes. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
const HeapRegion_t xHeapRegions[] =
{
	/* Start address with dummy offsets						Size */
	{ ucHeap + 1,											mainREGION_1_SIZE },
	{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
	{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
	{ NULL, 0 }
};

	vPortDefineHeapRegions( xHeapRegions );
}
/*-----------------------------------------------------------*/
