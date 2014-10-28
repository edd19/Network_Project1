#include "timer.h"

Timer_queue* Timer_queue_init()
{
  Timer_queue *queue ;
  queue=(Timer_queue*)malloc(sizeof(Timer_queue));
  return queue;
}

Timer* Timer_init()
{
  Timer  *timer;
  timer =(Timer*)malloc(sizeof(Timer));
  return timer;
}

void add(Timer *newTimer, Timer_queue *queue) //ajoute newtimer a la fin de la queue
{
  if(queue->count == 0){
    queue->first = newTimer;
    queue->last = queue->first;
  }
  else{
    queue->last->next=newTimer;
    queue->last = queue->last->next;
  }
  queue->count = queue->count++;
}

void dequeue(Timer_queue *queue)//retire le premier element de la queue
{
  if(queue->count != 0){
    // free first  d'abord
    free(queue->first);
    queue->first=queue->first->next;
    queue->count = queue->count - 1;
  }
}

 void destroy(Timer_queue *queue)//on ignore pour l'instant
{
  int n=queue->count;
  //Timer *tbd = queue->first;  // timer to be deleted
  //Timer *current = queue->first->next;

      while(n!=0)
	{
	  dequeue(queue);
	  n--;
	}
     free(queue);
}

 void dequeue_n(Timer_queue *queue, int n) //enleve les n premiers elements
{
  int i=0;

  while (i<n)
    {
      dequeue(queue);
      i++;
    }
}

int search(Timer_queue *queue, int seq_num) //renvoie la position du timer possedant ce num de sequence
 {

   Timer *current=queue->first;
   int n=1;
   if(queue->count==0)
     {
       return -1; //liste vide
     }
   else
     {
     while(current->seq_num!=seq_num)
     {
       current=current->next;
       n++;
     }
   }
   return n;
 }
