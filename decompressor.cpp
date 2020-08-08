#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
using namespace std;

void str_without_compress(char*);
unsigned char process_n_bits_NUMBER(unsigned char*,unsigned char,int*,FILE*);
void process_n_bits_TO_STRING(unsigned char*,unsigned char,int*,FILE*,string*);




/*
first (one byte)        ->  letter_count
second (one byte)       ->  password_length
third (bytes)           ->  password
fourth (bit groups)
    4.1 (8 bits)        ->  current character
    4.2 (8 bits)        ->  length of the transformation
    4.3 (bits)          ->  transformation code of that character

fifth (8 bits)          ->  bits_in_last_byte
sixth (a lot of bits)   ->  transformed version of the original file
*/



struct translation{
    ersel *zero=NULL,*one=NULL;
    int number;
    unsigned char character;
};

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
    str_without_compress(file);
    strcpy(newfile+4,file);
    fread(&letter_count,1,1,fp_compressed);
    


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
    //above code block checks the password






    string transformation[256];
    unsigned char current_byte=0,len,current_character;
    int current_bit_count=0;
    unsigned char control_bit[9]={128,1,2,4,8,16,32,64,128};

    for(int i=0;i<letter_count;i++){
        current_character=process_n_bits_NUMBER(&current_byte,8,&current_bit_count,fp_compressed);
        len=process_n_bits_NUMBER(&current_byte,8,&current_bit_count,fp_compressed);
        process_n_bits_TO_STRING(&current_byte,len,&current_bit_count,fp_compressed,&transformation[current_character]);
    }

    ///*----------delete later-----------
    //        writes transformation code
    for(int i=0;i<256;i++){
        if(transformation[i]!=""){
            cout<<transformation[i]<<" "<<i<<endl;
        }
    }

    //-----------------------------------*/
}

void process_n_bits_TO_STRING(unsigned char *current_byte,unsigned char n,int *current_bit_count,FILE *fp_read,string *str){
    unsigned char check=0b10000000;
    for(int i=0;i<n;i++){
        if(*current_bit_count==0){
            fread(current_byte,1,1,fp_read);
            *current_bit_count=8;
        }

        switch((*current_byte)&check){
            case 0:*str+='0';break;
            case 128:*str+='1';break;
        }
        (*current_byte)<<=1;
        (*current_bit_count)--;
    }
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