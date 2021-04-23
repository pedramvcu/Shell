////////////////////////////////////////////////////////////////////////////////
//
//  File           : shellex.c
//  Description    : This is the main source code for for project 3.
//						This program is a simple shell with built in commands.
//						To get more infor simply run the program and type help.
//
//   Author        : Pedram Maleki	
//   Last Modified : 4/23/2020
//					 CMSC-257
//					 Dr. Sonmez

/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void handler(int sig); //This function will handle control C input from keyborad

sigjmp_buf buf;

int main(int argc, char *argv[]) 
{
    char cmdline[MAXLINE]; /* Command line */
    char *shellName="My257sh>"; //The original shell prompt name
    char dash[2]=">";
    if(!sigsetjmp(buf,1)){ //if a signal is detected it is handled by the handler function
      Signal(SIGINT, handler);
    }


    if(argc>=2){ //if user uses the -p option as a command line argument they can change the prompt
      if(strcmp(argv[1],"-p")==0){
        shellName="";
        shellName = argv[2];   
        strcat(shellName,dash);
      }
    
  }
   
    while (1) {
	/* Read */  
	printf(shellName);                   
	Fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */

void handler(int sig){//This function is void and does nothing, basically just ignores the signal

}
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(1);
            }
        }

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0){
		unix_error("waitfg: waitpid error");
      }
      if ( WIFEXITED(status) ) { //This function will check for status and will print the status of child
        int exitstat = WEXITSTATUS(status);
        printf("Child exit status: %d\n", exitstat);
    }
      
	}
  
	else
	    printf("%d %s", pid, cmdline);

    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
	/*pid function will print the process ID of the process*/
  if(!strcmp(argv[0], "pid")){
    int pid=getpid();
    printf("%d\n",pid);
    return 1;
  }
  /*ppid function will print the process ID of the parent process*/
  if(!strcmp(argv[0], "ppid")){
    int pid=getppid();
    printf("%d\n",pid);
    return 1;
  }
  /*cd function has multiple option if invoked*/
  if(!strcmp(argv[0], "cd")){
    /*if the user input is only cd with no other arguments
	/*the current working directory is printed*/
    if(argv[1]==NULL){
              
      char cwd[MAXLINE];
      getcwd(cwd,sizeof(cwd));
      printf("%s\n",cwd);
      return 1;

    }
	/*If user inputs ".." just like in linux the shell will take you
	/*to the parent directory*/
    if(strcmp(argv[1], "..")==0){
      chdir("..");  
      return 1;    
            
    }
	/*In other cases the cd command will take you to the specified directory*/
    else{
      char changeDir[MAXLINE];
      char dash[2]="/";
      getcwd(changeDir,sizeof(changeDir));
      strcat(changeDir,dash);
      strcat(changeDir,argv[1]);
      chdir(changeDir);
	  return 1;
    }

    return 1;
  }
  
	/*If user input is exit, the system will exit by sendig
	/*a kill signal to itself*/
	if (!strcmp(argv[0], "exit")){/* quit command */
      raise(SIGKILL);
    } 
	/*The help function will give explanation of the shell commands*/ 
    if (!strcmp(argv[0], "help")){
      printf("Pedram Maleki\n");
      printf("This shell has the following built in commands: \n");
      printf("<exit> will exit the shell\n");
      printf("<pid> will print the process ID of this process\n");
      printf("<ppid> will print the process ID of the parent process\n");
      printf("<cd> will prinf the current working directory\n");
      printf("<cd ..> will take you to the parent directory\n");
      printf("<cd dirName> will take you to directory named dirName\n");
      printf("You can also use the commands build into Linux, for more info use \"man term\" to look up info on term\n");

      return 1;
    }    /* Ignore singleton & */
	
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */