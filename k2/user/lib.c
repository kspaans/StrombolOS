int strlen (char *s) {
  int i;
  for (i = 0; s++; i++);
  return i;
}


void strcpy (char *d, char *s) {
  while (*d++ = *s++);
}
int WhoIs (char *name) {
  int tid;
  char msg[5];
  msg[0] = 'w';
  strcpy((msg+1), name);
  Send (1, msg, 5, (char*)&tid, 4);
  return tid; // fix
}

int Register (char *name) {
  char msg[5];
  int res;
  msg[0] = 'r';
  strcpy ((msg+1), name);
  Send (1, msg, 5, 0, 0);
  return 0; // fix
}
