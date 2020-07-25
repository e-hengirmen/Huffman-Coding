#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
using namespace std;

struct ersel{
    ersel *left,*right;
    int number;
    char character;
    string bit;
    int len;
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
    long long int size=ftell(original_fp)/sizeof(char);
    cout<<"size of the original file: "<<size<<" bytes"<<endl;                  //delete later
    rewind(original_fp);

    register char x;
    fread(&x,sizeof(char),1,original_fp);
    for(int i=0;i<size;i++){
        if(x==10)printf("%d, 10\n",i+1);
        number[x]++;
        fread(&x,sizeof(char),1,original_fp);
    }
    rewind(original_fp);

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
    for(e=array;e<array+letter_count;e++){
        e->len=e->bit.length();
        bits+=e->len*e->number;
        fprintf(codes_fp,"%d %s\n",e->character,&(e->bit)[0]);
    }
    fclose(codes_fp);
    char bits_in_last_byte=bits%8;                
    







    
    compressed_fp=fopen(&scompressed[0],"wb");
    fwrite(&bits_in_last_byte,sizeof(char),1,compressed_fp);     
    //The first byte of the compressed file now holds the number of bits which will be written to last byte
    //if it is 0 than we will write use all of the last byte
    
    char current_byte=0,*str_pointer;
    fread(&x,sizeof(char),1,original_fp);
    for(int i=0,j=0;i<size-bits_in_last_byte;/*dont forget*/){
        str_pointer=&(array[x].bit);
        while(j<8){
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;j++;break;
                case '0':current_byte<<=1;j++;break;
                case  0 :break;
            }
        }
        
        //TODO
        
        
        /*after adding the 8th bit write the current_byte to compressed file then take the start writing the next byte
        When you write the last bit of the corresponding number take a new number*/


        /*translate the code into compressed file
        first bit should have the number of bits to be written in the last byte*/
        fread(&x,sizeof(char),1,original_fp);
    }

    for(int i=0;i<bits_in_last_byte;/*dont forget*/){
        
        //TODO
        /*translate the code into compressed file
        first bit should have the number of bits to be written in the last byte*/
        fread(&x,sizeof(char),1,original_fp);
    }


}
