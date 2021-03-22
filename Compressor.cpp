#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include "progress_bar.hpp"

using namespace std;


void write_from_uChar(unsigned char,unsigned char&,int,FILE*);

int this_is_not_a_folder(char*);
long int size_of_the_file(char*);
void count_in_folder(string,long int*,long int&,long int&);

void write_file_count(int,unsigned char&,int,FILE*);
void write_file_size(long int,unsigned char&,int,FILE*);
void write_file_name(char*,string*,unsigned char&,int&,FILE*);
void write_the_file_content(FILE*,long int,string*,unsigned char&,int&,FILE*);
void write_the_folder(string,string*,unsigned char&,int&,FILE*);





/*          CONTENT TABLE IN ORDER
---------PART 1-CALCULATING TRANSLATION INFO----------
Important Note:4 and 5 are the most important parts of this algorithm
If you dont know how Huffman's algorithm works I really recommend you to check this link before you continue:
https://en.wikipedia.org/wiki/Huffman_coding#Basic_technique

1-Size information
2-Counting usage frequency of unique bytes and unique byte count
3-Creating the base of the translation array
4-Creating the translation tree inside the translation array by weight distribution
5-adding strings from top to bottom to create translated versions of unique bytes

---------PART 2-CREATION OF COMPRESSED FILE-----------
    Compressed File's structure had been documented below

first (one byte)            ->  letter_count
second (bit group)
    2.1 (one byte)          ->  password_length
    2.2 (bytes)             ->  password (if password exists)
third (bit groups)
    3.1 (8 bits)            ->  current unique byte
    3.2 (8 bits)            ->  length of the transformation
    3.3 (bits)              ->  transformation code of that unique byte

fourth (2 bytes)**          ->  file_count (inside the current folder)
    fifth (1 bit)*          ->  file or folder information  ->  folder(0) file(1)
    sixth (8 bytes)         ->  size of current input_file (IF FILE)
    seventh (bit group)
        7.1 (8 bits)        ->  length of current input_file's or folder's name
        7.2 (bits)          ->  transformed version of current input_file's or folder's name
    eighth (a lot of bits)  ->  transformed version of current input_file (IF FILE)

*whenever we see a new folder we will write seventh then start writing from fourth to eighth
**groups from fifth to eighth will be written as much as file count in that folder
    (this is argument_count-1(argc-1) for the main folder)

*/

progress PROGRESS;

struct ersel{   //this structure will be used to create the translation tree
    ersel *left,*right;
    long int number;
    unsigned char character;
    string bit;
};

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
}



