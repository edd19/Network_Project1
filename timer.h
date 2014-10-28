#include <sys/time.h>


typdef struct{
  Linked_timer *next;
  Timer *timer;
} Linked_timer;

typdef struct{
    int seq_num;  // sequence number of the packet corresponding to the timer
    struct timeval t;  // time at which the timer expires
} Timer;


void add_timer(Timer t, Linked_timer head, Linked_timer tail);
void remove_timer(Timer t, Linked_timer head, Linked_timer tail);
void destroy(Linked_timer head, Linked_timer tail);
Timer look_first(Linked_timer head);
Timer look_last(Linked_timer tail);
void remove_n(Linked_timer head, Linked_timer tail, int n);
int search(Linked_timer head, int seq_num);

