#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUDGET 999999

int main(){
  printf("%ld\n", (long)getpid());
  for(;;);
  return 0;
}