int main(int argc,char *argv[]){
    long int number[256];
    long int total_bits=0;
    int letter_count=0;
    if(argc==1){
        cout<<"Missing file name"<<endl<<"try './archive {{file_name}}'"<<endl;
        return 0;
    }
    for(long int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    string scompressed;
    FILE *original_fp,*compressed_fp;

    for(int i=1;i<argc;i++){                    //checking for wrong input
        if(this_is_not_a_folder(argv[i])){
            original_fp=fopen(argv[i],"rb");
            if(!original_fp){
                cout<<argv[i]<<" file does not exist"<<endl<<"Process has been terminated"<<endl;
                return 0;
            }
            fclose(original_fp);
        }
    }

    scompressed=argv[1];
    scompressed+=".compressed";


    //------------------1 and 2--------------------
        // This code block counts number of times that all of the unique bytes is used on the files and file names and folder names
        // and stores that info in 'number' array
            // after this code block, program checks the 'number' array
            //and writes the number of unique bytes count to 'letter_count' variable

    unsigned char *x_p,x;                  //these are temp variables to take input from the file
    x_p=&x;
    long int total_size=0,size;
    total_bits+=16+9*(argc-1);
    for(int current_file=1;current_file<argc;current_file++){

        for(char *c=argv[current_file];*c;c++){        //counting usage frequency of unique bytes on the file name (or folder name)
            number[(unsigned char)(*c)]++;
        }

        if(this_is_not_a_folder(argv[current_file])){
            total_size+=size=size_of_the_file(argv[current_file]);
            total_bits+=64;

            original_fp=fopen(argv[current_file],"rb");
            fread(x_p,1,1,original_fp);
            for(long int j=0;j<size;j++){    //counting usage frequency of unique bytes inside the file
                number[x]++;
                fread(x_p,1,1,original_fp);
            }
            fclose(original_fp);

        }
        else{
            string temp=argv[current_file];
            count_in_folder(temp,number,total_size,total_bits);
        }        
    }

	for(long int *i=number;i<number+256;i++){                 
        	if(*i){
			letter_count++;
			}
    }
    //---------------------------------------------



    //--------------------3------------------------
        // creating the base of translation array(and then sorting them by ascending frequencies
        // this array of type 'ersel' will not be used after calculating transformed versions of every unique byte
        // instead its info will be written in a new string array called str_arr 
    ersel array[letter_count*2-1];
    ersel *e=array;
    for(long int *i=number;i<number+256;i++){                         
        	if(*i){
                e->right=NULL;
                e->left=NULL;
                e->number=*i;
                e->character=i-number;
                e++;
            }
    }
    sort(array,array+letter_count,erselcompare0);
    //---------------------------------------------
    
                   
    
    //-------------------4-------------------------
        // min1 and min2 represents nodes that has minimum weights
        // isleaf is the pointer that traverses through leafs and
        // notleaf is the pointer that traverses through nodes that are not leafs
    ersel *min1=array,*min2=array+1,*current=array+letter_count,*notleaf=array+letter_count,*isleaf=array+2;            
    for(int i=0;i<letter_count-1;i++){                           
        current->number=min1->number+min2->number;
        current->left=min1;
        current->right=min2;
        min1->bit="1";
        min2->bit="0";     
        current++;
        
        if(isleaf>=array+letter_count){
            min1=notleaf;
            notleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min1=isleaf;
                isleaf++;
            }
            else{
                min1=notleaf;
                notleaf++;
            }
        }
        
        if(isleaf>=array+letter_count){
            min2=notleaf;
            notleaf++;
        }
        else if(notleaf>=current){
            min2=isleaf;
            isleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min2=isleaf;
                isleaf++;
            }
            else{
                min2=notleaf;
                notleaf++;
            }
        }
        
    }
        // At every cycle, 2 of the least weighted nodes will be chosen to
        // create a new node that has weight equal to sum of their weights combined.
            // After we are done with these nodes they will become childrens of created nodes
            // and they will be passed so that they wont be used in this process again.
    //---------------------------------------------


    
    //-------------------5-------------------------
    for(e=array+letter_count*2-2;e>array-1;e--){
        if(e->left){
            e->left->bit=e->bit+e->left->bit;
        }
        if(e->right){
            e->right->bit=e->bit+e->right->bit;
        }
        
    }
        // In this block we are adding the bytes from root to leafs
        // and after this is done every leaf will have a transformation string that corresponds to it
            // Note: It is actually a very neat process. Using 4th and 5th code blocks, we are making sure that
            // the most used character is using least number of bits.
                // Specific number of bits we re going to use for that character is determined by weight distribution
    //---------------------------------------------












    compressed_fp=fopen(&scompressed[0],"wb");
    int current_bit_count=0;
    unsigned char current_byte;
    //--------------writes first--------------
    fwrite(&letter_count,1,1,compressed_fp);
    total_bits+=8;
    //----------------------------------------



    //--------------writes second-------------
    {
        cout<<"If you want a password write any number other then 0"<<endl
            <<"If you do not, write 0"<<endl;
        int check_password;
        cin>>check_password;
        if(check_password){
            string password;
            cout<<"Enter your password (Do not use whitespaces): ";
            cin>>password;
            int password_length=password.length();
            if(password_length==0){
                cout<<"You did not enter a password"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 0;
            }
            if(password_length>100){
                cout<<"Password cannot contain more then 100 characters"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 0;
            }
            unsigned char password_length_unsigned=password_length;
            fwrite(&password_length_unsigned,1,1,compressed_fp);
            fwrite(&password[0],1,password_length,compressed_fp);
            total_bits+=8+8*password_length;
        }
        else{
            fwrite(&check_password,1,1,compressed_fp);
            total_bits+=8;
        }
    }
        //Above code block puts password to compressed file
    //----------------------------------------







    



    //------------writes third---------------
    char *str_pointer;
    unsigned char len,current_character;
    string str_arr[256];
    for(e=array;e<array+letter_count;e++){
        str_arr[(e->character)]=e->bit;     //we are putting the transformation string to str_arr array to make the compression process more time efficient
        len=e->bit.length();
        current_character=e->character;

        write_from_uChar(current_character,current_byte,current_bit_count,compressed_fp);
        write_from_uChar(len,current_byte,current_bit_count,compressed_fp);
        total_bits+=len+16;
        // above lines will write the byte and the number of bits
        // we re going to need to represent this specific byte's transformated version
        // after here we are going to write the transformed version of the number bit by bit.
        
        str_pointer=&e->bit[0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Compression process aborted"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 1;
            }
           str_pointer++;
        }
        
         total_bits+=len*(e->number);
    }
    if(total_bits%8){
        total_bits=(total_bits/8+1)*8;        
        // from this point on total bits doesnt represent total bits
        // instead it represents 8*number_of_bytes we are gonna use on our compressed file
    }
    // Above loop writes the translation script into compressed file and the str_arr array
    //----------------------------------------


    cout<<"The size of the sum of ORIGINAL files is: "<<total_size<<" bytes"<<endl;
    cout<<"The size of the COMPRESSED file will be: "<<total_bits/8<<" bytes"<<endl;
    cout<<"Compressed file's size will be [%"<<100*((float)total_bits/8/total_size)<<"] of the original file"<<endl;
    if(total_bits/8>total_size){
        cout<<endl<<"COMPRESSED FILE'S SIZE WILL BE HIGHER THAN THE SUM OF ORIGINALS"<<endl<<endl;
    }
    cout<<"If you wish to abort this process write 0 and press enter"<<endl
        <<"If you want to continue write any other number and press enter"<<endl;
    int check;
    cin>>check;
    if(!check){
        cout<<endl<<"Process has been aborted"<<endl;
        fclose(compressed_fp);
        remove(&scompressed[0]);
        return 0;
    }



    PROGRESS.MAX=(array+letter_count*2-2)->number;      //setting progress bar

    //-------------writes fourth---------------
    write_file_count(argc-1,current_byte,current_bit_count,compressed_fp);
    //---------------------------------------

    for(int current_file=1;current_file<argc;current_file++){
        
        if(this_is_not_a_folder(argv[current_file])){   //if current is a file and not a folder
            original_fp=fopen(argv[current_file],"rb");
            fseek(original_fp,0,SEEK_END);
            size=ftell(original_fp);
            rewind(original_fp);

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_byte|=1;
            current_bit_count++;
            //---------------------------------------

            write_file_size(size,current_byte,current_bit_count,compressed_fp);             //writes sixth
            write_file_name(argv[current_file],str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh
            write_the_file_content(original_fp,size,str_arr,current_byte,current_bit_count,compressed_fp);      //writes eighth
            fclose(original_fp);
        }
        else{   //if current is a folder instead

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_bit_count++;
            //---------------------------------------

            write_file_name(argv[current_file],str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh

            string folder_name=argv[current_file];
            write_the_folder(folder_name,str_arr,current_byte,current_bit_count,compressed_fp);
        }
    }





    if(current_bit_count==8){      // here we are writing the last byte of the file
        fwrite(&current_byte,1,1,compressed_fp);
    }
    else{
        current_byte<<=8-current_bit_count;
        fwrite(&current_byte,1,1,compressed_fp);
    }

    fclose(compressed_fp);
    system("clear");
    cout<<endl<<"Created compressed file: "<<scompressed<<endl;
    cout<<"Compression is complete"<<endl;
    
}



//below function is used for writing the uChar to compressed file
    //It does not write it directly as one byte instead it mixes uChar and current byte, writes 8 bits of it 
    //and puts the rest to curent byte for later use
void write_from_uChar(unsigned char uChar,unsigned char &current_byte,int current_bit_count,FILE *fp_write){
    current_byte<<=8-current_bit_count;
    current_byte|=(uChar>>current_bit_count);
    fwrite(&current_byte,1,1,fp_write);
    current_byte=uChar;   
}



//below function is writing number of files we re going to translate inside current folder to compressed file's 2 bytes
    //It is done like this to make sure that it can work on little, big or middle-endian systems
void write_file_count(int file_count,unsigned char &current_byte,int current_bit_count,FILE *compressed_fp){
    unsigned char temp=file_count%256;
    write_from_uChar(temp,current_byte,current_bit_count,compressed_fp);
    temp=file_count/256;
    write_from_uChar(temp,current_byte,current_bit_count,compressed_fp);
}



//This function is writing byte count of current input file to compressed file using 8 bytes
    //It is done like this to make sure that it can work on little, big or middle-endian systems
void write_file_size(long int size,unsigned char &current_byte,int current_bit_count,FILE *compressed_fp){
    PROGRESS.next(size);        //updating progress bar
    for(int i=0;i<8;i++){
        write_from_uChar(size%256,current_byte,current_bit_count,compressed_fp);
        size/=256;
    }
}



// This function writes bytes that are translated from current input file's name to the compressed file.
void write_file_name(char *file_name,string *str_arr,unsigned char &current_byte,int &current_bit_count,FILE *compressed_fp){
    write_from_uChar(strlen(file_name),current_byte,current_bit_count,compressed_fp);
    char *str_pointer;
    for(char *c=file_name;*c;c++){
        str_pointer=&str_arr[(unsigned char)(*c)][0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Process has been aborted";
                exit(2);
            }
            str_pointer++;
        }
    }
}



