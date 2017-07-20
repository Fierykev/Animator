#
#  Makefile for fltk applications
#

LOCAL = /lusr

FLTK = /p

INCLUDE = -I$(LOCAL)/include -I$(FLTK)/include -I/lusr/X11/include
LIBDIR = -L$(LOCAL)/lib -L$(FLTK)/lib -L/lusr/X11/lib

LIBS = -lfltk -lfltk_gl -lXext -lX11 -lm -lGL -lGLU -lGLEW

CFLAGS = -g -std=c++11

CC = g++

.SUFFIXES: .o .cpp .cxx

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $*.o $<

ALL.O = bitmap.o camera.o color.o curve.o curveevaluator.o \
        graphwidget.o indicatorwindow.o linearcurveevaluator.o \
        modelerapp.o modelerdraw.o modelerui.o animatoruiwindows.o \
        modelerview.o particleSystem.o point.o \
        rect.o robotarm.o rulerwindow.o \
        ObjectFileLoader.o batman.o Particle.o \
		bsplinecurveevaluator.o \
		catmulromevaluator.o beziercurveevaluator.o \
		c2interpevaluator.o c1beziercurveevaluator.o
		

animator: $(ALL.O)
	$(CC) $(CFLAGS) -o $@ $(ALL.O) $(INCLUDE) $(LIBDIR) $(LIBS)

clean:  
	rm -f $(ALL.O)

clean_all:
	rm -f $(ALL.O) animator
