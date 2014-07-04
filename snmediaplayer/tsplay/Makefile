#################################################
#
# $@:   目标文件
# $^:   所有的依赖文件
# $<:   第一个依赖文件.
#
#################################################

include ./rule.in

# {exe(default), static, shared}
TARGET := libtsplayer.so
BINARY_TYPE := shared

ifndef PUB_DIR
export PUB_DIR := $(HISI_SRC)/../pub
endif

CFG_INC = -I$(PUB_DIR)/include 

LIBPATH = -L$(PUB_DIR)/lib

LIB = -Wl,--start-group -lpthread -lrt -ldl -lm -lhi_common -lhi_mpi -lhi_ecs -lhigo -lhigoadp -lfreetype -ltde -lhipng -lz -ljpeg -ljpge -lcommon -Wl,--end-group


DEFS := 
LINK_LIBS := 


INCDIR := -I$(HISI_SRC)/../sample/common
INCDIR += -I./tsplayer
INCDIR += -I../
LFLAGS := 


.PHONY: all
all: createdir $(TARGET)
	chmod a+x $(TARGET)
	@echo "-------- $(TARGET) done ---------"

SOURCES_C := $(wildcard *.c)
SOURCES_CPP := $(wildcard *.cpp)

OBJS_C := $(patsubst %.c, %.o, $(SOURCES_C))
OBJS_CPP := $(patsubst %.cpp, %.o, $(SOURCES_CPP))

OBJS := $(OBJS_C) $(OBJS_CPP)

OBJDIR := .objs


MKDIR := mkdir -p
RM := rm -rf

.PHONY: createdir
createdir: 
	$(MKDIR) $(OBJDIR)

%.o: %.c
	$(CC)  $(DEFS) $(INCDIR) $(CFG_INC) -c $< -o $(OBJDIR)/$@ 

%.o: %.cpp
	$(CXX)  $(DEFS) $(INCDIR) $(CFG_INC) -c $< -o $(OBJDIR)/$@ 

$(TARGET): $(OBJS)
ifeq ($(BINARY_TYPE), static)
	$(AR) rc $@ $(addprefix $(OBJDIR)/, $^)
else
ifeq ($(BINARY_TYPE), shared)
	$(CC) -shared -fPIC -o $@ $(addprefix $(OBJDIR)/, $^) $(CFG_INC) $(LIBPATH) $(LIB)
else
	$(CC)  -o $@ $(addprefix $(OBJDIR)/, $^)  $(CFG_INC) $(LIBPATH) $(LIB)
 endif
endif

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJDIR)

