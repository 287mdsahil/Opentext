#include <termios.h>
#include "../include/getwindowsize"
#include "../include/errorhandling"
#include <string.h>
#include <sys/types.h>
#include<stdlib.h>

typedef struct erow {
  int size;
  char *chars;
} erow;

class editorConfig
{
public:
	int cx,cy;
	int screenrows;
	int screencols;
	int numrows;
	erow row;
	struct termios original_termios;

	// Constructor
	editorConfig()
	{
		cx=0;
		cy=0;
		numrows=0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
	}
	editorConfig(char *filename)
	{
		cx=0;
		cy=0;
		numrows=0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
		editorOpen(filename);
	}
	void editorOpen(char *filename) 
	{
		FILE *fp = fopen(filename, "r");
		if (!fp) die("fopen");
		char *line = NULL;
		size_t linecap = 0;
		ssize_t linelen;
		linelen = getline(&line, &linecap, fp);
		if (linelen != -1) {
			while (linelen > 0 && (line[linelen - 1] == '\n' ||
								line[linelen - 1] == '\r'))
			linelen--;
			row.size = linelen;
			row.chars = (char*)malloc(linelen + 1);
			memcpy(row.chars, line, linelen);
			row.chars[linelen] = '\0';
			numrows = 1;
		}
		free(line);
		fclose(fp);
		
	}
		
};

//editorConfig ECONFIG;

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
