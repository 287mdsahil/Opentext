#include "../include/errorhandling"
#include <unistd.h>

/** defines **/
#define CTRL_KEY(k) ((k) & 0x1f)


// Function to wait for a keypress and returns the character key pressed
char editorReadKey()
{
	int ascii;
	char c;
	while((ascii = read(STDIN_FILENO, &c, 1)) != 1)
	{
		if(ascii == -1) die("read");
	}
	return c;
}


// Function to process the key presses
void editorProcessKeypress()
{
	char c = editorReadKey();

	switch (c) 
	{
		case CTRL_KEY('q'):
		       	// C^q to exit
			write(STDOUT_FILENO, "\x1b[2J", 4);
      			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}
