#include <termios.h>
#include "../include/getwindowsize"
#include "../include/errorhandling"
#include <string.h>
#include <vector>
#include <sys/types.h>
#include<stdlib.h>
using namespace std;

typedef struct erow {
  int size;
  char *chars;
} erow;

class editorConfig
{
public:
	int cx,cy;
	// row offset for scrolling
	int rowoff;
	// column offset for scrolling
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	vector<erow> row;
	struct termios original_termios;

	// Constructor
	editorConfig()
	{
		cx=0;
		cy=0;
		rowoff = 0;
		coloff = 0;
		numrows=0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
	}
	editorConfig(char *filename)
	{
		cx=0;
		cy=0;
		rowoff = 0;
		coloff = 0;
		numrows=0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
		editorOpen(filename);
	}

	/** row operations **/

	void editorAppendRow(char *s, size_t len)
	{
		erow newrow;
		newrow.size = len;
		newrow.chars = (char*) malloc(len+1);
		memcpy(newrow.chars, s, len);
		newrow.chars[len] = '\0';
		row.push_back(newrow);
		numrows ++;	
	}	

	void editorOpen(char *filename) 
	{
		FILE *fp = fopen(filename, "r");
		if (!fp) die("fopen");
		char *line = NULL;
		size_t linecap = 0;
		ssize_t linelen;
		while( (linelen = getline(&line, &linecap, fp)) != -1)
		{
			if (linelen != -1)
			{
				while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
				linelen--;
				editorAppendRow(line, linelen);
			}
		}
		free(line);
		fclose(fp);
		
	}
		
};

editorConfig ECONFIG;

struct abuf
{
	char *b;
	int len;

};

#define ABUF_INIT {NULL,0}

void abAppend(struct abuf *ab,const char*s,int len)
{
	char *new_ = (char*)realloc(ab->b, ab->len + len);

  	if (new_ == NULL) return;
  	memcpy(&new_[ab->len], s, len);
  	ab->b = new_;
  	ab->len += len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}
