void randserver() {
  int tid;
  char m[5];
  int seeded = 0;
  int last = 0;
  while (1) {
     if (recieve (&tid, &m, 5)) {
       switch (m[1]) {
        case 's':
           seeded = 1;
           last = (unsigned int)m[1];
           reply (tid, 0, 0);
           break;
         case 'r':
           if (!seeded) return 0;
           last =  ((unsigned long long)last * 279470273UL) % 4294967291UL;
           reply (tid, (char*)(&last), 4);
           break;
       }
    }
  }
}

