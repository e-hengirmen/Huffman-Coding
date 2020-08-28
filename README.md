<p align="center">
  <img src="https://github.com/e-hengirmen/Huffman_Coding/blob/master/images/logo.png">
</p>

<h1 align="center">Huffman Coding</h1>

<p align="center">
  <a href="https://github.com/e-hengirmen/Huffman_Coding/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/e-hengirmen/Huffman_Coding"
      alt="MIT License" />
  </a>
  <a href="https://www.linkedin.com/in/ersel-hengirmen/">
    <img src="https://img.shields.io/badge/-LinkedIn-black.svg?style=plastic-square&logo=linkedin&colorB=555"
      alt="MIT License" />
  </a>
</p>

Self-made C++ **file archiver** and **archive extractor** programs based on Huffman's lossless compression algorithm
## Table of Contents

* [How does it work](#how-does-it-work)
  * [Compressor](#compressor)
  * [Decompressor](#decompressor)
* [How does one use it?](#how-does-one-use-it)

## How does it work?
### Compressor:
The `Compressor` is a 2-pass program. What I mean by this is that the `Compressor` reads input files twice.

In the first pass, the program counts usage frequency of every unique byte and creates a weighted translation tree for every used unique byte inversely proportional to its usage frequency and then writes this transformation info to the compressed file for decompression purposes

In the second pass, the program translates input files according to the translation tree and writes it to the newly created compressed file

### Decompressor:
The `Decompressor` is a 1-pass program:
The `Decompressor` first reads translation info and creates a binary tree from it. After this process is done, it uses this binary translation tree to decode the rest of the file

## How does one use it?
1. Compile with `make` using your favourate shell:
```
make all
```
2. After running make, you can use `archive` command below to compress the file you want:
* To compress one file use:
```
./archive {{filename}}
```
* To compress multiple files use:

```
./archive {{filename1}} {{filename2}} ...
```
3.  And to decompress a compressed file, use the `extract` command below:
```
./extract {{filename}}
```
