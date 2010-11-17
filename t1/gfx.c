#include <stdio.h>

#define HEIGHT 8
#define WIDTH 24

struct pos {
  int x, y;
};
/*
struct pos sensorlocation (int sen) {
  switch (sen) {
    case 0: return ??;
    case 1: return ??;
    ....
    case 79: return ??;
  }
}

struct pos switchlocation (int sw) {

}
*/
char charatlocation (struct pos p, char *trk) {
  return trk[(p.x-1)+(p.y-1)*WIDTH];
}

void putpixel (struct pos p, int colour, char *trk) {
  printf ("[%d;%dH[%dm%c[m", p.y, p.x, colour, charatlocation(p,trk));
}

void drawtrack (char *trk) {
  int x,y;
  for (y = 0; y < HEIGHT; y++) {
    printf ("[%d;1H", y+1);
    for (x = 0; x < WIDTH; x++) {
      printf ("%c", trk[x+y*WIDTH]);
    }
  }
}

int main () {
  printf ("[2J[1;1H[m"); // clear screen and shit

  char trk[]= "---------------------,  "
              "---/  |,-------------,\\ "
              "   |  /      \\|/      \\|"
              "   |  |       |        |"
              "   |  \\      /|\\      /|"
              "---\\  |`-------------\'/ "
              "----\\  `-------------\'  "
              "------------\\---/-------";

  char trk2[] = "aaaaaaaaaaaaaaaaaaaaaaa1"
                "aaaaaaaaaaaaaaaaaaaaaaa2"
"aaaaaaaaaaaaaaaaaaaaaaa3"
"aaaaaaaaaaaaaaaaaaaaaaa4"
"aaaaaaaaaaaaaaaaaaaaaaa5""aaaaaaaaaaaaaaaaaaaaaaa6""aaaaaaaaaaaaaaaaaaaaaaa7""aaaaaaaaaaaaaaaaaaaaaaa8";
  drawtrack (trk);
  struct pos p;
  p.x = 10;
  p.y = 2;
  putpixel (p, 41, trk);
  p.x = 11;
  p.y = 4;
  putpixel (p, 42, trk);
  p.x = 3;
  p.y = 7;
  putpixel (p, 43, trk);
  printf ("[100;100H");
}

