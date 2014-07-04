#################################################
#
# $@:   目标文件
# $^:   所有的依赖文件
# $<:   第一个依赖文件.
#
#################################################

include ./rule.in

# {exe(default), static, shared}
TARGET := libSNmediaplayer.so
BINARY_TYPE := shared

DEFS := -s
LINK_LIBS := 


INCDIR += -I./include 
INCDIR += -I../RTSP_prv/include/
INCDIR += -I./tsplay
LFLAGS := 


.PHONY: all
all: createdir $(TARGET)
	chmod a+x $(TARGET)
	@echo "-------- $(TARGET) done ---------"

#SOURCES_C := $(wildcard *.c)
#SOURCES_C := utf8_gb2312.c common_setting.c 
#SOURCES_CPP := $(wildcard *.cpp)
SOURCES_CPP := SNmediaplayer.cpp RealizeSNmediaplayer.cpp StreamMediaPlayer.cpp RTSPClient_prv.cpp#TVClient.cpp VodClient.cpp \
		#ui_display.cpp timer_handler.cpp LocalFilePlayer.cpp  mp3_parse.cpp RTSPClient.cpp

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
	$(CC) $(LFLAGS) -o $@ $(addprefix $(OBJDIR)/, $^)  $(LINK_LIBS)
endif
endif

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJDIR)

