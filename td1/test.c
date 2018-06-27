#include <stdio.h>

int main(int argc, char const *argv[])
{
	int x = 10;
	int y = 20;
	printf("%d\n", x - 1);
	printf("%d\n", x - -1);
	printf("%d\n", x - - 1);
	printf("%d\n", ---x);
	printf("%d\n", x - -y);
	
	/*printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	printf("%d\n", );
	*/
	return 0;
}