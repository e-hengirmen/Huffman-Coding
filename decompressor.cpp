#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
using namespace std;

struct ersel{
    ersel *left,*right;
    int number;
    char character;
};

char process_8_bits(char *current_byte,char current_bit_count,FILE *fp_read){
    char temp_byte,val;
    fread(&temp_byte,1,1,fp_read);
    *current_byte<<=8-current_bit_count;
    *current_byte|=(temp_byte>>current_bit_count);
    val=*current_byte;
    *current_byte=(temp_byte<<(8-current_bit_count));
    *current_byte>>=(8-current_bit_count);
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

void transform_to_string(char temp,string *s){
    char x;
    for(int i=0;i<=7;i++){
        x=temp;
        x>>=7;
        switch(x&1){
            case 0:*s+='0';break;
            case 1:*s+='1';break;
            default:cout<<"Problem: "<<(unsigned short int)x<<endl<<"Decompression aborted"<<endl;exit(1);
        }
        temp<<=1;
    }
}



int main(){
    char current_byte=0,current_bit_count=0,letter_count,password_length;
    FILE *fp_compressed,*fp_new;
    char file[100],newfile[100]="New-";
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
    printf("letter count:%d\n",letter_count);

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
                exit(0);
            }
        }
        if(*rp!=*pi){
            cout<<"Wrong password"<<endl;
            fclose(fp_compressed);
            exit(0);
        }
        cout<<"Correct Password"<<endl;
    }
    //above code block checks the password
    
    string transformation[256];
    fp_new=fopen(newfile,"wb");
    {
        for(int i=0;i<letter_count;i++){
            char current_character,len,temp_byte;

            current_character=process_8_bits(&current_byte,current_bit_count,fp_compressed);
            len=process_8_bits(&current_byte,current_bit_count,fp_compressed);
            // above code will read the character and the number of bits
            // we re going to need to represent this specific character's transformated version
            // after here we are going to read the transformed version of the character bit by bit.

            for(;len>7;len-=8){
                string s;
                temp_byte=process_8_bits(&current_byte,current_bit_count,fp_compressed);
                transform_to_string(temp_byte,&s);
                transformation[current_character]+=s;
            }
            //cout<<transformation[(current_character]<<endl;
        
        }
    }
    





}

/*
first (one byte)        ->  letter_count
second (one byte)       ->  password_length
third (bytes)           ->  password
fourth (bit groups)
    4.1 (8 bits)        ->  current character
    4.2 (8 bits)        ->  length of the transformation
    4.3 (bits)          ->  transformation code of that character
fifth (8 bits)          ->  bits_in last byte
sixth (a lot of bits)   ->  transformed version of the original file
*/