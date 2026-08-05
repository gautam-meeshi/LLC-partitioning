//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#define N 700000


void main()
{
//	long int *a;
//	a  = (long int *) malloc(N*sizeof(long int));
  	static long int a[N];
	register long int i, j=0;

	for (i = 0; i < N; i=i+8)
	{
		j = j + a[i] + 1;
	}

	__asm__("mov $60, %rax\n\t"
	        "xor	%rdi, %rdi\n\t"
		"syscall\n\t");
//	printf("\n\nValue of j after: %ld\n\n",j);
//	return 0;
}
