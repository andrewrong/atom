#include <stdio.h>
#include <limits.h>
#include "mem.h"
#include "atom.h"

int main()
{
    char *str = "hello1";
    char *str2 = "hello2";
    int a = 101;

    mem_in_id = -1;
    is_check = 1;

    mem_check_init();
    printf("str = %s\n",atom_string(str));
    printf("str = %s\n",atom_int(LONG_MIN));
    //printf("len = %d\n",atom_length(str2));
    
    atom_reset();
    mem_check_write();
    return 0;
}
