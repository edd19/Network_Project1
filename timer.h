#include <sys/time.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <stdlib.h>


#define TIMER_TIME 100

typedef struct Timer Timer;
struct Timer {
 Timer *next;
  int seq_num;
  struct timeval time;
};


typedef struct {
  Timer *first;
  Timer *last;
  int count;
} Timer_queue;


Timer_queue* Timer_queue_init();
Timer* Timer_init();
void add(Timer *newTimer, Timer_queue *queue);
void dequeue(Timer_queue *queue);
void destroy(Timer_queue *queue);
void dequeue_n(Timer_queue *queue, int n);
int search(Timer_queue *queue, int seq_num);