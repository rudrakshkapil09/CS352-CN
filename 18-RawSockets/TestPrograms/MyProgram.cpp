#include"header.h"
int main()
{
    FILE * fp=fopen("Trial.txt","r+");
    int d1;
    int d2;
    char Buf[20];
    for(int i=0;i<3;i++)
    {
        int x=fscanf(fp,"%d %s %d\n", &d1,Buf,&d2);
        strcpy(Buf,"");
        cout<<x<<endl;
    }
    int fx=ftell(fp);
    fseek(fp,fx,SEEK_SET);
    fprintf(fp,"%d   ",1234);
    cout<<"Done \n";
}