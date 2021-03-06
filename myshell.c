/*
 * This code implements a simple shell program
 * It supports the internal shell command "exit",
 * backgrounding processes with "&", input redirection
 * with "<" and output redirection with ">".
 * However, this is not complete.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>


extern char **getaline();

/*
 * Handle exit signals from child processes
 */
void sig_handler(int signal) {
  int status;
  int result = wait(&status);
  printf("signal: %d\n", signal);
  //kill(getpid(), SIGCHLD);
  printf("Wait returned %d\n", result);
}

/*
 * The main shell function
 */
main() {
  int i;
  int j;
  char **args;
  char **beforeArgs;
  char **afterArgs;
  int result;
  int block;
  int output;
  int input;
  int and;
  char *output_filename;
  char *input_filename;
  int isPipe;
  int totalPipes;
  int doReturn;
  int wasPiped;

  int fdc[2];
  int fdp[2];
  int fd_in = 0;
  
 beforeArgs = malloc(50*sizeof(char*));
//  for(int k=0; k<50; k++) {
  //  beforeArgs[k]=malloc(50*sizeof(char*));
  //  beforeArgs[k] = NULL;
//  }

  afterArgs = malloc(50*sizeof(char*));
//  for(int k=0; k<50; k++) {
  //  afterArgs[k]=malloc(50*sizeof(char*));
  //  afterArgs[k] = NULL;
//  }

  // Set up the signal handler
  sigset(SIGCHLD, SIG_IGN);

  

  //printf("%d\n",tcsetpgrp());

  wasPiped = 0;
  setpgid(0, 0);

  // Loop forever
  while(1) {

    // Print out the prompt and get the input
    printf("->");
    args = getaline();


    //save file descriptors



      
    do {


      pipe(fdp);


      //check for a pipe
      isPipe = (pipeing(args) == 1);

      // No input, continue
      if(args[0] == NULL)
        continue;

      // Check for internal shell commands, such as exit
      if(internal_command(args))
        continue;

      // Check for an ampersand
      block = (ampersand(args) == 0);

      // Check for a pipe
      //pipe = (pipeing(args, beforeArgs, afterArgs) == 0);

      // Check for redirected input
      input = redirect_input(args, &input_filename);

      switch(input) {
      case -1:
        printf("Syntax error!\n");
        continue;
        break;
      case 0:
        break;
      case 1:
        printf("Redirecting input from: %s\n", input_filename);
        break;
      }

      // Check for redirected output
      output = redirect_output(args, &output_filename);

      switch(output) {
      case -1:
        printf("Syntax error!\n");
        continue;
        break;
      case 0:
        break;
      case 1:
        printf("Redirecting output to: %s\n", output_filename);
        break;
      case 2:
        printf("Appending output to: %s\n", output_filename);
      }

      // Do the command
      do_command(args, block,
	         input, input_filename,
	         output, output_filename, isPipe, wasPiped, &fdp, &fdc);

      if(isPipe == 1) {
        shiftPipe(args);
      }
      wasPiped = isPipe;
      
    } while(isPipe == 1);
    
  }
}


/*
 * Check for ampersand as the last argument
 */
int ampersand(char **args) {
  int i;

  for(i = 1; args[i] != NULL; i++) ;

  if(args[i-1][0] == '&') {
    free(args[i-1]);
    args[i-1] = NULL;
    return 1;
  } else {
    return 0;
  }

  return 0;
}

/*
 * Check for pipeing
 *
 */


int shiftPipe(char **args) {

  int i, k;
  int j = 0;
  for(k = 0; args[k] != NULL; k++) {
	free(args[k]);
  }

  for(i = k; args[i] != NULL || args[i+1] != NULL; i++) {
	args[j] = args[i+1];
	args[i+1] = NULL;
	j++;
  }
  args[j] = args[i+1];
  return 0;
}



int pipeing(char **args) {
  int i;
  int j;
  int k;

  for(i = 0; args[i] != NULL; i++) {

    // Look for the pipe
    
    if(args[i][0] == '|' || args[i][0] == ';') {

      args[i] = NULL;

      j = j + 1;
      k = 0;

      return 1;
    }
  }

  return 0;
}
/*
 * Check for internal commands
 * Returns true if there is more to do, false otherwise
 */
