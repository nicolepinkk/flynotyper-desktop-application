CC= g++
CFLAGS=	-g -std=c++17 -I/usr/local/include/opencv4 #`pkg-config --cflags --libs opencv`  
AOBJS=	image.o ientropy.o
WX-CFLAGS= `wx-config --cxxflags --libs`
MP-FLAGS= -fopenmp
LIBS=	-L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs
PROG = desktop_app

UNAME_S := $(uname -s)

ifeq ($(UNAME_S),Linux)
	CC = g++
else ifeq ($(UNAME_S),Darwin)
	CC = clang++
	LIBS += -lomp
	MP-FLAGS = -Xpreprocessor -fopenmp
endif

all:$(PROG)
       
desktop_app: desktop_app.cpp $(AOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(MP-FLAGS) $(WX-CFLAGS) $(LIBS)

%.o : %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@  $(LIBS)

clean:
	rm -f *.o $(PROG)
