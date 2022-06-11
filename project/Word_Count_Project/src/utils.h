#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#define LENGHTFILENAME 20
#define LENGHTLINE 300
#define SINGLEWORD 20


//STRUCTURE
typedef struct {
    char name_file[LENGHTFILENAME];
    int rank;
    int start;
    int end;
}StructWordForProcess;

typedef struct {
    char name_file[LENGHTFILENAME];
    int tot_words;
}File;

typedef struct{
    char word[SINGLEWORD];
    int word_occurency;
}WordFreq;


//TESTED METHOD
extern int numFilesInDirectory(char *path);
extern int numWordInFile(char *name_file);
extern char **listOfFile(char *directory_path,size_t *elems,int num_file);
extern void numWordForProcess(int *num_words, long tot_words, int num_proc);
extern int sumAllWordsInDifferentFile(char **list_files, File *singleFile,size_t *elem);
extern int setStructureWordForProcessForSplitFileForProcess(StructWordForProcess *wordsForProcess, int num_proc, int *num_words, File *File);


extern int isWordNew(WordFreq* dictionary, char* string, int new_words,int num_proc);
extern int whichWord(WordFreq** dictionary, char* string, int new_words);
extern char **split (char *allWords, const char *delim);
extern int wordCount(WordFreq* dictionary, StructWordForProcess* wordForProcessor,int count,int num_files,int num_proc);
extern int concatWordCount(WordFreq* dictionary,WordFreq* received_words,int count,int total_new_word);
extern void getDataOfWOrd(WordFreq* dictionary,int total_new_words);