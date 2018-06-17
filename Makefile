all: cycle

cycle: cycle1.bin 
cycle1.bin: cyclethes.cpp traceswr.hpp traceswr.cpp
	g++ -Wall -Wextra -O2 -g -std=c++11 -o cycle1.bin cyclethes.cpp traceswr.cpp nauty26r10/nauty.a
cycle2.bin: cycleart.cpp pp2p.cpp pp2p.hpp
	g++ -Wall -Wextra -O2 -g -std=c++11 -o cycle2.bin cycleart.cpp pp2p.cpp
