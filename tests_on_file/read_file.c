#include <stdio.h>
#include <string.h> 
#include <dirent.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    char copied_text;
    DIR* dir = opendir("C:\\Users\\anton\\OneDrive\\Desktop\\Word_Count_Project\\file_text");
    FILE* fPointer;
    long int res;

    if(dir == NULL){
        return 1;
    }
    
    struct dirent* entity;
    
    while((entity=readdir(dir)) != NULL){
            char* fileName = entity->d_name;
            if(entity->d_type == DT_REG){
                printf("Name file :%s\n", fileName);
                fPointer = fopen(fileName,"r");
                if(fPointer == NULL){
                    printf("File can't be opened\n");
                    fclose(fPointer);
                }
                char ch;
                int char_count = 0, word_count = 0, line_count = 1;
                int in_word = 0;
                while (!feof(fPointer)){
                    ch = fgetc(fPointer);
                    char_count++;

                    if(ch == ' ' || ch == '\t' || ch == '\0' || ch =='\n'){
                        if(in_word){
                            in_word = 0;
                            word_count++;
                        }
                         if(ch = '\0' || ch == '\n'){
                            line_count++;
                        }
                    }else {
                        in_word = 1;
                    }
                }
                //fseek(fPointer,0L,SEEK_END);
                //res = ftell(fPointer); 
                //printf("Size file : %d\n",res);
                printf("In the file %s:\n", fileName);
                printf("Number of characters: %d.\n", char_count);
                printf("Number of words: %d.\n", word_count);
                printf("Number of lines: %d.\n\n", line_count);
                fclose(fPointer);
            }
            fflush(stdin);
            fflush(stdout);
    }
   
    closedir(dir);
    return 0;
}

  