# Huffman_Coding
*Version 0.9 compresses 1 file at a time*

Self-made C++ compression program based on Huffman's lossless compression algorithm and its decompressor

## How does it work?
### Compressor:
The Compressor is a 2-pass program. What I mean by this is that the Compressor reads the original file 2 times.

In the first pass, the program counts usage frequency of every unique byte and creates a weighted translation tree for every used unique byte inversely proportional to its usage frequency and then writes this transformation info to the compressed file for decompression purposes

In the second pass, the program decodes the file according to the translation tree and writes it to the newly created compressed file

### Decompressor:
The Decompressor is a 1-pass program:
The Decompressor first reads translation info and creates a binary tree from it. After this process is done, it uses this binary translation tree to translate the rest of the file

## How does one use it?

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
* If you want to check the size of a file use the command below
```
    ./check_size {{filename}}
```
## Shortcomings
* Compressor.cpp does not compress multiple files
* Compressor.cpp does not compress folders
