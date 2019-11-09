#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Function for error handling
// Displays the error and exits the program
void die(const char *c)
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
      	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(c);
	exit(1);
}
