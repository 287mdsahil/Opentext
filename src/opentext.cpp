/*** includes ***/

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include "../include/errorhandling"
#include "rawmode.h"
#include "keyboard.h"
#include "screen.h"
using namespace std;

/** init **/
int main()
{
	// Entering raw mode:
	// Disbled echo and cannonical mode
	enterRawMode();

	// Input while loop
	while(1)
	{
		editorRefreshScreen();
		editorProcessKeypress();
	}	
	return 0;
}
