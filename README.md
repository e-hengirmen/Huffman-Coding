# Huffman_Coding
A C++ compression program based on Huffman's lossless compression algorithm and its decompressor

**Compressor and Decompressor has been finished but they haven't been tested throughly they are only tested in linux based operating systems. Use at your own discretion**

## How do I use it?

### Run shell script with:

    ./Huffman.sh

After running the script you can use archive command to compress the file you want like this:

    ./archive {filename}

And to decompress the compressed file use the extract command like below:

    ./extract {filename}

## Shortcomings
* Huffman.cpp does not compress multiple files
* Huffman.cpp does not compress folders

## TODO list
* ~~Finish decompressor~~(DONE)
* ~~Add a size checker~~(DONE)
* Improve documentation
* Make it so that name of the file is added to compressed file and translated from it by decompress
* ~~Add a make file or shell script~~(Done)(added script)
* Huffman.cpp should compress multiple Files
* Huffman.cpp should compress folders
