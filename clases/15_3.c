#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Ejercicio de tipo competitivo, 
char* nombre( char* name){
    char* vocales="aeiou";
    char* aux=(char*)malloc((sizeof(char)*(strlen(name))));
    int x=0;
    for(int i=0;i<5;i++){
        if(name[strlen(name)-1]==vocales[i]){
           while(x<(strlen(name)-1)){
                aux[x]=name[x];
                x++;
           }
            aux[x]='i';
            aux[x+1]='c';
            aux[x+2]=name[strlen(name)-1];
        }
    }
    for(int j=0;j<strlen(name)+2;j++){
        printf("%c",aux[j]);
    }
}
int main(){
    char* name= "roberto";
    nombre("roberto");
    nombre("");
}
*/

/*ejercicio recursivo -> escribir una palabra al revés*/

char* name(char* word){
    if(word[strlen(word)-1]=='\0'){
        return "/0";
    }
    char* aux=(char*)malloc(sizeof(char*)*strlen(word));
    for(int i=0;i<(strlen(word)-1);i++){
        aux[i]=word[i];
    }
    char cadenaTemporal[2];
    cadenaTemporal[0]=word[strlen(word)-1];
    printf("%s",cadenaTemporal);
    return name(aux);
}
int main(void){
   name("mariposa");
}
