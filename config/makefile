MAIN_SOURCES = ../src/main.cpp
MAIN_OBJECTS = ../build/main.o
COMMON_SOURCES = ../src/cont.cpp ../src/env.cpp ../src/expr.cpp ../src/parse.cpp ../src/step.cpp ../src/value.cpp 
INCS = ../src/catch.hpp ../src/cont.hpp ../src/env.hpp ../src/expr.hpp ../src/parse.hpp ../src/pointer.hpp ../src/step.hpp ../src/value.hpp
OBJS = ../build/cont.o ../build/env.o ../build/expr.o ../build/parse.o ../build/step.o ../build/value.o 
LIBS = ../build/msdscriptlib.a

CXX = clang++
CXXFLAGS = -std=c++11

msdscript: msdscriptlib.a $(MAIN_OBJECTS) $(INCS)
	$(CXX) $(CXXFLAGS) $(MAIN_OBJECTS) $(LIBS) -o ../build/msdscript

msdscriptlib.a: $(OBJS) $(INCS)
	$(AR) rsv msdscriptlib.a $(OBJS)
	mv ./msdscriptlib.a $(LIBS)

../build/cont.o: ../src/cont.cpp $(INCS) 
	$(CXX) $(CXXFLAGS) -c -o ../build/cont.o $<

../build/env.o: ../src/env.cpp $(INCS) 
	$(CXX) $(CXXFLAGS) -c -o ../build/env.o $<

../build/expr.o: ../src/expr.cpp $(INCS)  
	$(CXX) $(CXXFLAGS) -c -o ../build/expr.o $<

../build/main.o: $(MAIN_SOURCES) $(INCS)
	$(CXX) $(CXXFLAGS) -c -o $(MAIN_OBJECTS) $<

../build/parse.o: ../src/parse.cpp $(INCS) 
	$(CXX) $(CXXFLAGS) -c -o ../build/parse.o $<

../build/step.o: ../src/step.cpp $(INCS) 
	$(CXX) $(CXXFLAGS) -c -o ../build/step.o $<

../build/value.o: ../src/value.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c -o ../build/value.o $<