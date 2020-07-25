#include <iostream>
#include <cstdio>
using namespace std;

int main(){
    int size;
    string s;
    cout<<"Enter file name"<<endl;
    cin>>s;
    FILE *fp=fopen(&s[0],"rb");
    if(!fp){
        cout<<s<<" file does not exist"<<endl;
        return 0;
    }
    fseek(fp,0,SEEK_END);
    size=ftell(fp)/sizeof(char);
    cout<<"Size of file named \""<<s<<"\" is: "<<size<<" bytes"<<endl;
    rewind(fp);
    fclose(fp);  
}
