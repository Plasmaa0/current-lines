index = 1
go = False
nodes = []
with open('t1.msh', 'r') as f:
    for line in f:
        if '$Nodes' in line:
            go = True
            continue
        if '$End' in line:
            go = False
        if go:
            line = line.strip()
            nodes.append(line)

nodes.pop(0)
# print(nodes)
from math import sin

for node in nodes:
    # print(node)
    id, x, y, z = node.split(' ')
    y = float(y)
    print(int(id), sin(y * 20) * 4, abs(sin(y * 20) * 4) + 1, 0)
