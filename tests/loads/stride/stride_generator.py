import os
loads = int(input('Number of loads = '))
#stride = int(input('stride = '))
op_type = input('Type (ld/st) = ')

def gen(loads, op_type, stride,N):
    if(op_type=='ld'):
        lines = [f'#define N {N}\n', 'void main(){\n', 'static long int a[N];\n', 'register long int i, j=0;\n']
        
        i=0
        while(i<N):
            lines.append('j = j + a['+str(i)+'] + 1;\n')
            i+=stride

        lines.append('__asm__("mov $60, %rax\\n\\t"\n')
        lines.append('\t\t"xor	%rdi, %rdi\\n\\t"\n')
        lines.append('\t"syscall\\n\\t");\n')
        lines.append('}')
        f=open(f'ld{stride}-{loads}.c','w')
        print(f'ld{stride}-{loads}.c generated')
        f.writelines(lines)
        f.close()
    else:
        lines = [f'#define N {N}\n', 'void main(){\n', 'static long int a[N];\n', 'register long int i, j=0;\n']
        i=0
        while(i<N):
            lines.append(f'a[{i}] = j + 1;\n')
            i+=stride

        lines.append('__asm__("mov $60, %rax\\n\\t"\n')
        lines.append('\t\t"xor	%rdi, %rdi\\n\\t"\n')
        lines.append('\t"syscall\\n\\t");\n')
        lines.append('}')
        f=open(f'st{stride}_{loads}.c','w')
        print(f'st{stride}_{loads}.c', 'file generated')
        f.writelines(lines)
        f.close()
        os.system(f'gcc -O0 -nostdlib -Wl,-emain st{stride}_{loads}.c -o st{stride}')

for stride in range(1,9):
    N = loads*stride
    gen(loads, op_type,stride, N)
