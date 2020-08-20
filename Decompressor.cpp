#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

unsigned char check=0b10000000;

struct translation{
    translation *zero,*one;
    unsigned char character;
};

void change_name_if_exists(char*);
unsigned char process_8_bits_NUMBER(unsigned char*,int,FILE*);
void process_n_bits_TO_STRING(unsigned char*,int,int*,FILE*,translation*,unsigned char);
void burn_tree(translation*);



/*          CONTENT TABLE IN ORDER
    compressed file's composition is in order below
    that is why we re going to translate it part by part

.first (one byte)           ->  letter_count
.second (2 bytes)           ->  file_count
.third (bit group)
    3.1 (one byte)          ->  password_length
    3.2 (bytes)             ->  password (if password exists)
.fourth (bit groups)
    4.1 (8 bits)            ->  current character
    4.2 (8 bits)            ->  length of the transformation
    4.3 (bits)              ->  transformation code of that character

    .fifth (8 bytes)        ->  size of current original_file
    .sixth (bit group)
        6.1 (8 bits)        ->  length of current original_file's name
        6.2 (bits)          ->  transformed version of current original_file's name
    .seventh (a lot of bits)->  transformed version of current original_file

**groups from fourth to sixth will be written as much as the file count
*/







int main(int argc,char *argv[]){
    int letter_count=0,password_length=0;
    FILE *fp_compressed,*fp_new;
    if(argc==1){
        cout<<"Missing file name"<<endl<<"try './extract {{file_name}}'"<<endl;
        return 0;
    }
    fp_compressed=fopen(argv[1],"rb");
    if(!fp_compressed){
        cout<<argv[1]<<" does not exist"<<endl;
        return 0;
    }



    //---------reads .first-----------
    fread(&letter_count,1,1,fp_compressed);
    if(letter_count==0)letter_count=256;
    //-------------------------------



    //---------reads .second----------
    int file_count;
    {
        unsigned char temp;
        fread(&temp,1,1,fp_compressed);
        file_count=temp;
        fread(&temp,1,1,fp_compressed);
        file_count+=256*temp;
    }
    //--------------------------------



    //-----------------reads .third--------------------
    fread(&password_length,1,1,fp_compressed);
    if(password_length){
        char real_password[password_length+1],password_input[257];
        fread(real_password,1,password_length,fp_compressed);
        real_password[password_length]=0;
        cout<<"Enter password:";
        cin>>password_input;
        char *rp=real_password,*pi=password_input;
        for(;*rp&&*pi;rp++,pi++){
            if(*rp!=*pi){
                cout<<"Wrong password"<<endl;
                fclose(fp_compressed);
                return 0;
            }
        }
        if(*rp!=*pi){
            cout<<"Wrong password"<<endl;
            fclose(fp_compressed);
            return 0;
        }
        cout<<"Correct Password"<<endl;
    }
        // this code block reads and checks the password
    //--------------------------------------------------


    //----------------reads .fourth---------------------
        // and stores transformation info into translation tree for later use
    unsigned char current_byte=0,current_character;
    int current_bit_count=0,len;
    translation *root=(translation*)malloc(sizeof(translation));
    root->zero=NULL;
    root->one=NULL;

    for(int i=0;i<letter_count;i++){
        current_character=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed);
        len=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed);
        if(len==0)len=256;
        process_n_bits_TO_STRING(&current_byte,len,&current_bit_count,fp_compressed,root,current_character);
    }
    //--------------------------------------------------










    for(int current_file=0;current_file<file_count;current_file++){

        //---------reads .fifth-----------
        long int size=0;
        {
            long int multiplier=1;
            for(int i=0;i<8;i++){
                size+=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed)*multiplier;
                multiplier*=256;
            }
        }
            // Size was written to the compressed file from least significiant byte 
            // to the most significiant byte to make sure system's endianness
            // does not affect the process and that is why we are processing size information like this
        //--------------------------------



        //---------------reads .sixth---------------------
            //Decodes current file's name
        int file_name_length=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed);
        char newfile[file_name_length+4];       //change later
        newfile[file_name_length]=0;          //change later
        translation *node;
        for(int i=0;i<file_name_length;i++){
            node=root;
            while(node->zero||node->one){
                if(current_bit_count==0){
                    fread(&current_byte,1,1,fp_compressed);
                    current_bit_count=8;
                }
                if(current_byte&check){
                    node=node->one;
                }
                else{
                    node=node->zero;
                }
                current_byte<<=1;           
                current_bit_count--;
            }
            newfile[i]=node->character;
        }
        change_name_if_exists(newfile);
        //--------------------------------------------------


        //---------------reads .seventh---------------------
            // This code block translates compressed file from info that is now stored in the translation tree
            // than writes it to a newly created file
        fp_new=fopen(newfile,"wb");
        for(long int i=0;i<size;i++){
            node=root;
            while(node->zero||node->one){
                if(current_bit_count==0){
                    fread(&current_byte,1,1,fp_compressed);
                    current_bit_count=8;
                }
                if(current_byte&check){
                    node=node->one;
                }
                else{
                    node=node->zero;
                }
                current_byte<<=1;           
                current_bit_count--;
            }
            fwrite(&(node->character),1,1,fp_new);
        }
        //--------------------------------------------------
        fclose(fp_new);
    }











    


    



    



    








    fclose(fp_compressed);
    burn_tree(root);
    cout<<"Decompression is complete"<<endl;
}

