#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

/*
---Commands supported---
 *exit
 *pid
 *ppid
 *cd
 *pwd
 *background processessss
*/ 

/*
  This function takes in the size and command array and determines if (size - 1 == &)
*/
int isBackgroundProcess(char** command, int size); 

/*
 This function takes in string and breaks in up into an array of strings using spaces as a delimter"
*/
char** strtokme(char* command, int* size);

int main(int argc, char** argv) {
   //Vairables
   char* prompt = "308sh>>>";
   char command[1024]; 
   int process, child; 
   char** commandList;
   int keeplooping = 1; 
   int status;   

   if(argc != 1 && argc != 3) 
      printf("It's bad\n"); 
   else if(argc == 3) {
      if(strcmp(argv[1], "-p") != 0) 
         printf("It's real bad\n"); 
      else  
         prompt = argv[2]; 
   }

   while(keeplooping) {
      printf("%s ", prompt);
      fgets(command, 1024, stdin); 
      command[strlen(command)-1]='\0';
      process = getpid(); 
      int size = 0; 
      commandList = strtokme(command, &size);
      // "exit" Closes the shell program
      if(strcmp(commandList[0], "exit") == 0) { 
         if(commandList[1] != NULL) 
            printf("exit() error: Extra parameters\n"); 
         else               
	    keeplooping = 0;
      }

      // "pid" Returns pid of shell
      else if(strcmp(commandList[0], "pid") == 0) {
         if(commandList[1] != NULL) 
            printf("pid() error: Extra parameters\n"); 
         else                
	    printf("[%d] %d\n", process, getpid());
      }

      // "ppid" Returns ppid of shell
      else if(strcmp(commandList[0], "ppid") == 0) { 
         if(commandList[1] != NULL) 
            printf("ppid() error: Extra parameters\n"); 
         else               
            printf("[%d] %d\n", process, getppid());
      }

      // "cd" Changes to the given directory
      else if(strcmp(commandList[0], "cd") == 0) {
         int cdError = chdir(commandList[1]);
         if(cdError != 0) 
            perror("cd() error"); 
      } 

      // "pwd" Shows current working diectory
      else if(strcmp(commandList[0], "pwd") == 0) {
         char buffer[1024]; 
         getcwd(buffer, sizeof(buffer));
         if(buffer == NULL) 
            perror("pwd() error");
         if(commandList[1] != NULL)
            printf("pwd() error: Extra parameters\n"); 
         else
            printf("%s\n", buffer);  
      } else { //Below is all other processes 
         if(isBackgroundProcess(commandList, size)) { //Checks if a background process
            child = fork();
            if(child == 0) {
               child = fork(); 
               if(child == 0) { 
                  execvp(commandList[0], commandList);
                  perror("execvp() error");
                } else {
                waitpid(child, &status, 0); 
                printf("\n[%d] %s Exit %d\n", process, commandList[0], WEXITSTATUS(status));  
              }       
            }
         } else {  
            child = fork(); 
            if(child == 0) {    
               execvp(commandList[0], commandList);
               perror("execvp() error");
            } else { 
               waitpid(child, &status, 0);
            }
         }
         printf("[%d] %s Exit %d\n", process, commandList[0], WEXITSTATUS(status));  
      }
   }
   return 0; 
}

int isBackgroundProcess(char** command, int size) {
   if(strcmp(command[size-1], "&") == 0) {
      command[size-1] = NULL;
      return 1;   
   }
   return 0; 
} 

char** strtokme(char* command, int* size) {
   char** commandArray = malloc(sizeof(char*) * 16); 
   char* space = " ";  
   char* token = strtok(command, space);
   int index = 0;  
   while(token != NULL) {
      commandArray[index] = token; 
      index++;
      token = strtok(NULL, space);
   }
   *size = index;
   return commandArray;  
}   
