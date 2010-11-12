# turn data file into C structures -- actually, I inserted the data into this
# file --> to run it, rename "lines = track_.split()" appropriately and possibly
# make sure that the variables are prepended with a or b so that there are no
# conflicts

tracka = """track 72
0 A1 sensor 0 100 10 2
  51 ahead 218
  66 behind 500
1 A3 sensor 1 135 85 2
  53 ahead 62
  15 behind 440
2 A5 sensor 2 160 285 2
  42 ahead 216
  12 behind 649
3 A7 sensor 3 100 265 2
  13 ahead 473
  41 behind 211
4 A9 sensor 4 70 245 2
  11 ahead 289
  40 behind 210
5 A11 sensor 5 40 225 2
  40 ahead 410
  69 behind 60
6 A13 sensor 6 70 30 2
  43 ahead 215
  65 behind 320
7 A15 sensor 7 40 50 2
  67 ahead 145
  43 behind 394
8 B1 sensor 8 290 235 2
  30 ahead 398
  55 behind 225
9 B3 sensor 9 290 215 2
  16 ahead 230
  55 behind 220
10 B5 sensor 10 290 35 2
  25 ahead 405
  52 behind 214
11 B7 sensor 11 20 245 2
  4 ahead 289
  68 behind 60
12 B9 sensor 12 20 285 2
  2 ahead 649
  70 behind 70
13 B11 sensor 13 20 265 2
  3 ahead 473
  71 behind 70
14 B13 sensor 14 365 180 2
  58 ahead 224
  31 behind 202
15 B15 sensor 15 135 185 2
  1 ahead 440
  54 behind 70
16 C1 sensor 16 325 180 2
  9 ahead 230
  61 behind 228
17 C3 sensor 17 420 285 2
  64 ahead 410
  44 behind 226
18 C5 sensor 18 220 260 2
  45 ahead 79
  54 behind 411
19 C7 sensor 19 265 285 2
  57 ahead 215
  42 behind 145
20 C9 sensor 20 200 235 2
  54 ahead 305
  55 behind 146
21 C11 sensor 21 200 35 2
  52 ahead 140
  53 behind 314
22 C13 sensor 22 245 11 2
  35 ahead 877
  50 behind 60
23 C15 sensor 23 294 260 2
  29 ahead 405
  45 behind 223
24 D1 sensor 24 365 90 2
  59 ahead 227
  33 behind 200
25 D3 sensor 25 400 35 2
  49 ahead 223
  10 behind 405
26 D5 sensor 26 550 70 2
  34 ahead 375
  48 behind 213
27 D7 sensor 27 550 42 2
  48 ahead 290
  35 behind 375
28 D9 sensor 28 550 225 2
  37 ahead 361
  47 behind 291
29 D11 sensor 29 400 260 2
  23 ahead 405
  46 behind 220
30 D13 sensor 30 400 235 2
  8 ahead 398
  56 behind 225
31 D15 sensor 31 400 215 2
  14 ahead 202
  56 behind 230
32 E1 sensor 32 325 90 2
  60 ahead 222
  39 behind 203
33 E3 sensor 33 400 50 2
  24 ahead 200
  49 behind 220
34 E5 sensor 34 470 35 2
  26 ahead 375
  49 behind 74
35 E7 sensor 35 470 11 2
  27 ahead 375
  22 behind 877
36 E9 sensor 36 550 200 2
  47 ahead 215
  38 behind 375
37 E11 sensor 37 470 260 2
  28 ahead 361
  46 behind 65
38 E13 sensor 38 470 235 2
  56 ahead 62
  36 behind 375
39 E15 sensor 39 290 50 2
  52 ahead 230
  32 behind 203
40 SW1 switch 1 140 255 curved 3
  5 straight 410
  4 curved 210
  41 behind 191
41 SW2 switch 2 180 275 curved 3
  40 straight 191
  3 curved 211
  42 behind 182
42 SW3 switch 3 230 285 curved 3
  2 straight 216
  41 curved 182
  19 behind 145
43 SW4 switch 4 140 20 straight 3
  7 straight 394
  6 curved 215
  51 behind 185
44 SW5 switch 5 375 285 curved 3
  17 straight 226
  46 curved 334
  57 behind 187
45 SW6 switch 6 245 260 straight 3
  23 straight 223
  57 curved 338
  18 behind 79
46 SW7 switch 7 447 260 straight 3
  29 straight 220
  44 curved 334
  37 behind 65
47 SW8 switch 8 570 166 curved 3
  28 straight 291
  36 curved 215
  48 behind 193
48 SW9 switch 9 570 111 curved 3
  27 straight 290
  26 curved 213
  47 behind 193
49 SW10 switch 10 445 35 straight 3
  25 straight 223
  33 curved 220
  34 behind 74
50 SW11 switch 11 220 11 curved 3
  51 straight 190
  53 curved 445
  22 behind 60
51 SW12 switch 12 180 11 curved 3
  0 straight 218
  43 curved 185
  50 behind 190
52 SW13 switch 13 245 35 straight 3
  10 straight 214
  39 curved 230
  21 behind 140
53 SW14 switch 14 150 70 curved 3
  50 straight 445
  21 curved 314
  1 behind 62
54 SW15 switch 15 150 200 curved 3
  18 straight 411
  20 curved 305
  15 behind 70
55 SW16 switch 16 245 235 straight 3
  8 straight 225
  9 curved 220
  20 behind 146
56 SW17 switch 17 445 235 straight 3
  30 straight 225
  31 curved 230
  38 behind 62
57 SW18 switch 18 315 285 curved 3
  19 straight 215
  45 curved 338
  44 behind 187
61 SW99 switch 0x99 345 160 curved 3
  62 straight 230
  16 curved 228
  58 behind 24
58 SW9A switch 0x9a 345 150 straight 3
  61 straight 24
  14 curved 224
  60 behind 32
59 SW9B switch 0x9b 345 110 straight 3
  63 straight 247
  24 curved 227
  60 behind 24
60 SW9C switch 0x9c 345 120 curved 3
  59 straight 24
  32 curved 222
  58 behind 32
62 DE1 stop 0 345 200 1
  61 ahead 230
63 DE2 stop 1 345 70 1
  59 ahead 247
64 DE3 stop 2 500 285 1
  17 ahead 410
65 DE4 stop 3 10 30 1
  6 ahead 320
66 DE5 stop 4 10 10 1
  0 ahead 500
67 DE6 stop 5 20 50 1
  7 ahead 145
68 DE7 stop 6 10 245 1
  11 ahead 60
69 DE8 stop 7 20 225 1
  5 ahead 60
70 DE9 stop 8 10 285 1
  12 ahead 70
71 DE10 stop 9 10 265 1
  13 ahead 70"""

