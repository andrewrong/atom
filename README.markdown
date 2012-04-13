*****
###关于atom
>这个程序来自于&lt;&lt;C interface and implementations>>这本书中的第二章的一个例子.

1.  什么是atom

    atom可以简单的认为是一个指向字符串的一个指针,但是与指向字符串指针的区别在于它的唯一性,也就是说每一个atom指向的字符串是唯一的,而且是不能被改变的.

    其实atom也在很多的地方被使用着,比如在编译器中就可以通过atom将源代码中的标识符和常字符串都保存到atom表中,那样就可以节省很多内存空间.

2.  接口介绍

	    atom_length(const char*str)
    这个接口主要是用来完成对原子str返回长度.如果str本身不是原子的话将中断程序本身

	    atom_new(const char *str,int len,int flag)
    这个接口定义的不是很好,本身的作用是用来创建一个str的原子的,如果str本身已经存在就返回一个存在的指针,如果不存在就创建并返回.atom_new会创建str本身的空间然后进行复制,但atom_add的前提是客户本身可以保证不会释放str本身而在创建过程中不对str本身进行申请空间而只是指针的复制

	    atom_string(const char* str)
    类似于atom_new

	    atom_int(long n)
    将n变成字符串进行保存

	    atom_reset
    释放所有的原子

	    atom_free
    释放某一个原子

	    atom_vload
    加载字符串

	    atom_aload
    与atom_vload类似

3.  技术关键点的介绍

    1.数组的长度    
    >	     #define NELEM(x) (sizeof(x) / sizeof(x[0]))
    虽然我知道这个用法,但是真的很少我会使用这样的用法.我通常最多也就是用一个宏去定义数组的长度,然后进行调用.但是用这个在编写代码的时候我发现基本上这样是很稳定啦,以后对数组的改动,也基本上不会影响很大,这是一个值得学习的地方

    2.关于long的LONG_MAX
    >	    if(n == LONG_MIN)
    >	    {
    >		  m = LONG_MAX + 1UL;
    >        }
    >	    else if(n < 0)
    >	    {
    >	        m = -n;
    >	    }
    >	    else
    >	    {
    >	        m = n;
    >	    }
    关于这段代码,让我觉得自己写程序很粗心.首先long的最小值不能用绝对值表示这就是一个关键的一点,所以程序用一个无符号的long来表示这个最小值.关于&lt;limits>这个头文件保存了一些数据结构的最大值和最小值.

    3.不定参数的使用
    >	    void atom_vload(const char *str,...)
    >	    {
    >	        va_list vap;
    >	        
    >	        assert(str);
    >	    
    >	        va_start(vap,str);
    >	        for(str;str;str = va_arg(vap,char*))
    >	        {
    >	    	atom_string(str);
    >	        }
    >	    }
    关于不定参数我其实也用过几次,可是每次重新使用的时候就会忘记,所以今天在重新学习一下.相对语法还是比较简单的,va_list 用来声明一个不定参数列表,va_start用来初始化不定列表.va_arg这是从不定列表中取出参数.va_end结束
    
    4.free 和 malloc
    这两个函数本身的使用其实可以说很简单,你可以成对的使用通常情况下是不会有问题的.但是在我的程序中我使用了一种以前没有用过的方法

		ptr = (atom*)MALLOC(sizeof(struct atom)+ len + 1);

    之前都没有试过这样的方式.上面的这个申请空间本身就申请的比struct atom的大小要多(len+1),所以我在想如果在free的时候是不是要先把多的那个部分先手动的释放,然后在释放ptr.因为我一直以为free在释放内存的时候是根据ptr本身的数据类型来释放内存的大小的.可后在网上看了其实是不用的.下面就来介绍关于free和malloc拉...

    malloc本身申请的空间是在进程的堆空间.个人觉得os肯定是会对这部分的堆空间进行管理的,可能是链表的形式.根据&lt;&lt;unix 环境高级编程>>所讲的那样,malloc在分配空间的时候都是会分配比要求的大一些的空间,多的那个空间是用来存放一些信息,这些信息是**分配的大小和指向下一个分配块的指针**,也就是说申请的空间可以分为可见空间和不可见空间,不可见的空间是用来存放内存的信息,下面看看这些信息和free源代码.

	    struct mem_control_block { 
	    int is_available;   
    	    int size;            
    	    };

	    void  free(void *ptr)  
    	    { 
    	            struct mem_control_block *free; 
    	            free = ptr - sizeof(struct mem_control_block); 
    	            free->is_available = 1; 
    	            return; 
    	    }

    mem_control_block就是这个信息的,而它本身应该是放在可见空间的前面,从free的源代码中可以看出.所以我们可以知道的是free本身好像没有做一些实质性的东西.不过可以肯定的是free不是根据ptr的数据类型来释放空间的.
