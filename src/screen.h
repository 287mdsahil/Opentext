#include <unistd.h>
#include <iostream>
#include "../include/globalstate"

void editorDrawRows()
{
	for(int i=0;i<ECONFIG.screenrows;i++)
		write(STDOUT_FILENO, "~\r\n",3);
}

void editorRefreshScreen()
{
	// Clear the entire screen
	write(STDOUT_FILENO, "\x1b[2J", 4);
	// Place the curson to the top right corner
	write(STDOUT_FILENO, "\x1b[H", 3);

	// Print tildes
	editorDrawRows();

	// Place the curson to the top right corner
	write(STDOUT_FILENO, "\x1b[H", 3);
}
