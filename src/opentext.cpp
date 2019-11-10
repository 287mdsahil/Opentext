#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "../include/errorhandling"
#include "../include/globalstate"
#include "rawmode.h"
#include "keyboard.h"
#include "screen.h"
using namespace std;

/** init **/
int main(int argc,char *argv[])
{
	// Entering raw mode:
	// Disbled echo and cannonical mode
	enterRawMode();
	if (argc>=2)
	{
		editorConfig ECONFIG(argv[1]);
	}
	else
	{
		editorConfig ECONFIG();
	}

	// Input while loop
	while(1)
	{
		editorRefreshScreen();
		editorProcessKeypress();
	}	
	return 0;
}
