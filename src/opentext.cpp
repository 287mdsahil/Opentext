/*** includes ***/

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include "../util/errorhandling.h"
#include "rawmode.h"
using namespace std;

/** defines **/
#define CTRL_KEY(k) ((k) & 0x1f)

/** init **/
int main()
{
	// Entering raw mode:
	// Disbled echo and cannonical mode
	enterRawMode();

	char c;
	// Input while loop
	while(1)
	{
		c = '\0';
		//read character
		if(read(STDIN_FILENO, &c, 1) == -1) die("read");
		if(iscntrl(c))
			printf("Character entered: (%d)\n\r",c);
		else
			printf("character entered: %c (%d)\n\r",c,c);
		
		// Quit the program if C^q is entered
		if(c == CTRL_KEY('q'))
			break;
	}	
	return 0;
}
