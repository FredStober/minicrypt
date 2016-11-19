SOURCES = Crypt.cpp
HEADERS = Crypt.h HashTools.h tiger_hash.hpp

all: minicrypt.bin test.bin

minicrypt.bin: $(SOURCES) $(HEADERS) MiniCrypt.cpp I18N.cpp I18N.h resource.h
	$(CXX) -o $@ -std=c++14 $(SOURCES) MiniCrypt.cpp I18N.cpp `wx-config --cxxflags` `wx-config --libs`

test.bin: $(SOURCES) $(HEADERS) test.cpp
	$(CXX) -o $@ -std=c++14 $(SOURCES) test.cpp

clean:
	rm -f *.o *.bin
