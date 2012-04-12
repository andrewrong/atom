.PHONY:clean
.PHONY:all

MKDIR = mkdir
RM = rm
RMFLAGS = -fr

CC = gcc

DIR_OBJS = objs
DIR_OUTS = outs
DIR_DEPS = deps
DIRS = $(DIR_OBJS) $(DIR_OUTS) $(DIR_DEPS)

OUTS = main 
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS)) 
DEPS = $(patsubst %.c,%.dep,$(SRCS))

OBJS :=$(addprefix $(DIR_OBJS)/,$(OBJS))
OUTS := $(addprefix $(DIR_OUTS)/,$(OUTS))
DEPS :=$(addprefix $(DIR_DEPS)/,$(DEPS))

ifeq ("$(wildcard $(DIR_OBJS))","")
    DEP_DIR_OBJS := $(DIR_OBJS)
endif

ifeq ("$(wildcard $(DIR_OUTS))","")
    DEP_DIR_OUTS := $(DIR_OUTS)
endif

ifeq ("$(wildcard $(DIR_DEPS))","")
    DEP_DIR_DEPS := $(DIR_DEPS)
endif

all:$(OUTS) 

include $(DEPS) 

$(DIRS):
	$(MKDIR) $@


$(OUTS):$(OBJS) $(DEP_DIR_OUTS) 
	$(CC) $(filter %.o,$^) -o $@ 

$(DIR_OBJS)/%.o:%.c $(DEP_DIR_OBJS)
	$(CC) -c $(filter %.c,$^) -o $@ 

$(DIR_DEPS)/%.dep:%.c $(DEP_DIR_DEPS)
	@echo "creating $@..."
	@set -e;\
	    $(RM) $(RMFLAGS) $@.tmp;\
	    $(CC) -E -MM $(filter %.c,$^) >$@.tmp;\
	    sed 's,\(.*\)\.o[ :]*,objs/\1.o $@: ,g' < $@.tmp > $@;\
	    $(RM) $(RMFLAGS) $@.tmp;
	    
clean:
	$(RM) $(RMFLAGS)  $(DIRS)
