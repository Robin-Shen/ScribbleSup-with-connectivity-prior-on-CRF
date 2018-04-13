GUROBI   = /path/to/gurobi/gurobi752/linux64
BOOST    = /path/to/boost/boost_1_66_0

#STANDARD = /usr/lib/x86_64-linux-gnu/
STANDARD = /usr/local/include

CPP      = g++-4.9 
#MUST BE G++-4.9!

CARGS    = -std=c++14
CPPLIB   = `pkg-config --libs opencv` -lpng -lgurobi_c++ -lgurobi75 

manual: main_1_manual.o image.o callback.o
	$(CPP) $(CARGS) -L$(STANDARD) -L$(GUROBI)/lib main_1_manual.o image.o callback.o $(CPPLIB) -o main_manual

auto: main_1_auto.o image.o callback.o
	$(CPP) $(CARGS) -L$(STANDARD) -L$(GUROBI)/lib main_1_auto.o image.o callback.o $(CPPLIB) -o main_auto

auto_prob: main_2_auto.o image.o callback.o
	$(CPP) $(CARGS) -L$(STANDARD) -L$(GUROBI)/lib main_2_auto.o image.o callback.o $(CPPLIB) -o main_auto_prob
	
main_2_auto.o: main_2_auto.cpp graph.h image.h callback.h
	$(CPP) $(CARGS) -I$(BOOST) -I$(GUROBI)/include -c main_2_auto.cpp

	
main_1_manual.o: main_1_manual.cpp graph.h image.h callback.h
	$(CPP) $(CARGS) -I$(BOOST) -I$(GUROBI)/include -c main_1_manual.cpp
	
main_1_auto.o: main_1_auto.cpp graph.h image.h callback.h
	$(CPP) $(CARGS) -I$(BOOST) -I$(GUROBI)/include -c main_1_auto.cpp

image.o: image.cpp graph.h image.h
	$(CPP) $(CARGS) -I$(BOOST) -I$(GUROBI)/include -c image.cpp

callback.o: callback.cpp callback.h graph.h
	$(CPP) $(CARGS) -I$(BOOST) -I$(GUROBI)/include -c callback.cpp
