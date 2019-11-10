#include <termios.h>
#include "../include/getwindowsize"
#include "../include/errorhandling"
#include <string.h>
#include <vector>
#include <sys/types.h>
#include<stdlib.h>

#define TAB_SIZE 8

using namespace std;

typedef struct erow {
  int size;
  char *chars;
  char *render;
  int rsize;
} erow;

class editorConfig
{
public:
	int cx,cy;
	int rx;
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
	editorConfig(char *filename = NULL)
	{
		cx=0;
		rx=0;
		cy=0;
		rowoff = 0;
		coloff = 0;
		numrows=0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
		if(filename != NULL)
			editorOpen(filename);
	}

	/** row operations **/

	int editorRowCxtoRx(erow *row, int cx)
	{
		int rx = 0;
		for(int i =0;i<cx;i++)
		{
			if(row->chars[i] == '\t')
				rx += (TAB_SIZE - 1) - (rx % TAB_SIZE);
			rx++;
		}
		return rx;	
	}

	// function to decide the rendering of the line in the terminal
	void editorUpdateRow(erow *row)
	{
		int ntabs = 0;
		for(int i=0;i<row->size;i++)
			if(row->chars[i] == '\t')
				ntabs++;

		free(row->render);
		row->render = (char*) malloc(sizeof(char)*(row->size + (TAB_SIZE-1)*ntabs +  1));

		int x=0, rx=0;
		for(x = 0; x<row->size; x++)
		{
			if(row->chars[x] == '\t')
			{
				row->render[rx++] = ' ';
				while(rx % TAB_SIZE != 0)
					row->render[rx++] = ' ';
			}
			else
				row->render[rx++] = row->chars[x];
		}

		row->render[rx] = '\0';
		row->rsize = rx;
	}

	void editorAppendRow(char *s, size_t len)
	{
		erow newrow;
		newrow.size = len;
		newrow.chars = (char*) malloc(len+1);
		memcpy(newrow.chars, s, len);
		newrow.chars[len] = '\0';
		newrow.rsize = 0;
		newrow.render = NULL;
		editorUpdateRow(&newrow);

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
