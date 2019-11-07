#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
using namespace std;

struct termios original_termios;

// Function to exit raw mode
void exitRawMode()
{
	// Resetting the attributes to inital values
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}


// Function to enter raw mode
void enterRawMode()
{
	// Getting the terminal's initial attributes and storing them
	// in a local variable for restoring the terminal attributes
	tcgetattr(STDIN_FILENO, &original_termios);

	struct termios raw = original_termios;
	
	// To disable echo in terminal
	//raw.c_lflag &= ~(ECHO);
	// To disable cannonical mode
	//raw.c_lflag &= ~(ICANON);
	// To disable both echo and cannonical mode
	raw.c_lflag &= ~( ECHO | ICANON );

	// Setting the new attributes
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	// exitRawMode to be called when the program terminates
	atexit(exitRawMode);
}


int main()
{
	// Entering raw mode:
	// Disbled echo and cannonical mode
	enterRawMode();

	char c;
	// Read character
	// Enter 'q' to exit
	while(read(STDIN_FILENO, &c, 1) == 1 && c !='q')
	{
		printf("character entered: %c (%d)\n",c,c);
	}	
	return 0;
}
