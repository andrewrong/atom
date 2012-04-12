#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mem.h"
#include "atom.h"

#define NELEM(x) (sizeof(x) / sizeof(x[0]))
struct atom
{
    struct atom *link;
    int len;
    char *str;
};
typedef struct atom atom; 
atom* buckets[2048] = {NULL};

static unsigned long scatter[256] = {281286,3219,924748,838926,953998,196433,388902,73599,965216,38123,912379,166517,262425,129587,487599,707625,149827,168340,390521,145896,747038,945897,91322,150386,752558,916467,539739,728514,45712,812002,638002,563558,118533,743044,643843,897842,426396,744256,190606,451841,205084,367301,581206,698028,701443,632480,67873,524095,695785,214580,159466,54670,62322,912749,759517,50986,503035,387751,560124,263855,992080,720453,507079,355469,816143,4287,967669,948754,120618,575002,775666,21700,445398,916098,370309,819376,558806,784780,44890,87073,317820,172954,70158,211228,475623,210605,90699,863959,564151,867620,180146,233230,492353,763809,65774,635399,974143,798063,622736,514328,709806,275546,288753,881221,90484,697932,300044,3193,122392,158756,607280,979268,75639,893134,831901,152058,394789,964290,543066,154392,653612,562029,631384,35104,858778,163901,560139,447677,904473,749183,227015,108653,106230,146450,200135,970049,754184,903840,187163,79085,150151,416451,37947,702638,253865,22931,855210,831217,949403,792203,687256,11896,48214,791685,617302,246953,103138,925729,712448,964059,285121,17471,450850,725612,545793,34297,242133,852288,567643,596118,255739,778460,270543,586752,110255,194836,993642,568886,21959,774522,477897,571236,138220,675941,807970,48031,307523,352356,170033,830639,585813,208272,6947,418895,632177,874809,185976,890712,811862,261539,382219,829172,158126,595076,404157,495274,159804,20860,544504,59444,969816,228571,935041,785201,471935,32207,698262,83359,860156,214355,211236,507947,883331,93705,855203,334077,126856,666102,671360,652752,1576,760095,218973,836909,536294,116802,527710,645005,93043,408200,963071,98293,167749,259336,733152,297718,8419,918770,845885,550486,434769,910665,660029,139961,945400,746492};


static void init_scatter()
{
    int i = 0;

    srand(10);

    for(i = 0; i < 256; i++)
    {
	scatter[i] = rand();
	if(scatter[i] > 1000000)
	{
	    i--;
	}
    }

    for(i = 0; i < 256; i++)
    {
	printf("%lu,",scatter[i]);	
    }
    
    printf("\n");
}

//增加的用来判断的函数
//0:表示是用来str是不是存在buckets中
//1：表示是如果str在buckets中的话就删除这个原子
static int is_in_buckets(const char* str,int len,int flag)
{
    unsigned long h = 0;
    atom* pre = NULL;
    atom* ptr = NULL;
    int index = 0;
    int i = 0;

    assert(str);
    assert(len >= 0);
    
    for(i = 0; i < len; i++)
    {
	h = (h<<1) + scatter[(unsigned char)str[i]];
    }

    index = h % (NELEM(buckets));
    
    for(ptr = pre = buckets[index]; ptr != NULL; ptr = ptr->link)
    {
	if(ptr->str == str)
	{
	    switch(flag)
	    {
		case 0:
		    return 1;
		    break;
		case 1:
		    {
			if(pre == buckets[index])
			{
			    buckets[index] = ptr->link;
			}
			else
			{
			    pre->link = ptr->link;
			}
			FREE(ptr);
			return 1;	
			break;
		    }
	    }
	}
	pre = ptr;
    }

    return 0;
}

const char* atom_string(const char *str) 
{
    assert(str);

    return atom_new(str,strlen(str),0);
}

const char* atom_int(long n) 
{
    char str[43];
    char *str1 = str + sizeof(str); unsigned long m; if(n == LONG_MIN)
    {
	m = LONG_MAX + 1UL;
    }
    else if(n < 0)
    {
	m = -n;
    }
    else
    {
	m = n;
    }

    do
    {
	*(--str1) = m%10 + '0';
	m = m/10;
    }while(m);

    if(n < 0)
    {
	*(--str1) = '-';
    }

    return atom_new(str1,str + sizeof(str) - str1,0);
}

//flag 表示了对str是否进行复制的一个过程
//0:表示的是如果对str进行开辟空间复制
//1：表示不进行空间复制
const char* atom_new(const char*str,int len,int flag)
{
    unsigned long h = 0;
    atom* ptr = NULL;
    int index = 0;
    int i = 0;

    assert(str);
    assert(len >= 0);
    
    for(i = 0; i < len; i++)
    {
	h = (h<<1) + scatter[(unsigned char)str[i]];
    }

    index = h % (NELEM(buckets));
    
    for(ptr = buckets[index]; ptr != NULL; ptr = ptr->link)
    {
	if(ptr->len == len)
	{
	    for(i = 0; str[i] == ptr->str[i] && i < len; i++);

	    if(i == len)
	    {
		return ptr->str;
	    }
	}
    }
    
    //recreate atom

    switch(flag)
    {
	case 0:
	    {
		ptr = (atom*)MALLOC(sizeof(struct atom)+ len + 1);
    	    	assert(ptr);
    	    	
    	    	ptr->len = len;
    	    	ptr->str = (char*)(ptr + 1);

    	    	if(len > 0)
    	    	{
    	    	    memcpy(ptr->str,str,len);
    	    	}
    	    	ptr->str[len] = '\0';

		break;
	    }
	case 1:
	    {
		ptr = (atom*)MALLOC(sizeof(struct atom));
		assert(ptr);

		ptr->len = len;
		ptr->str = str;

		break;
	    }
    }

    ptr->link = buckets[index];
    buckets[index] = ptr;

    return ptr->str;
}

int atom_length(const char *str)
{
    assert(str);

   if(is_in_buckets(str,strlen(str),0)) 
   {
       return strlen(str);
   }

    assert(0);
    return 0;
}

void atom_reset()
{
    atom* ptr = NULL;
    int i = 0;

    for(i = 0; i < NELEM(buckets); i++)
    {
	ptr = buckets[i];

	for(;ptr != NULL;)
	{
	    atom* tmp = ptr;
	    ptr = ptr->link;
	    FREE(tmp);
	}
    }
}

void atom_free(const char *str)
{
    assert(str);

    if(!is_in_buckets(str,strlen(str),1))
    {
	assert(0);			
    }
}

void atom_vload(const char *str,...)
{
    va_list vap;
    
    assert(str);

    va_start(vap,str);
    for(str;str;str = va_arg(vap,char*))
    {
	atom_string(str);
    }
}

void atom_aload(const char* str[])
{
    const char* tmp = NULL;
    int i = 0;
    
    assert(str);
    for(tmp = str[i]; tmp; i++)
    {
	atom_string(tmp);
    }
}

const char* atom_add(const char* str,int len) 
{
    assert(str);
    assert(len >= 0);
    return atom_new(str,len,1);    
}