trackb = """track 70
0 A1 sensor 0 100 10 2
  51 ahead 218
  66 behind 500
1 A3 sensor 1 135 85 2
  53 ahead 62
  15 behind 440
2 A5 sensor 2 160 285 2
  42 ahead 216
  12 behind 649
3 A7 sensor 3 100 265 2
  13 ahead 473
  41 behind 211
4 A9 sensor 4 70 245 2
  11 ahead 289
  40 behind 210
5 A11 sensor 5 60 200 2
  40 ahead 265
  7 behind 774
6 A13 sensor 6 70 30 2
  43 ahead 215
  65 behind 320
7 A15 sensor 7 60 70 2
  5 ahead 774
  43 behind 260
8 B1 sensor 8 290 235 2
  30 ahead 398
  55 behind 225
9 B3 sensor 9 290 215 2
  16 ahead 230
  55 behind 220
10 B5 sensor 10 290 35 2
  25 ahead 405
  52 behind 214
11 B7 sensor 11 20 245 2
  4 ahead 289
  67 behind 60
12 B9 sensor 12 20 285 2
  2 ahead 649
  68 behind 70
13 B11 sensor 13 20 265 2
  3 ahead 473
  69 behind 70
14 B13 sensor 14 365 180 2
  58 ahead 224
  31 behind 202
15 B15 sensor 15 135 185 2
  1 ahead 440
  54 behind 70
16 C1 sensor 16 325 180 2
  9 ahead 230
  61 behind 228
17 C3 sensor 17 420 285 2
  64 ahead 410
  44 behind 226
18 C5 sensor 18 220 260 2
  45 ahead 79
  54 behind 411
19 C7 sensor 19 265 285 2
  57 ahead 215
  42 behind 145
20 C9 sensor 20 200 235 2
  54 ahead 305
  55 behind 146
21 C11 sensor 21 200 35 2
  52 ahead 140
  53 behind 314
22 C13 sensor 22 245 11 2
  35 ahead 785
  50 behind 60
23 C15 sensor 23 294 260 2
  29 ahead 405
  45 behind 223
24 D1 sensor 24 365 90 2
  59 ahead 227
  33 behind 200
25 D3 sensor 25 400 35 2
  49 ahead 223
  10 behind 405
26 D5 sensor 26 550 70 2
  34 ahead 275
  48 behind 213
27 D7 sensor 27 550 42 2
  48 ahead 290
  35 behind 375
28 D9 sensor 28 550 225 2
  37 ahead 284
  47 behind 291
29 D11 sensor 29 400 260 2
  23 ahead 405
  46 behind 220
30 D13 sensor 30 400 235 2
  8 ahead 398
  56 behind 225
31 D15 sensor 31 400 215 2
  14 ahead 202
  56 behind 230
32 E1 sensor 32 325 90 2
  60 ahead 222
  39 behind 203
33 E3 sensor 33 400 50 2
  24 ahead 200
  49 behind 220
34 E5 sensor 34 470 35 2
  26 ahead 275
  49 behind 74
35 E7 sensor 35 470 11 2
  27 ahead 375
  22 behind 785
36 E9 sensor 36 550 200 2
  47 ahead 215
  38 behind 275
37 E11 sensor 37 470 260 2
  28 ahead 284
  46 behind 65
38 E13 sensor 38 470 235 2
  56 ahead 62
  36 behind 275
39 E15 sensor 39 290 50 2
  52 ahead 230
  32 behind 203
40 SW1 switch 1 140 255 curved 3
  5 straight 245
  4 curved 210
  41 behind 191
41 SW2 switch 2 180 275 curved 3
  40 straight 191
  3 curved 211
  42 behind 182
42 SW3 switch 3 230 285 curved 3
  2 straight 216
  41 curved 182
  19 behind 145
43 SW4 switch 4 140 20 straight 3
  7 straight 260
  6 curved 215
  51 behind 185
44 SW5 switch 5 375 285 curved 3
  17 straight 226
  46 curved 334
  57 behind 187
45 SW6 switch 6 245 260 straight 3
  23 straight 223
  57 curved 338
  18 behind 79
46 SW7 switch 7 447 260 straight 3
  29 straight 220
  44 curved 334
  37 behind 65
47 SW8 switch 8 570 166 curved 3
  28 straight 291
  36 curved 215
  48 behind 193
48 SW9 switch 9 570 111 curved 3
  27 straight 290
  26 curved 213
  47 behind 193
49 SW10 switch 10 445 35 straight 3
  25 straight 223
  33 curved 220
  34 behind 74
50 SW11 switch 11 220 11 curved 3
  51 straight 190
  53 curved 445
  22 behind 60
51 SW12 switch 12 180 11 curved 3
  0 straight 218
  43 curved 185
  50 behind 190
52 SW13 switch 13 245 35 straight 3
  10 straight 214
  39 curved 230
  21 behind 140
53 SW14 switch 14 150 70 curved 3
  50 straight 445
  21 curved 314
  1 behind 62
54 SW15 switch 15 150 200 curved 3
  18 straight 411
  20 curved 305
  15 behind 70
55 SW16 switch 16 245 235 straight 3
  8 straight 225
  9 curved 220
  20 behind 146
56 SW17 switch 17 445 235 straight 3
  30 straight 225
  31 curved 230
  38 behind 62
57 SW18 switch 18 315 285 curved 3
  19 straight 215
  45 curved 338
  44 behind 187
61 SW99 switch 0x99 345 155 curved 3
  62 straight 230
  16 curved 228
  58 behind 24
58 SW9A switch 0x9a 345 145 straight 3
  61 straight 24
  14 curved 224
  60 behind 32
59 SW9B switch 0x9b 345 120 straight 3
  63 straight 247
  24 curved 227
  60 behind 24
60 SW9C switch 0x9c 345 130 curved 3
  59 straight 24
  32 curved 222
  58 behind 32
62 DE1 stop 0 345 200 1
  61 ahead 230
63 DE2 stop 1 345 70 1
  59 ahead 247
64 DE3 stop 2 500 285 1
  17 ahead 410
65 DE4 stop 3 10 30 1
  6 ahead 320
66 DE5 stop 4 10 10 1
  0 ahead 500
67 DE7 stop 6 10 245 1
  11 ahead 60
68 DE9 stop 8 10 285 1
  12 ahead 70
69 DE10 stop 9 10 265 1
  13 ahead 70"""

