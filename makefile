minicrypt.bin: Hash.cpp Hash.h HashTools.h MiniCrypt.cpp resource.h Tiger.cpp Tiger.h
	$(CXX) -o $@ -std=c++11 Hash.cpp Tiger.cpp MiniCrypt.cpp `wx-config --cxxflags` `wx-config --libs`
