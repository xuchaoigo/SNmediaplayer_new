#################################################
#
# $@:   目标文件
# $^:   所有的依赖文件
# $<:   第一个依赖文件.
#
#################################################

include ./rule.in

# {exe(default), static, shared}
BINARY_TYPE := exe

TARGET := rtsp_test

DEFS := 


INCDIR += -I../include 
LFLAGS := -lpthread -lm
LFLAGS += -L../src  -lrtsp

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
	$(CC)  $(DEFS) $(INCDIR) -c $< -o $(OBJDIR)/$@ 

%.o: %.cpp
	$(CXX)  $(DEFS) $(INCDIR) -c $< -o $(OBJDIR)/$@ 

$(TARGET): $(OBJS)
ifeq ($(BINARY_TYPE), static)
	$(AR) rc $@ $(addprefix $(OBJDIR)/, $^)
else
ifeq ($(BINARY_TYPE), shared)
	$(CC) -shared -fPIC -o $@ $(addprefix $(OBJDIR)/, $^)
else
	$(CXX)  -o $@ $(addprefix $(OBJDIR)/, $^)  $(LFLAGS)
endif
endif

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJDIR)

