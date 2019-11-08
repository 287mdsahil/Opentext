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
	
	// ~ECHO to disable echo
	// ~ICANON to disable cannonical mode
	// ~ISIG to disable C^c to quit and C^z to suspend
	// ~IEXTEN to disable C^v
	// ~BRKINT to disable break conditions from causing SIGINT signal
	// ~ISTRIP disables parity checking
	raw.c_lflag &= ~( BRKINT | ISTRIP | ECHO | ICANON | ISIG );
	
	// ~IXON to disable C^s (suspends output) and C^q (resumes output)
	// ~ICRNL to disable \r translation to \n
	raw.c_iflag &= ~( IXON | ICRNL );

	raw.c_cflag &= ~( CS8 );

	// ~OPOST to disable processing \n to \n\r
	raw.c_oflag &= ~( OPOST );
	
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
		printf("character entered: %c (%d)\n\r",c,c);
	}	
	return 0;
}
