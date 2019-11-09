#include <termios.h>
#include <unistd.h>
#include "../include/errorhandling"
#include "../include/globalstate"
/*** data ***/


/*** terminal ***/

// Function to exit raw mode
void exitRawMode()
{
	// Resetting the attributes to inital values
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &ECONFIG.original_termios) == -1) 
		die("tcsetattr");
}


// Function to enter raw mode
void enterRawMode()
{
	// Getting the terminal's initial attributes and storing them
	// in a local variable for restoring the terminal attributes
	if(tcgetattr(STDIN_FILENO, &ECONFIG.original_termios) == -1) die("tcsetattr");

	struct termios raw = ECONFIG.original_termios;
	
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

	// Sets character size to 8 bits per byte
	raw.c_cflag &= ~( CS8 );

	// ~OPOST to disable processing \n to \n\r
	raw.c_oflag &= ~( OPOST );

	// Set the minimum no of character to read() before returning to zero
	raw.c_cc[VMIN] = 0;

	// Maximum amount of time to wait for input before it returns
	// (unit is 1/10th of a second)
	raw.c_cc[VTIME] = 1;
	
	
	// Setting the new attributes
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");

	// exitRawMode to be called when the program terminates
	atexit(exitRawMode);
}


