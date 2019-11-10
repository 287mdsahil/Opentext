#include <termios.h>
#include "../include/getwindowsize"
#include "../include/errorhandling"
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <fcntl.h>
#include <stdarg.h>

#define TAB_SIZE 8
#define QUIT_TIMES 3
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
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	int dirty;
	struct termios original_termios;

	// Constructor
	editorConfig(char *fname = NULL)
	{
		cx=0;
		rx=0;
		cy=0;
		rowoff = 0;
		coloff = 0;
		numrows=0;
		filename = NULL;
		statusmsg[0] = '\0';
		statusmsg_time = 0;
		dirty = 0;
		if( getwindowsize(&screenrows,&screencols) == -1) die("getwindowsize");
		
		// making room for status bar
		screenrows -= 2;

		if(fname != NULL)
			editorOpen(fname);
	}


	void editorSetStatusMessage(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(statusmsg, sizeof(statusmsg), fmt, ap);
		va_end(ap);
		statusmsg_time = time(NULL);
	}

	/** row operations -----------------------------------------------------------------**/

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
		dirty++;	
	}


	void editorInsertRow(int at, char *s, size_t len)
	{
		erow newrow;
		newrow.size = len;
		newrow.chars = (char*) malloc(len+1);
		memcpy(newrow.chars, s, len);
		newrow.chars[len] = '\0';
		newrow.rsize = 0;
		newrow.render = NULL;
		editorUpdateRow(&newrow);

		row.insert(row.begin() + at,newrow);
		numrows ++;
		dirty++;	
	}


	void editorRowInsertChar(erow *row, int at, int c)
	{
		if(at < 0 || at > row->size)
			at = row->size;
		row->chars = (char*) realloc(row->chars, row->size+2);
		memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
		row->size++;
		row->chars[at] = c;
		editorUpdateRow(row);
		dirty++;
	}

	void editorRowDelChar(erow *row, int at)
	{
		if(at < 0 || at >= row->size)
			return;
		memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
		row->size--;
		editorUpdateRow(row);
		dirty++;
	}

	void editorFreeRow(erow *row) 
	{
  		free(row->render);
		free(row->chars);
	}

	void editorDelRow(int at) 
	{
  		if (at < 0 || at >= numrows) return;
  		editorFreeRow(&row[at]);
  		row.erase(row.begin() + at);
  		numrows--;
  		dirty++;
	}

	void editorRowAppendString(erow *row, char *s, size_t len)
	{
  		row->chars = (char*) realloc(row->chars, row->size + len + 1);
  		memcpy(&row->chars[row->size], s, len);
  		row->size += len;
  		row->chars[row->size] = '\0';
  		editorUpdateRow(row);
  		dirty++;
	}


	/** editor operation ------------------------------------------------------------------**/

	void editorInsertChar(int c)
	{
		if(cy == numrows)
		{
			char s[80];
			s[0] = '\0';
			editorInsertRow(numrows,s,0);
		}
		editorRowInsertChar(&row[cy], cx, c);
		cx++;
	}

	void editorDelChar()
	{
		if(cy == numrows)
			return;

		if(cx == 0 && cy == 0)
			return;
		
		if(cx>0)
		{
			editorRowDelChar(&row[cy],cx - 1);
			cx--;
		}
		else
		{
			cx = row[cy - 1].size;
			editorRowAppendString(&row[cy-1], row[cy].chars, row[cy].size);
			editorDelRow(cy);
			cy--;
		}
	}

	void editorInsertNewline() {
  		if (cx == 0) 
		{
			char s[2];
			s[0] = '\0';
    			editorInsertRow(cy, s, 0);
  		} 
		else 
		{
   		 	erow *nrow = &row[cy];
    			editorInsertRow(cy + 1, &nrow->chars[cx], nrow->size - cx);
    			nrow = &row[cy];
    			nrow->size = cx;
    			nrow->chars[nrow->size] = '\0';
    			editorUpdateRow(nrow);
  		}
  		cy++;
  		cx = 0;
	}

	//----------------------------------------------------------------------------------------
	

	void editorOpen(char *fname) 
	{
		free(filename);
		filename = strdup(fname);
		FILE *fp = fopen(fname, "r");
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
		dirty = 0;
		
	}


	/** file I/0 ----------------------------------------------------------------------------- **/

	char* editorRowsToString(int *buflen)
	{
		int totlen = 0;
		int j;
		for(j = 0;j<numrows;j++)
			totlen += row[j].size + 1;
		*buflen = totlen;

		char *buf = (char*) malloc (totlen);
		char *p = buf;

		for(j = 0; j< numrows; j++)
		{
			memcpy(p, row[j].chars, row[j].size);
			p += row[j].size;
			*p = '\n';
			p++;
		}

		return buf;
	}

	void editorSave()
	{
		if(filename == NULL)
			return;
		
		int len;
		char *buf = editorRowsToString(&len);

		int fd = open(filename, O_RDWR | O_CREAT, 0644);
		if(fd != -1)
		{
			if(ftruncate(fd, len) != -1)
			{
				if(write(fd, buf, len) == len)
				{
					close(fd);
					free(buf);
					dirty = 0;
					editorSetStatusMessage("%d bytes written to disk", len);
					return;
				}
			}
			close(fd);
		}
		free(buf);
		editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
	}	

	//-------------------------------------------------------------------------------------------
		
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
