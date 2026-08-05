import math

numcpus = 8
avgipcs = []
for i in range(9,16):
    f = open(f'output/micromix{i}/stats.txt', 'r')
    ipcs = [-2 for i in range(numcpus)]
    for line in f:
        if('totalIpc' in line):
            l = line.split()
            cpuid = int(l[0].split('.')[1][-1])
            ipcs[cpuid] = float(l[1])
    
    print(f'mix{i}')
    print(ipcs)
    f.close()
    print(sum(ipcs)/numcpus)
    avgipcs.append(round(sum(ipcs)/numcpus, 6))

print('averages')
for i in avgipcs:
    print(i)