# Parse the sample text files into DOT

graph = {}
file = 'tracka'
data = open(file).read()
lines = data.split('\n')

for i in xrange(0, 230):
    tokens = lines[i].split(' ')
    if tokens[0]:
        #print 'New node! \'%s\'' % tokens
        graph[tokens[0]] = []
        next = i + 1
        if i >= 226:
            break
        nextline = lines[next].split(' ')
        while nextline[0] == '':
            #print 'Line to add \'%s\'' % nextline
            graph[tokens[0]].append(nextline[2])
            next += 1
            nextline = lines[next].split(' ')

print 'digraph traingraph {'
for key in graph:
    print '%s -> {' % key
    for edge in graph[key]:
        print '%s; ' % edge
    print '}'
print '}'
#print 'Graph looks like:\n%s' % graph
