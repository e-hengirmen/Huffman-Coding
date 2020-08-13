# Huffman_Coding
A C++ compression program based on Huffman's lossless compression algorithm and its decompressor

**Compression and Decompression codes are ready, but they are only tested in Linux based operating systems. Use it at your discretion.**

## How do I use it?

1. Run below script using your favourate shell:
```
    chmod u+x Huffman.sh
    ./Huffman.sh
```

2. After running the script, you can use archive command below to compress the file you want:
```
    ./archive {{filename}}
```
3.  And to decompress the compressed file, use the extract command below:
```
    ./extract {{filename}}
```
* If you want to check size of a file use the commands below
```
    g++ size_checker.cpp -o check_size
    ./check_size {{filename}}
```
## Shortcomings
* Compressor.cpp does not compress multiple files
* Compressor.cpp does not compress folders

## TODO list
* ~~Finish decompressor~~(DONE)
* ~~Add a size checker~~(DONE)
* ~~Improve documentation~~(DONE)
* Name of the file should be added to compression code
* ~~Add a make file or shell script~~(Done)(added script)
* Huffman.cpp should compress multiple files
* Huffman.cpp should compress folders