lines = track_.split('\n')

state0 = ''
state1 = ''
idptrs = {}
id2lbl = {}

for line in lines:
    tokens = line.split(' ')
    if tokens[0] == 'track':
        continue
    if len(tokens) == 1:
        # Skip the final blank line that we don't care about
        continue
    if tokens[0] == '':
        idptrs[state0].append(tokens)
        continue
    state0 = tokens[0]
    state1 = tokens[1]
    id2lbl[state0] = state1
    idptrs[state0] = []
    print 'struct track_node %s;' % tokens[1]
    if tokens[2] == 'switch':
        print '%s.type      = SWITCH;' % tokens[1]
        print '%s.num_edges = %d;' % (tokens[1], int(tokens[7]))
    else:
        if tokens[2] == 'sensor':
            print '%s.type      = SENSOR;' % tokens[1]
        else:
            print '%s.type      = STOP;' % tokens[1]
        print '%s.num_edges = %d;' % (tokens[1], int(tokens[6]))
    print '%s.id        = %s;' % (tokens[1], tokens[3])
    print '%s.x         = %d;' % (tokens[1], int(tokens[4]))
    print '%s.y         = %d;' % (tokens[1], int(tokens[5]))

# Now need to declare-before-use fill up the edge array with addresses of other
# struct track_nodes
for key in idptrs:
    i = 0
    for edge in idptrs[key]:
        #print 'Key %s| Edge-%d \'%s\'' % (key, i, edge)
        if edge[3] == 'ahead' or edge[3] == 'straight':
            direction = 'AHEAD'
        elif edge[3] == 'behind':
            direction = 'BEHIND'
        elif edge[3] == 'curved':
            direction = 'CURVED'
        print '%s.edges[%s].dist = %d;' %   (id2lbl[key], direction, int(edge[4]))
        print '%s.edges[%s].dest = &%s;' % (id2lbl[key], direction, id2lbl[edge[2]])
        i += 1
