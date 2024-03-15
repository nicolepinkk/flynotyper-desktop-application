CC=	g++
CFLAGS=	-g -std=c++17 -I/usr/local/include/opencv4 #`pkg-config --cflags --libs opencv`  
AOBJS=	image.o ientropy.o
WX-CFLAGS= `wx-config --cxxflags --libs`
PROG=	desktop_app
LIBS=	-L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs

all:$(PROG)
       
desktop_app: desktop_app.cpp $(AOBJS)
	gsettings set org.gnome.mutter check-alive-timeout 150000
	$(CC) $(CFLAGS) -o $@ $^ $(WX-CFLAGS) $(LIBS)

%.o : %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@  $(LIBS)

clean:
	rm -f *.o $(PROG)
	