void burn_tree(translation *node){          // this function is used for deallocating translation tree
    if(node->zero)burn_tree(node->zero);
    if(node->one)burn_tree(node->one);
    free(node);
}

// process_n_bits_TO_STRING function reads n successive bits from the compressed file
// and stores it in a leaf of the translation tree
// after creating that leaf and sometimes nodes that are binding that leaf to the tree
void process_n_bits_TO_STRING(unsigned char *current_byte,int n,int *current_bit_count,FILE *fp_read,translation *node,unsigned char uChar){
    for(int i=0;i<n;i++){
        if(*current_bit_count==0){
            fread(current_byte,1,1,fp_read);
            *current_bit_count=8;
        }

        switch((*current_byte)&check){
            case 0:
            if(!(node->zero)){
                node->zero=(translation*)malloc(sizeof(translation));
                node->zero->zero=NULL;
                node->zero->one=NULL;
            }
            node=node->zero;
            break;
            case 128:
            if(!(node->one)){
                node->one=(translation*)malloc(sizeof(translation));
                node->one->zero=NULL;
                node->one->one=NULL;
            }
            node=node->one;
            break;
        }
        (*current_byte)<<=1;
        (*current_bit_count)--;
    }
    node->character=uChar;
}

// process_8_bits_NUMBER reads 8 successive bits from compressed file
// and returns it in unsigned char form
unsigned char process_8_bits_NUMBER(unsigned char *current_byte,int current_bit_count,FILE *fp_read){
    unsigned char val,temp_byte;
    fread(&temp_byte,1,1,fp_read);
    val=(*current_byte)|(temp_byte>>current_bit_count);
    *current_byte=temp_byte<<8-current_bit_count;
    return val;
}

void change_name_if_exists(char *name){
    FILE *fp=fopen(name,"rb");
    char *i;
    int copy_count;
    if(fp){
        fclose(fp);
        char *dot_pointer=NULL;
        for(i=name;*i;i++){
            if(*i=='.')
            dot_pointer=i;
        }
        if(dot_pointer){
            string s=dot_pointer;
            strcpy(dot_pointer,"(1)");
            dot_pointer++;
            strcpy(dot_pointer+2,&s[0]);
        }
        else{
            dot_pointer=i;
            strcpy(dot_pointer,"(1)");
            dot_pointer++;
        }
        for(copy_count=1;copy_count<10;copy_count++){
            *dot_pointer=(char)('0'+copy_count);
            if(fp=fopen(name,"rb")){
                fclose(fp);
            }
            else{
                break;
            }
        }
    }
}