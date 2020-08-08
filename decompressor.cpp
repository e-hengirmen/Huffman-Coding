#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

struct translation{
    translation *zero=NULL,*one=NULL;
    unsigned char character;
};

void str_without_compress(char*);
unsigned char process_n_bits_NUMBER(unsigned char*,unsigned char,int*,FILE*);
void process_n_bits_TO_STRING(unsigned char*,unsigned char,int*,FILE*,translation*,unsigned char);
void burn_tree(translation*);



//delete this function later----------------------------------
void print_tree(translation* node,int n){       //delete later
    for(int i=0;i<n;i++)cout<<" ";
    if(node->zero){
        cout<<0<<endl;
        print_tree(node->zero,n+1);
    }
    for(int i=0;i<n;i++)cout<<" ";
    if(node->one){
        cout<<1<<endl;
        print_tree(node->one,n+1);
    }
    if(!node->one&&!node->zero){
        cout<<"character="<<(int)node->character<<endl;
    }
}
//-----------------------------------------------------------


/*          CONTENT TABLE IN ORDER

.first (8 bytes)         ->  size of the original file
.second (one byte)       ->  letter_count
.third (one byte)        ->  password_length
.fourth (bytes)          ->  password (if password exists)
.fifth (bit groups)
    5.1 (8 bits)        ->  current character
    5.2 (8 bits)        ->  length of the transformation
    5.3 (bits)          ->  transformation code of that character
.sixth (a lot of bits)   ->  transformed version of the original file
*/







int main(){
    int letter_count,password_length;
    register FILE *fp_compressed,*fp_new;
    char file[100],newfile[104]="New-";
    cout<<"Enter the name of the compressed file"<<endl;
    cin>>file;
    fp_compressed=fopen(file,"rb");
    if(!fp_compressed){
        cout<<file<<" does not exist"<<endl;
        return 0;
    }



    //---------reads .first-----------
    long int size=0;
    {
        long int multiplier=1;
        unsigned char temp;
        for(int i=0;i<8;i++){
            fread(&temp,1,1,fp_compressed);
            size+=temp*multiplier;
            multiplier*=256;
        }
    }
        //Size was written to the compressed file from least significiant byte 
        //to the most significiant byte to make sure system's endianness does
        //not affect the process and that is why we are getting size information like this
    //-------------------------------



    str_without_compress(file);
    strcpy(newfile+4,file);
    //---------reads .second-----------
    fread(&letter_count,1,1,fp_compressed);
    //-------------------------------



    //------------reads .third and .fourth--------------------
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
        //this code block reads and checks the password
    //----------------------------------------------------



    //----------------reads .fifth----------------------
        //and stores transformation info for later use
    unsigned char current_byte=0,len,current_character;
    int current_bit_count=0;
    translation *root=(translation*)malloc(sizeof(translation));

    for(int i=0;i<letter_count;i++){
        current_character=process_n_bits_NUMBER(&current_byte,8,&current_bit_count,fp_compressed);
        len=process_n_bits_NUMBER(&current_byte,8,&current_bit_count,fp_compressed);
        process_n_bits_TO_STRING(&current_byte,len,&current_bit_count,fp_compressed,root,current_character);
    }
    //--------------------------------------------------



    print_tree(root,0);     //delete later


    
    //----------------reads .sixth----------------------
    fp_new=fopen(newfile,"wb");
    translation *node;
    {
        unsigned char check=0b10000000;
        for(long int i=0;i<size;i++){
            node=root;
            while(node->zero||node->one){
                if(current_bit_count==0){
                    fread(&current_byte,1,1,fp_compressed);
                    current_bit_count=8;
                }
                current_byte<<=1;           //
                if(current_byte&check){
                    node=node->one;
                }
                else{
                    node=node->zero;
                }
                current_byte<<=1;           //
                current_bit_count--;
            }
            fwrite(&(node->character),1,1,fp_new);
        }
    }
    //--------------------------------------------------

    burn_tree(root);
    cout<<"Decompression is complete"<<endl;
}

void burn_tree(translation *node){
    if(node->zero)burn_tree(node->zero);
    if(node->one)burn_tree(node->one);
    free(node);
}

void process_n_bits_TO_STRING(unsigned char *current_byte,unsigned char n,int *current_bit_count,FILE *fp_read,translation *node,unsigned char uChar){
    unsigned char check=0b10000000;
    for(int i=0;i<n;i++){
        if(*current_bit_count==0){
            fread(current_byte,1,1,fp_read);
            *current_bit_count=8;
        }

        switch((*current_byte)&check){
            case 0:
            if(!(node->zero))node->zero=(translation*)malloc(sizeof(translation));
            node=node->zero;
            break;
            case 128:
            if(!(node->one))node->one=(translation*)malloc(sizeof(translation));
            node=node->one;
            break;
        }
        (*current_byte)<<=1;
        (*current_bit_count)--;
    }
    node->character=uChar;
}

unsigned char process_n_bits_NUMBER(unsigned char *current_byte,unsigned char n,int *current_bit_count,FILE *fp_read){
    unsigned char check=0b10000000,val=0;
    if(n>8){
        fclose(fp_read);
        cout<<"Coding Error"<<endl;
        exit(123);
    }
    for(int i=0;i<n;i++){
        val<<=1;
        if(*current_bit_count==0){
            fread(current_byte,1,1,fp_read);
            *current_bit_count=8;
        }

        if((*current_byte)&check)val|=1;
        (*current_byte)<<=1;
        (*current_bit_count)--;
    }
    return val;
}

void str_without_compress(char *p){
    char s[12]="desserpmoc.";
    char *p1=p,*p2=s;
    if(strlen(p)<12)return;
    for(;*p1;p1++);
    p1--;
    for(;*p2;p1--,p2++){
        if(*p1!=*p2){
            return;
        }
    }
    p1++;
    *p1=0;
}