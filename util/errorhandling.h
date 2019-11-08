#include <stdlib.h>
#include <stdio.h>

// Function for error handling
// Displays the error and exits the program
void die(const char *c)
{
	perror(c);
	exit(1);
}
