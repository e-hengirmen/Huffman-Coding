# Huffman_Coding
A C++ compression program based on Huffman's lossless compression algorithm and its decompressor

**Compressor and Decompressor have been finished but they haven't been tested throughly they are only tested in linux based operating systems. Use at your own discretion**

## How do I use it?

1. Run below script using your favourate shell:
```
    chmod u+x Huffman.sh
    ./Huffman.sh
```

2. After running the script you can use archive command below to compress the file you want:
```
    ./archive {filename}
```
3. And to decompress the compressed file use the extract command below:
```
    ./extract {filename}
```
* If you want to check size of a file use the commands below
```
    g++ size_checker.cpp -o check_size
    ./check_size {filename}
```
## Shortcomings
* Huffman.cpp does not compress multiple files
* Huffman.cpp does not compress folders

## TODO list
* ~~Finish decompressor~~(DONE)
* ~~Add a size checker~~(DONE)
* Improve documentation
* Make it so that name of the file is added to compressed file and translated from it by decompress
* ~~Add a make file or shell script~~(Done)(added script)
* Huffman.cpp should compress multiple files
* Huffman.cpp should compress folders
