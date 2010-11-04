#ifndef ANSI__INCLUDED
#define ANSI__INCLUDED

// colours
#define BG       40
#define FG       30
#define BRIGHT   60
#define BLACK    0
#define RED      1
#define GREEN    2
#define YELLOW   3
#define BLUE     4
#define MAGENTA  5
#define CYAN     6
#define WHITE    7

// chars
#define CHR_BACKSPACE  8
#define CHR_RETURN     13

// drawing
#define CLEAR()             bwprintf(COM2,"[2J");
#define SETCOLOUR(__ANSI_C) bwprintf(COM2,"[%dm", __ANSI_C)

#define CLEARTOEND()        bwprintf(COM2,"[K")  

// cursor management
#define SHOWCURSOR() bwprintf(COM2, "[?25h")
#define HIDECURSOR() bwprintf(COM2, "[?25l")
#define CURSORPUSH() bwprintf(COM2, "[s")
#define CURSORPOP()  bwprintf(COM2, "[u")

// scrolling
#define ENABLESCROLL(__ANSI_S)          bwprintf(COM2, "[%d;1000r",__ANSI_S)
#define SCROLLUP()                      bwprintf(COM2, "[S")
#define SCROLLDOWN()                    bwprintf(COM2, "[T")
#define ENABLEWRAP()                    bwprintf(COM2, "[7h");
#define DISABLEWRAP()                   bwprintf(COM2, "[7l");

// Cursor positioning
#define CURSORMOVE(__ANSI_R,__ANSI_C)   bwprintf(COM2, "[%d;%dH",__ANSI_R,__ANSI_C)
#define CURSORBACK(__ANSI_n)            bwprintf(COM2, "[%dD",__ANSI_n)
#endif // ANSI__INCLUDED
