objdir = objs
$(shell mkdir bin lib ${objdir})
platform=$(shell uname)
$(warning ${platform})

compiler = g++ 
incdirs = -I./include -I/usr/local/Ice-3.6.1/include -I../public/libcurl/include -I../public

ifeq (${platform}, AIX)
cppflags = -D_REENTRANT -D_EVENTDES -DAIX -D__linux__  -D_GNU_SOURCE -DIA64 -DICE_CONST_ICONV_INBUF
libs = -lIceUtil -lpthread -liconv -lcurl
libdirs = -L./ -L../libs -L../libs/aix -L/usr/local/Ice-3.6.1/lib -L/usr/local/lib
target = ../libs/aix/libSwAgentSdk.a
endif

ifeq (${platform}, HP-UX)
cppflags = -D_REENTRANT -D_EVENTDES -DHPUX 
libs = -lIceUtil -lpthread -liconv -lcurl
libdirs = -L./ -L../libs -L../libs/hpux -L/usr/local/Ice-3.6.1/lib -L/usr/local/lib
target = ../libs/hpux/libSwAgentSdk.a
endif

ifeq (${platform}, Linux)
cppflags = -m32 -D_REENTRANT -D_EVENTDES -DLINUX
libs = -m32 -lIceUtil -lpthread -liconv -lcurl
libdirs = -L./ -L../libs -L../libs/linux -L/usr/local/Ice-3.6.1/lib -L/usr/local/lib
target = ../libs/linux/libSwAgentSdk.a
endif

vpath %.cpp src src/util src/thread src/http
srcfile = $(wildcard src/*.cpp)  $(wildcard src/util/*.cpp) $(wildcard src/thread/*.cpp) $(wildcard src/http/*.cpp)

objs = $(patsubst %.cpp, ${objdir}/%.o, $(notdir ${srcfile}))
${objdir}/%.o:	%.cpp
	${compiler} -g ${cppflags} ${incdirs} -c $< -o $@

${target}:	${objs}
	rm -f $@
	ar rcs $@ $(objs)
	rm -f $(objs)
	rm -rf ../public/skywalking/include/*
	mkdir -p ../public/skywalking/include
	cp ./include/*.h ../public/skywalking/include
#	${compiler} ${libdirs} $^ ${libs} -o $@

clean:
	rm -f ${target} ${objs}
	
all: ${target}
 
rebuild: clean  all