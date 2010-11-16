#ifndef LIB__INCLUDED
#define LIB__INCLUDED

struct heap {
  int size;
  int bottom;
  /* A heap of pointers */
  int **data;
};

int strlen (char *s);
void strcpy (char *d, char *s);
void sens_id_to_name(int id, char *str);
void UseBits(unsigned char *bits, int numbits);
void PrintBits(unsigned char *bits, int len);
void heap_push(int *val, struct heap *h);
int *heap_pop(struct heap *h);

#endif // LIB__INCLUDED
