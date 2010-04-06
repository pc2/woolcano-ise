#include <stdlib.h>
#include <stdio.h>

const int SIZE = 33;

void loop(int (&a)[SIZE])
{
  // loop without dependency
  int i;
  for(i=0; i<SIZE; i++) 
    a[i] = i;
}

int main(int argc) 
{
  int a[SIZE];
  loop(a);
  return a[argc];  // or: volatile int a[SIZE]
}



//    a[i] = (double)rand() / ((double)(RAND_MAX) + 1.0);
