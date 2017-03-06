#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "process.h"

#define NUM_SEM 100
#define QUEUE_LIMIT 20

typedef struct s_semaphore
{
	int value;
	int used;

	int up;
	int down;

	int own;//ID of the process that get the semaphore

	PROCESS* queue[QUEUE_LIMIT];
	
}SEMAPHORE;

#endif