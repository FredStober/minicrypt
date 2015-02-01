SOURCES = Hash.cpp Crypt.cpp Tiger.cpp
HEADERS = Hash.h   Crypt.h   Tiger.h   HashTools.h

all: minicrypt.bin test.bin

minicrypt.bin: $(SOURCES) $(HEADERS) MiniCrypt.cpp I18N.cpp I18N.h resource.h
	$(CXX) -o $@ -std=c++11 $(SOURCES) MiniCrypt.cpp I18N.cpp `wx-config --cxxflags` `wx-config --libs`

test.bin: $(SOURCES) $(HEADERS) test.cpp
	$(CXX) -o $@ -std=c++11 $(SOURCES) test.cpp

clean:
	rm -f *.o *.bin