int internal_command(char **args) {
  if(strcmp(args[0], "exit") == 0) {
    exit(0);
  }

  return 0;
}

/*
 * Do the command
 */
int do_command(char **args, int block,
	       int input, char *input_filename,
	       int output, char *output_filename, int isPipe, int wasPiped, int** fdp, int** fdc) {

  int result;
  pid_t child_id;
  int status;
  
 /* if(isPipe == 1 || wasPiped == 1) {
    
      dup2(*fdp[0],0);
      dup2(*fdp[1], 1);

    if(wasPiped == 0 && isPipe == 1) {
      close(*fdp[0]);
    }
    if(isPipe == 1) {
      close(*fdp[0]);
    }
  }
*/
  // Fork the child process
  child_id = fork();
  
  // Check for errors in fork()
  switch(child_id) {
  case EAGAIN:
    perror("Error EAGAIN: ");
    return;
  case ENOMEM:
    perror("Error ENOMEM: ");
    return;
  }

  if(!block) {
    setpgid(child_id, 0);
  }

  //if this is the child
  if(child_id == 0) {
    if(!block) {
      setpgid(0, 0);
    }
   

/*
    if(isPipe == 1 || wasPiped == 1) {
      pipe(*fdc);
      dup2(*fdc[1], 1);
      dup2(*fdc[0], 0);
      close(*fdc[1]);
      printf("child\n");
    }
*/
	//printf("fd_in = %d and set to 0\n", *fd_in);
	//printf("fd_in = %d\n", *fd_in);	


  
    // Set up redirection in the child process
    if(input)
      freopen(input_filename, "r", stdin);
    if(output == 2) {
      printf("printing to file %s\n", output_filename);
      freopen(output_filename, "a+", stdout);
    }
    if(output == 1) {
      freopen(output_filename, "w+", stdout);
    }

    // Execute the command
    result = execvp(args[0], args);
    //close(*fdc[0]);
    //close(*fdc[1]);


    exit(-1);
  }

  if(isPipe == 1 || wasPiped == 1) {
  
    wait(NULL);
  /*  close(*fdc[0]);
    fdp[0] = fdp[1];


    close(*fdp[0]);
    close(*fdp[1]);
  */
	//printf("fd_in = %d\n", *fd_in);
  } else {
  // Wait for the child process to complete, if necessary
    if(block) {
    //printf("Waiting for child, pid = %d\n", child_id);
      result = waitpid(child_id, &status, WUNTRACED);

    } else {
  	result = waitpid(child_id, &status, WNOHANG);
    }

  }


}

/*
 * Check for input redirection
 */
int redirect_input(char **args, char **input_filename) {
  int i;
  int j;

  for(i = 0; args[i] != NULL; i++) {

    // Look for the <
    if(args[i][0] == '<') {
      free(args[i]);

      // Read the filename
      if(args[i+1] != NULL) {
	*input_filename = args[i+1];
      } else {
	return -1;
      }

      // Adjust the rest of the arguments in the array
      for(j = i; args[j-1] != NULL; j++) {
	args[j] = args[j+2];
      }

      return 1;
    }
  }

  return 0;
}

/*
 * Check for output redirection
 */
int redirect_output(char **args, char **output_filename) {
  int i;
  int j;
  int shouldAppend = 0;

  for(i = 0; args[i] != NULL; i++) {

    // Look for the >
    if(args[i][0] == '>') {
      free(args[i]);

      // Look for the >>
      if(args[i+1][0] == '>') {
	shouldAppend = 1;
	free(args[i+1]);
      }


      // Get the filename
      if(args[i+1+shouldAppend] != NULL) {
		*output_filename = args[i+1+shouldAppend];
      } else {
	return -1;
      }

      // Adjust the rest of the arguments in the array

      for(j = i; args[j-1] != NULL; j++) {
		args[j] = args[j+2+shouldAppend];
      }
      if(shouldAppend == 1) {
		return 2;
      } else {
        return 1;
      }
    }
  }

  return 0;
}
