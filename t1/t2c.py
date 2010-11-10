# turn data file into C structures
# TODO, rewrite this in Scheme
#  create the array mapping sensors/switches/stops into the graph

file  = 'tracka'
#file  = 'trackb'
data  = open(file).read()
lines = data.split('\n')

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
        print '%s.type      = SWITCH' % tokens[1]
        print '%s.num_edges = %d' % (tokens[1], int(tokens[7]))
        #print 'struct track_edge *%s_edges[%d];' % (tokens[1], int(tokens[7]))
        print 'struct track_edge *%s_edges[3];' % tokens[1]
    else:
        if tokens[2] == 'sensor':
            print '%s.type      = SENSOR' % tokens[1]
        else:
            print '%s.type      = STOP' % tokens[1]
        print '%s.num_edges = %d;' % (tokens[1], int(tokens[6]))
        #print 'struct track_edge *%s_edges[%d];' % (tokens[1], int(tokens[6]))
        print 'struct track_edge *%s_edges[3];' % tokens[1]
    print '%s.id        = %s;' % (tokens[1], tokens[3])
    print '%s.x         = %d;' % (tokens[1], int(tokens[4]))
    print '%s.y         = %d;' % (tokens[1], int(tokens[5]))
    print '%s.edges     = %s_edges;' % (tokens[1], tokens[1])

# Now need to declare-before-use fill up the edge array with addresses of other
# struct track_nodes
for key in idptrs:
    i = 0
    for edge in idptrs[key]:
        print 'Key %s| Edge-%d \'%s\'' % (key, i, edge)
	edge_name = '%s_%s' % (id2lbl[key], id2lbl[edge[2]])
        # Default all of the edges to NULL
        for j in [0, 1, 2]:
            print '%s.edges[%d] = NULL;' % (id2lbl[key], j)
        if edge[3] == 'ahead':
            print '%s.edges[AHEAD]  = &%s;' % (id2lbl[key], edge_name)
        if edge[3] == 'behind':
            print '%s.edges[BEHIND] = &%s;' % (id2lbl[key], edge_name)
        if edge[3] == 'curved':
            print '%s.edges[CURVED] = &%s;' % (id2lbl[key], edge_name)
        print 'struct track_edge %s;' % edge_name
	print '%s.dist = %d' % (edge_name, int(edge[4]))
	print '%s.dest = &%s' % (edge_name, id2lbl[edge[2]])
        #print '%s.edges[%d] = &%s;' % (id2lbl[key], i, edge_name)
        i += 1
