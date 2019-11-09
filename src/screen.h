#include <unistd.h>
#include <iostream>

void editorRefreshScreen()
{
	// Clear the entire screen
	write(STDOUT_FILENO, "\x1b[2J", 4);

	// Place the curson to the top right corner
	write(STDOUT_FILENO, "\x1b[H", 3);
}
