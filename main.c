// Kshitij Pingle
// kpingle@csu.fullerton.edu
// Date: October 17, 2024
// Description: Program to pipe processes in terminal.

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>


void add_character_to_string(char *str, char c) {
	int len = strlen(str);
	str[len] = c;
	str[len + 1] = '\0';
}


// splits string by spaces; adds a NULL into the array after the last word
void split(char *cmd, char *words[], char delimiter) {
	int word_count = 0;
	char *next_char = cmd;
  	char current_word[10000];
  	strcpy(current_word, "");       // Initialize the string to be empty

	// strdup() : A way to allocate space for a string and also copy it. Equivalent to malloc and a strcpy

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
    char*directories[1000];

    split(getenv("PATH"), directories, ':');

    // look in array intil I find the paththing + cmd

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
    if (argc < 3) {
        printf("Incorrect number of arguments\n");
        printf("Usage: %s <output_file> <command> <arguments>\n", argv[0]);
        return 1;
    }

    int output_descriptor = 0;

    // Check if we need to change the output descriptor
    if (strcmp(argv[1], "-")) {
        output_descriptor = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    }

    char absolute_path[1000];
    char *words[1000];

    split(argv[2], words, ' ');

    if (words[0] == NULL) {
        printf("No command given\n");
        return 1;
    }

    if ( ! find_absolute_path(words[0], absolute_path) ) {
        printf("Command not found: '%s'\n", words[0]);
        return 1;
    }

    for (int i = 0; words[i] != NULL; i++) {
        printf("words[%d] = '%s'\n", i, words[i]);
    }

    printf("absolute_path = '%s'\n", absolute_path);    // DEBUGGING TACTIC : Have the ' ' to also be able to see extra spaces


    int child_pid = fork();

    if (child_pid == 0) {

        if (output_descriptor != 0) {
            // Think of dup2 as a reverse assignment statement
                // Think of it like 'STDOUT_FILENO = output_descriptor'
            dup2(output_descriptor, STDOUT_FILENO);
            close(output_descriptor);
        }

        execve(absolute_path, words, NULL);
    }

    wait(NULL);

    return 0;
}