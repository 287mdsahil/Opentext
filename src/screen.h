#include <unistd.h>
#include <iostream>
#include "../include/globalstate"
#define VERSION "0.0.1"

void editorScroll()
{
	ECONFIG.rx = 0;
	if (ECONFIG.cy < ECONFIG.numrows)
		ECONFIG.rx = ECONFIG.editorRowCxtoRx(&ECONFIG.row[ECONFIG.cy],ECONFIG.cx);

	//vertical scrolling
	if(ECONFIG.cy < ECONFIG.rowoff)
	{
		ECONFIG.rowoff = ECONFIG.cy;
	}
	if(ECONFIG.cy > ECONFIG.rowoff + ECONFIG.screenrows)
	{
		ECONFIG.rowoff = ECONFIG.cy - ECONFIG.screenrows + 1;
	}

	//horizontal scrolling
	if(ECONFIG.rx < ECONFIG.coloff)
	{
		ECONFIG.coloff = ECONFIG.rx;
	}
	if(ECONFIG.rx > ECONFIG.coloff + ECONFIG.screencols)
	{
		ECONFIG.coloff = ECONFIG.rx - ECONFIG.screencols + 1;
	}

}

void editorDrawRows(struct abuf *ab)
{
	int i;
	for(i=0;i<ECONFIG.screenrows;i++)
	{
		int filerow = i + ECONFIG.rowoff;
		if(filerow >=ECONFIG.numrows)
		{
			if (ECONFIG.numrows == 0 && i == ECONFIG.screenrows / 3) 
			{
				//show welcome message
      				char welcome[80];
      				int welcomelen = snprintf(welcome, sizeof(welcome), "Opentext_ %s", VERSION);
      				if (welcomelen > ECONFIG.screencols) welcomelen = ECONFIG.screencols;
				//centering the messagex
				int padding = (ECONFIG.screencols - welcomelen) / 2;
				if (padding) 
				{
					abAppend(ab, "~", 1);
					padding--;
				}
     				 while (padding--) abAppend(ab, " ", 1);
     				 abAppend(ab, welcome, welcomelen);
    			}
			else 
			{
			abAppend(ab,"~",1);
			}
		} 
		else 
		{
			int len = ECONFIG.row[filerow].rsize - ECONFIG.coloff;
			if(len < 0) len = 0;
			if (len > ECONFIG.screencols) len = ECONFIG.screencols;
			abAppend(ab, ECONFIG.row[filerow].render + ECONFIG.coloff, len);
		}

		abAppend(ab,"\x1b[K",3);
		//modifying to print ~ in all rows
		if(i<ECONFIG.screenrows - 1)
		{
			abAppend(ab,"\r\n",2);
		}
	}
}

void editorRefreshScreen()
{
	//scrolling
	editorScroll();

	struct abuf ab = ABUF_INIT;
	//Hide cursor when repainting
	abAppend(&ab,"\x1b[?25l",6);
	
	// Place the curson to the top right corner
	abAppend(&ab,"\x1b[H", 3);

	// Print tildes
	editorDrawRows(&ab);

	//move the cursor
	char buf[32];
  	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", ECONFIG.cy - ECONFIG.rowoff + 1, ECONFIG.rx - ECONFIG.coloff + 1);
  	abAppend(&ab, buf, strlen(buf));

	abAppend(&ab,"\x1b[?25h",6);

	write(STDOUT_FILENO,ab.b,ab.len);
	abFree(&ab);
}
