index = 1
go = False
nodes = []
with open('cylinder_tetrahedron.msh', 'r') as f:
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
from math import sin, cos

print(f'''
$NodeData
1
"Scalar"
1
0
3
0
3
{len(nodes)}''')
for node in nodes:
    # print(node)
    id, x, y, z = node.split(' ')
    x = float(x)
    print(int(id), x+1, cos(x*10), sin(x*10))
print('$EndNodeData')