// Below function translates and writes bytes from current input file to the compressed file.
void write_the_file_content(FILE *original_fp,long int size,string *str_arr,unsigned char &current_byte,int &current_bit_count,FILE *compressed_fp){
    unsigned char *x_p,x;
    x_p=&x;
    char *str_pointer;
    fread(x_p,1,1,original_fp);
    for(long int i=0;i<size;i++){
        str_pointer=&str_arr[x][0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Process has been aborted";
                exit(2);
            }
            str_pointer++;
        }
        fread(x_p,1,1,original_fp);
    }
}

int this_is_not_a_folder(char *path){
    DIR *temp=opendir(path);
    if(temp){
        closedir(temp);
        return 0;
    }
    return 1;
}

long int size_of_the_file(char *path){
    long int size;
    FILE *fp=fopen(path,"rb");
    fseek(fp,0,SEEK_END);
    size=ftell(fp);
    fclose(fp);
    return size;
}



// This function counts usage frequency of bytes inside a folder
    // only give folder path as input
void count_in_folder(string path,long int *number,long int &total_size,long int &total_bits){
    FILE *original_fp;
    path+='/';
    DIR *dir=opendir(&path[0]),*next_dir;
    string next_path;
    total_size+=4096;
    total_bits+=16; //for file_count
    struct dirent *current;
    while((current=readdir(dir))){
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }
        total_bits+=9;

        for(char *c=current->d_name;*c;c++){        //counting usage frequency of bytes on the file name (or folder name)
            number[(unsigned char)(*c)]++;
        }

        next_path=path+current->d_name;

        if((next_dir=opendir(&next_path[0]))){
            closedir(next_dir);
            count_in_folder(next_path,number,total_size,total_bits);
        }
        else{
            long int size;
            unsigned char *x_p,x;
            x_p=&x;
            total_size+=size=size_of_the_file(&next_path[0]);
            total_bits+=64;

            //--------------------2------------------------
            original_fp=fopen(&next_path[0],"rb");

            fread(x_p,1,1,original_fp);
            for(long int j=0;j<size;j++){    //counting usage frequency of bytes inside the file
                number[x]++;
                fread(x_p,1,1,original_fp);
            }
            fclose(original_fp);
        }
    }
    closedir(dir);
}







