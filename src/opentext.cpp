#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

//to make more portable
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "../include/errorhandling"
#include "../include/globalstate"
#include "rawmode.h"
#include "keyboard.h"
#include "screen.h"
using namespace std;

/** init **/
int main(int argc,char *argv[])
{
	if (argc>=2)
	{
		ECONFIG = editorConfig(argv[1]);
	}
	else
	{
		ECONFIG = editorConfig();
	}

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
