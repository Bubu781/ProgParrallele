#include <stdio.h>
#include <stdlib.h>

int x;

int function1 (void)
{
    static int z;
    
    z = x;
    printf("|z        | %p    |                |                |\n", (void*)&z);
    
    return (z);
}

int main(int argc, char **argv) 
{
    int y = 0;
	  int loop = 0;
    char *str;
	  char cpy_str[] = "Ce cours est genial! Maintenant je sais reconnaitre les zones memoires ou sont situees mes variables";

    y = 4;
    x = y;

    str = malloc(100*sizeof(char));
    for (loop = 0; loop < 100; ++loop)
		str[loop] = cpy_str[loop];
	  printf("str : %s\n", str);
    free(str);
    
    printf("______________________________________________________________\n");
    printf("|Variable |    Stactic     |      Stack     |      Heap      |\n");
    printf("|_________|________________|________________|________________|\n");
    printf("|x        | %p    |                |                |\n", (void*)&x);
    printf("|y        |                | %p |                |\n", (void*)&y);
    loop = function1 ();
    printf("|str      |                |                | %p |\n", (void*)&str[0]);
    printf("|cpy_str  |                | %p |                |\n", (void*)&cpy_str[0]);
    printf("|loop     |                | %p |                |\n", (void*)&loop);
    printf("|_________|________________|________________|________________|\n");
    
    return 0;
}