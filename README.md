# Opentext
Terminal based text editor

## Key bindings
exit: Ctrl+q  
save: Ctrl+s  
Navigation: Arrowkeys, 
            Page up, Page down,
            home, end  
            
            
#### If current file is not saved , have to press ctrl+q 3 times to exit
#### Copy and Paste shortcuts are terminal specific
#### Status line shows the name of the current file and the line number the cursor is currently on.

## Implementation
The main data structure used here is a list, where each elements denotes a line in the document.  
When a file is loaded, the data is read and stored in the list (buffer). The screen refreshes in a while loop, each time reloading the buffer. When some text is edited on the screen , the buffer is modified. Pressing ctrl+s updates the file with the contents of the buffer

## How to run:
To run the program, open the termial and type in the following commands  
`$ make`  
`$ ./Opentext <filename>`  
File name is optional field
