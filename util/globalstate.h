#include <termios.h>
#include "../include/getwindowsize"
#include "../include/errorhandling"
class editorConfig
{
public:
	int screenrows;
	int screencols;
	struct termios original_termios;

	// Constructor
	editorConfig()
	{
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
	}
};

editorConfig ECONFIG;