void write_the_folder(string path,string *str_arr,unsigned char &current_byte,int &current_bit_count,FILE *compressed_fp){
    FILE *original_fp;
    path+='/';
    DIR *dir=opendir(&path[0]),*next_dir;
    string next_path;
    struct dirent *current;
    int file_count=0;
    long int size;
    while((current=readdir(dir))){
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }
        file_count++;
    }
    rewinddir(dir);
    write_file_count(file_count,current_byte,current_bit_count,compressed_fp);  //writes fourth

    while((current=readdir(dir))){  //if current is a file
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }

        next_path=path+current->d_name;
        if(this_is_not_a_folder(&next_path[0])){

            original_fp=fopen(&next_path[0],"rb");
            fseek(original_fp,0,SEEK_END);
            size=ftell(original_fp);
            rewind(original_fp);

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_byte|=1;
            current_bit_count++;
            //---------------------------------------

            write_file_size(size,current_byte,current_bit_count,compressed_fp);                     //writes sixth
            write_file_name(current->d_name,str_arr,current_byte,current_bit_count,compressed_fp);                //writes seventh
            write_the_file_content(original_fp,size,str_arr,current_byte,current_bit_count,compressed_fp);      //writes eighth
            fclose(original_fp);
        }
        else{   // if current is a folder

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_bit_count++;
            //---------------------------------------

            write_file_name(current->d_name,str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh

            write_the_folder(next_path,str_arr,current_byte,current_bit_count,compressed_fp);
        }
    }
    closedir(dir);


}