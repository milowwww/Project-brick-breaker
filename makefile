CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
OBJECTS = project.o game.o ball.o brick.o paddle.o tools.o message.o

project: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o project $(OBJECTS)

project.o: project.cc game.h
	$(CXX) $(CXXFLAGS) -c project.cc

game.o: game.cc game.h ball.h brick.h paddle.h tools.h constants.h message.h
	$(CXX) $(CXXFLAGS) -c game.cc

ball.o: ball.cc ball.h tools.h
	$(CXX) $(CXXFLAGS) -c ball.cc

brick.o: brick.cc brick.h tools.h
	$(CXX) $(CXXFLAGS) -c brick.cc

paddle.o: paddle.cc paddle.h tools.h
	$(CXX) $(CXXFLAGS) -c paddle.cc

tools.o: tools.cc tools.h
	$(CXX) $(CXXFLAGS) -c tools.cc

message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) -c message.cc

clean:
	rm -f *.o project
