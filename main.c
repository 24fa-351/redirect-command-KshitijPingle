
// NOTE: The following is not my program, its Roz Michelle's example program
// I'm testing with it and learning from it
// Sorry, i did not manage my prioroties well enough to start this assignment earlier




#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

// int main(int argc, char *argv[]) {
//   int fds[2];                      // an array that will hold two file descriptors
//   pipe(fds);                       // populates fds with two file descriptors
//   pid_t pid = fork();              // create child process that is a clone of the parent
  
//   if (pid == 0) {                  // if pid == 0, then this is the child process
//     dup2(fds[0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
//     close(fds[0]);                 // file descriptor no longer needed in child since stdin is a copy
//     close(fds[1]);                 // file descriptor unused in child
//     char *argv[] = {(char *)"sort", NULL};   // create argument vector
//     if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)
//   } 

//   // if we reach here, we are in parent process
//   close(fds[0]);                 // file descriptor unused in parent
//   const char *words[] = {"pear", "peach", "apple"};
//   // write input to the writable file descriptor so it can be read in from child:
//   size_t numwords = sizeof(words)/sizeof(words[0]);
//   for (size_t i = 0; i < numwords; i++) {
//     dprintf(fds[1], "%s\n", words[i]); 
//   }

//   // send EOF so child can continue (child blocks until all input has been processed):
//   close(fds[1]); 

//   int status;
//   pid_t wpid = waitpid(pid, &status, 0); // wait for child to finish before exiting
//   return wpid == pid && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
// }




// fork/
// if child
// dup2(output_descriptor, STDOUT_FILENO)
// close(output_descriptor)

// 1) break argv[2] into words
// 2) find the absolute path of the command


void add_character_to_string(char *str, char c) {
  int len = strlen(str);
  str[len] = c;
  str[len + 1] = '\0';
}


// splits string by spaces; adds a NULL into the array after the last word
void split(char *cmd, char *words[], char delimiter) {
  // words[0] = strdup(cmd);       // Way to allocate space for a string and also copy it
  //                                   // Equivalent to malloc and a strcpy
  // words[1] = NULL;

  int word_count = 0;

  char *next_char = cmd;
  char current_word[10000];
  strcpy(current_word, "");       // Initialize the string to be empty


  while (*next_char != '\0') {
    if (*next_char == delimiter) {
      words[word_count++] = strdup(current_word);     // Add the word, then zero it out
      strcpy(current_word, "");
    }
    else {
      add_character_to_string(current_word, *next_char);
    }
    ++next_char;
  }

  words[word_count++] = strdup(current_word);
  words[word_count] = NULL;

}


// true = found, false = not found
bool find_absolute_path(char *cmd, char *absolute_path) {



  // strcpy(absolute_path, cmd);

  // char *path = getenv("PATH");
  // printf("PATH = '%s'\n", path);

  char*directories[1000];

  split(getenv("PATH"), directories, ':');

  // look in array intil i find the paththing + cmd

  for (int i = 0; directories[i] != NULL; i++) {
    char path[1000];
    strcpy(path, directories[i]);
    add_character_to_string(path, '/');
    strcat(path, cmd);

    if (access(path, X_OK) == 0) {
      strcpy(absolute_path, path);
      return true;
    }
  }


  return false;
}

int main(int argc, char *argv[]) {
  char absolute_path[1000];
  char *words[1000];

  split(argv[2], words, ' ');

  if (words[0] == NULL) {
    printf("No command given\n");
    return 1;
  }

  if (!find_absolute_path(words[0], absolute_path)) {
    printf("Command not found\n");
    return 1;
  }


  for (int i = 0; words[i] != NULL; i++) {
    printf("words[%d] = '%s'\n", i, words[i]);
  }

  printf("absolute_path = '%s'\n", absolute_path);    // DEBUGGING TACTICE : Have the ' ' to also be able to see extra spaces

  execve(absolute_path, words, NULL);


  // strtok is not thread safe
    // You give it a string, it modifies the string
    // So 2 threads can overwrite the string

  return 0;
}