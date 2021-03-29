#include<iostream>

struct progress{
    // this struct is here for maintaining progress bar feature
    // it doesn't have any additional function
    // It isn't neccessary for the program
    long int MAX=0,CURRENT=0,LAST=0;
    int percentage=0;
    
    void update(){
        if(CURRENT*100/MAX>percentage){
            percentage=CURRENT*100/MAX;
            BAR();
        }
    }
    void current(long int a){
        CURRENT=a;
        update();
    }
    void next(long int a){
        CURRENT+=LAST;
        LAST=a;
        update();
    }
    void BAR(){
        system("clear");
        std::cout<<'[';
        for(int i=1;i<=percentage;i++)std::cout<<'#';
        for(int i=1;i<=100-percentage;i++)std::cout<<':';
        std::cout<<"]:%"<<percentage<<std::endl;
    }
};
