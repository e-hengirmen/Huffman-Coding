all : archive extract

archive : Compressor.cpp
	g++ -std=c++14 Compressor.cpp -o archive

extract : Decompressor.cpp
	g++ -std=c++14 Decompressor.cpp -o extract

clean :
	@rm -f archive
	@rm -f extract

.PHONY : all clean
