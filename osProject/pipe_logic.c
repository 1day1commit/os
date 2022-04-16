#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 25
#define READ 0
#define WRITE 1

int main(void)
{
  pid_t pid;
  //open two pipes, one for each direction
  int toChild[2];
  int toParent[2];

  /* create the pipe */
  if (pipe(toChild) == -1 || pipe(toParent) == -1) {
    fprintf(stderr,"Pipe failed");
    return 1;
  }

  /* now fork a child process */
  pid = fork();

  if (pid < 0) {
    fprintf(stderr, "Fork failed");
    return 1;
  }


  if (pid > 0) {  /* parent process */
    int parentVal = 0;
    // The parent is not going to read from the first pipe.
    // Close the read end of the pipe.
    close(toParent[WRITE]);    // The parent is not going to write to the second pipe.
    // Close the write end of the pipe.
    while(1) {
      parentVal++;
      write(toChild[WRITE],&parentVal,sizeof(parentVal));
      printf("Parent: writes value : %d\n", parentVal);
      
      // If the child closes the write end of the second pipe,
      // break out of the loop.
      if (read(toParent[READ],&parentVal,sizeof(parentVal)) > 0 )
        {
          printf("Parent: reads value : %d\n", parentVal);
        }
      else
        {
          break;
        }
    }
    
    close(toChild[WRITE]);    // Close the write end of the first pipe
    close(toParent[READ]);    // Close the read end of the second pipe
  }
  
  
  
  else { /* child process */
    int childVal = 0;
    close(toChild[WRITE]);    // The child is not going to write to the first pipe.
    // Close the write end of the pipe.
    close(toParent[READ]);    // The child is not going to read from the second pipe.
    // Close the read end of the pipe.
    while(1) {
      
      // If the parent closes the write end of the first pipe,
      // break out of the loop.
      if ( read(toChild[READ],&childVal,sizeof(childVal)) > 0 )
        {
          printf("child: read value : %d\n", childVal);
        }
      else
        {
          break;
        }
      
      childVal++;
      write(toParent[WRITE],&childVal,sizeof(childVal));
      printf("child: write value : %d\n",childVal);
    }
    close(toChild[READ]);     // Close the read end of the first pipe
    close(toParent[WRITE]);   // Close the write end of the second pipe
  }
}