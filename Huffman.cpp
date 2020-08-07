#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;


/*
COMPRESSED FILE WILL BE LIKE THIS

first (one byte)        ->  letter_count
second (one byte)       ->  password_length
third (bytes)           ->  password (if password exists)
fourth (bit groups)
    4.1 (8 bits)        ->  current character
    4.2 (8 bits)        ->  length of the transformation
    4.3 (bits)          ->  transformation code of that character
fifth (8 bits)          ->  bits_in last byte
sixth (a lot of bits)   ->  transformed version of the original file
*/


struct ersel{
    ersel *left,*right;
    int number;
    unsigned char character;
    string bit;
};

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
}

bool erselcomparec(ersel a,ersel b){
    return a.character<b.character;
}





int main(){
    int number[256];
    long long int bits=0,total_bits=0;
    int letter_count=0;
    for(int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    cout<<"Enter the name of the file you want to compress"<<endl;
    string s,scompressed;
	cin>>s;
    register FILE *original_fp=fopen(&s[0],"rb"),*compressed_fp;
    if(NULL==original_fp){
        cout<<s<<" file does not exist"<<endl;
        return 0;
    }
    scompressed=s+".compressed";

    fseek(original_fp,0,SEEK_END);
    long long int size=ftell(original_fp);
    cout<<"The size of the original file is: "<<size<<" bytes"<<endl;
    rewind(original_fp);



    register unsigned char x;
    fread(&x,1,1,original_fp);
    for(long long int i=0;i<size;i++){
        number[x]++;
        fread(&x,1,1,original_fp);
    }
    rewind(original_fp);
    //Above code block counts all of the possible unique bytes on the original file and records them

	for(int *i=number;i<number+256;i++){                 
        	if(*i){
			letter_count++;
			}
    }

    ersel array[letter_count*2-1];
    register ersel *e=array;
    for(int *i=number;i<number+256;i++){                         
        	if(*i){
                e->right=NULL;
                e->left=NULL;
                e->number=*i;
                e->character=i-number;
                e++;
            }
    }
    
    sort(array,array+letter_count,erselcompare0);               
    
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
    

    for(e=array+letter_count*2-2;e>array-1;e--){
        if(e->left){
            e->left->bit=e->bit+e->left->bit;
        }
        if(e->right){
            e->right->bit=e->bit+e->right->bit;
        }
        
    }














    int current_bit_count=0;
    char *str_pointer;
    unsigned char current_byte=0,len,current_character;
    compressed_fp=fopen(&scompressed[0],"wb");
    fwrite(&letter_count,1,1,compressed_fp);
    total_bits+=8;
    // The program writes the number of different bytes
    // that we re gonna use to decode this compressed file.
    // This will be useful when we re decompressing this file later on.
    


    {
        cout<<"If you want a password write any number other then 0"<<endl
            <<"If you do not, write 0"<<endl;
        int check_password;
        cin>>check_password;
        if(check_password){
            string password;
            cout<<"Enter your password (Do not use spaces): ";
            cin>>password;
            int password_length=password.length();
            if(password_length==0){
                cout<<"You did not enter a password"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return 0;
            }
            if(password_length>255){
                cout<<"Password cannot contain more then 255 characters"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
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

    

    string str_arr[256];
    for(e=array;e<array+letter_count;e++){
        str_arr[(e->character)]=e->bit;
        len=e->bit.length();
        current_character=e->character;

        cout<<"len="<<(int)len<<endl;       //sil


        current_byte<<=8-current_bit_count;
        current_byte|=((unsigned char)(current_character>>current_bit_count));
        fwrite(&current_byte,1,1,compressed_fp);
        printf("ilki karakter: %d\n",current_byte);      //sil
        current_byte=(current_character<<(8-current_bit_count));
        current_byte>>=(8-current_bit_count);
        
        current_byte<<=8-current_bit_count;
        current_byte|=((unsigned char)(len>>current_bit_count));
        fwrite(&current_byte,1,1,compressed_fp);
        printf("ilki len: %d\n",current_byte);      //sil
        current_byte=(len<<(8-current_bit_count));
        current_byte>>=(8-current_bit_count);

        total_bits+=len+16;
        // above code blocks will write the byte and the number of bits
        // we re going to need to represent this specific byte's transformated version
        // after here we are going to write the transformed version of the number bit by bit.
        
        str_pointer=&e->bit[0];
        while(*str_pointer){
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Compression process aborted"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return 1;
            }
            if(current_bit_count==8){
                current_bit_count=0;
                fwrite(&current_byte,1,1,compressed_fp);
                current_byte=0;
            }
           str_pointer++;
        }
        
         bits+=len*e->number;
    }           
    total_bits+=bits;
    unsigned char bits_in_last_byte=total_bits%8;
    if(bits_in_last_byte){
        total_bits=(total_bits/8+1)*8;
        // from this point on total bits doesnt represent total bits but
        // instead it represents 8*number_of_bytes we are gonna use on our compressed file
    }

    fwrite(&bits_in_last_byte,1,1,compressed_fp);
    total_bits+=8;
    /* Above loop of the code is doing 2 in this order
    1- It determines number of bits that we are gonna write to the compressed file
        (this number only represents number of bytes thats going to be translated it doesn't include translation script
    2-It writes the translation script into [name of the original].compressed file and the str_arr array */

    cout<<"The size of the compressed file will be: "<<total_bits/8<<" bytes"<<endl;
    cout<<"Compressed file's size will be %"<<100*((float)total_bits/8/size)<<" of the original file"<<endl;
    if(total_bits/8>size){
        cout<<endl<<"COMPRESSED FILE'S SIZE WILL BE HIGHER THAN THE ORIGINAL"<<endl<<endl;
    }
    cout<<"If you wish to abort this process write 0 and press enter"<<endl
        <<"If you want to continue write any other number and press enter"<<endl;
    int check;
    cin>>check;
    if(!check){
        cout<<endl<<"Process has been aborted"<<endl;
        fclose(compressed_fp);
        fclose(original_fp);
        remove(&scompressed[0]);
        return 0;
    }

    
    
    fread(&x,1,1,original_fp);
    for(long long int i=0;i<bits;){
        str_pointer=&str_arr[x][0];
        while(*str_pointer){
            switch(*str_pointer){
                case '1':i++;current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':i++;current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Process has been aborted";
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return 2;
            }
            if(current_bit_count==8){
                current_bit_count=0;
                fwrite(&current_byte,1,1,compressed_fp);
                current_byte=0;
            }
            str_pointer++;
        }
        fread(&x,1,1,original_fp);
    }
    /*
    - Above code writes bytes that are translated from original file into compressed file.
    - After it writes the last bit corresponding to the number we read from the original file,
    it reads a new number from the original file */
    if(bits_in_last_byte){
        fwrite(&current_byte,1,1,compressed_fp);
    }
    fclose(compressed_fp);
    fclose(original_fp);

    cout<<"Compression is complete"<<endl;
    
}