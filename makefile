OUT = project
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
PKGS = gtkmm-4.0
GTKFLAGS = $(shell pkg-config --cflags $(PKGS))
GTKLIBS = $(shell pkg-config --libs $(PKGS))

OBJECTS = project.o gui.o graphic.o game.o ball.o brick.o paddle.o tools.o message.o

$(OUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJECTS) $(GTKLIBS)

project.o: project.cc gui.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c project.cc

gui.o: gui.cc gui.h graphic_gui.h constants.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c gui.cc

graphic.o: graphic.cc graphic.h graphic_gui.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c graphic.cc

game.o: game.cc game.h ball.h brick.h paddle.h tools.h constants.h message.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c game.cc

ball.o: ball.cc ball.h tools.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c ball.cc

brick.o: brick.cc brick.h tools.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c brick.cc

paddle.o: paddle.cc paddle.h tools.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c paddle.cc

tools.o: tools.cc tools.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c tools.cc

message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) -c message.cc

clean:
	rm -f *.o project