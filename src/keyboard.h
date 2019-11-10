#include "../include/errorhandling"
#include "../include/globalstate"
#include <unistd.h>


/** defines **/
#define CTRL_KEY(k) ((k) & 0x1f)
enum editorKey 
{
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT ,
  ARROW_UP ,
  ARROW_DOWN ,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};


// Function to wait for a keypress and returns the character key pressed
int editorReadKey()
{
	int ascii;
	char c;
	while((ascii = read(STDIN_FILENO, &c, 1)) != 1)
	{
		if(ascii == -1) die("read");
	}
	//incorporating arrow keys as well to move cursor
	if (c == '\x1b') 
	{
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == '[') 
	{
		if (seq[1] >= '0' && seq[1] <= '9') {
			if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
			if (seq[2] == '~') {
			switch (seq[1]) {
				case '1': return HOME_KEY;
				case '3': return DEL_KEY;
            	case '4': return END_KEY;
				case '5': return PAGE_UP;
				case '6': return PAGE_DOWN;
				case '7': return HOME_KEY;
            	case '8': return END_KEY;
			}
			}
      } else {
      switch (seq[1]) 
	  {
        case 'A': return ARROW_UP;
        case 'B': return ARROW_DOWN;
        case 'C': return ARROW_RIGHT;
        case 'D': return ARROW_LEFT;
		case 'H': return HOME_KEY;
        case 'F': return END_KEY;

      }
    }
	}
	else if (seq[0]=='o')
	{
		switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
	}
    return '\x1b';
	} 
  	else
	{
		return c;
	}
}
//method to move cursor using a,s,w,d
void editorMoveCursor(int key) 
{
	erow *row = (ECONFIG.cy >= ECONFIG.numrows) ? NULL : &ECONFIG.row[ECONFIG.cy];
	
  	switch (key) {
    	case ARROW_LEFT:
		if(ECONFIG.cx!=0)
		{
	      		ECONFIG.cx--;
		}
		else if(ECONFIG.cy > 0)
		{
			ECONFIG.cy--;
			ECONFIG.cx = ECONFIG.row[ECONFIG.cy].size;
		}
	      	break;
	    case ARROW_RIGHT:
	      if(row && ECONFIG.cx < row->size)
	      {
	      		ECONFIG.cx++;
	      }
	      else if(ECONFIG.cy < ECONFIG.numrows)
	      {
		      ECONFIG.cy++;
		      ECONFIG.cx = 0;
	      }
	      break;
	    case ARROW_UP:
		  if(ECONFIG.cy!=0){
	      ECONFIG.cy--;}
	      break;
	    case ARROW_DOWN:
		  if(ECONFIG.cy < ECONFIG.numrows){
	      ECONFIG.cy++;}
	      break;
  	}


	// reposition cursor to end of current line if the cursor position exceed the line size
	row = (ECONFIG.cy >= ECONFIG.numrows) ? NULL : &ECONFIG.row[ECONFIG.cy];
	if(row == NULL)
	{
		ECONFIG.cy = 0;
	}
	else if( ECONFIG.cx > row->size)
	{
		ECONFIG.cx = row->size;
	}
}


// Function to process the key presses
void editorProcessKeypress()
{
	int c = editorReadKey();

	switch (c) 
	{
		case '\r':
			//todo
			break;

		case CTRL_KEY('q'):
		       	// C^q to exit
			write(STDOUT_FILENO, "\x1b[2J", 4);
      			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
		case HOME_KEY:
      			ECONFIG.cx = 0;
      			break;
    		case END_KEY:
      			if(ECONFIG.cy < ECONFIG.numrows)
				ECONFIG.cx = ECONFIG.row[ECONFIG.cy].size;
      			break;

		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			//todo
			break;

		case PAGE_UP:
		case PAGE_DOWN:
		{
			if(c==PAGE_UP)
			{
				ECONFIG.cy = ECONFIG.rowoff;
			}
			else if(c==PAGE_DOWN)
			{
				ECONFIG.cy = ECONFIG.rowoff + ECONFIG.screenrows - 1;
				if (ECONFIG.cy > ECONFIG.numrows)
					ECONFIG.cy = ECONFIG.numrows;
			}

			int times = ECONFIG.screenrows;
			while (times--)
			editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
		break;
	
	case ARROW_UP:
	case ARROW_DOWN:
    	case ARROW_LEFT:
    	case ARROW_RIGHT:
			editorMoveCursor(c);
			break;
	
	case CTRL_KEY('l'):
	case '\x1b':
			break;

	default:
			ECONFIG.editorInsertChar(c);
			break;
	}
}
