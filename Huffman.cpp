#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;


struct ersel{
    ersel *left,*right;
    int number;
    char character;
    string bit;
};

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
}

bool erselcomparec(ersel a,ersel b){
    return a.character<b.character;
}
    


int main()
{
    int number[256];
    int bits=0,letter_count=0;
    for(int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    cout<<"Enter the name of the file you want to compress"<<endl;
    string s,scode,scompressed;
	cin>>s;
    register FILE *original_fp=fopen(&s[0],"rb"),*compressed_fp;
    if(NULL==original_fp){
        cout<<s<<" file does not exist"<<endl;
        return 0;
    }
    scode=s+".code";
    scompressed=s+".compressed";

    fseek(original_fp,0,SEEK_END);
    long long int size=ftell(original_fp);
    cout<<"The size of the original file is: "<<size<<" bytes"<<endl;
    rewind(original_fp);

    register char x;
    fread(&x,1,1,original_fp);
    for(int i=0;i<size;i++){
        number[x]++;
        fread(&x,1,1,original_fp);
    }
    rewind(original_fp);
    //Above code block counts all of the possible bytes on the original file and records them

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



    FILE *codes_fp=fopen(&scode[0],"w");
    string str_arr[256];
    for(e=array;e<array+letter_count;e++){
        bits+=e->bit.length()*e->number;
        str_arr[e->character]=e->bit;
        fprintf(codes_fp,"%d %s\n",e->character,&(e->bit)[0]);
    }
    fclose(codes_fp);
    char bits_in_last_byte=bits%8;                
    /* This part of the code is doing 2 things
    1- It determines number of bits that are gonna be written to the compressed file
        (this number only represents number of bytes thats going to be translated it doesn't include translation script
    2-It writes the translation script into [name of the original].codes file and the str_arr array */

    cout<<"The size of the compressed file will be a little more then: "<<bits/8<<" bytes"<<endl;
    if(bits/8<size){
        cout<<"Compressed file's size will be near %"<<100*((float)bits/8/size)<<" of the original file"<<endl;
    }
    cout<<"If you wish to abort this process write 0 and press enter"<<endl
        <<"If you want to continue write any number and press enter";
    int check;
    cin>>check;
    if(!check){
        cout<<endl<<"Your process has been aborted"<<endl<<"Don't forget to delete "<<scode<<" file";
        exit(0);
    }





    
    compressed_fp=fopen(&scompressed[0],"wb");
    fwrite(&bits_in_last_byte,1,1,compressed_fp);
    //The first byte of the compressed file now holds the number of bits which will be written to last byte
    //if it is 0 than we will use all of the last byte
    
    char current_byte=0,*str_pointer;
    fread(&x,1,1,original_fp);
    for(int i=0,current_bit_count=0;i<bits;){
        str_pointer=&str_arr[x][0];
        while(*str_pointer){
            switch(*str_pointer){
                case '1':i++;current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':i++;current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl;exit(EXIT_FAILURE);
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
    - Above code writes current_byte to compressed file,
    after adding the 8th bit to the current_byte it creating the next byte.
    - After it writes the last bit corresponding to the number we read from the original file,
    it reads a new number from the original file */
    if(bits_in_last_byte){
        fwrite(&current_byte,1,1,compressed_fp);
    }

    cout<<"Compression process has been done"<<endl;


}
