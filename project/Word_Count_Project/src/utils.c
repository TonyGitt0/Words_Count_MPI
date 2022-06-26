#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"
#define MAXWORDS 10000


int sumAllWordsInDifferentFile(char **list_files, File *singleFile, size_t *elem)
{
    int sum_all_words = 0, file = 0;
    char **local_list_file = list_files;
    for (int i = 0; i < *elem; i++)
    {
        printf("name file add to struct: %s\n", local_list_file[i]);
        strcpy(singleFile[file].name_file, local_list_file[i]);
        singleFile[file].tot_words = numWordInFile(local_list_file[i]);
        sum_all_words = sum_all_words + singleFile[file].tot_words;
        file++;
    }
    return sum_all_words;
}

char **listOfFile(char *directory_path, size_t *elems, int num_files)
{
    DIR *directory = opendir(directory_path);
    struct dirent *entity;

    char **list_files = (char **)malloc(num_files * sizeof(char)); 
    int i = 0;

    if (directory != NULL)
    {
        while ((entity = readdir(directory)) != NULL)
        {
            if (entity->d_type == DT_REG)
            {
                const char *ext = strrchr(entity->d_name, '.');
                if ((ext != NULL) && (!strcmp(ext + 1, "txt")) && i < num_files)
                {
                    i++;
                    list_files = realloc(list_files, sizeof(*list_files) * (*elems + 1));
                    list_files[(*elems)++] = strdup(entity->d_name);
                }
            }
        }
        closedir(directory);
    }
    else
    {
        printf("Directory can't be opened\n");
        return list_files;
    }
    return list_files;
}


int numFilesInDirectory(char *directory_path)
{
    DIR *directory = opendir(directory_path);
    struct dirent *entity;

    int num_files = 0;
    if (directory != NULL)
    {
        while ((entity = readdir(directory)) != NULL)
        {
            char *fileName = entity->d_name;
            if (entity->d_type == DT_REG)
            {
                const char *ext = strrchr(entity->d_name, '.');
                if ((ext != NULL) && (!strcmp(ext + 1, "txt")))
                {
                    num_files++;
                }
            }
        }
        closedir(directory);
    }
    else
    {
        printf("Directory can't be opened\n");
        return -1;
    }
    return num_files;
}


int numWordInFile(char *name_file)
{
    FILE *fPointer;
    fPointer = fopen(name_file, "r");

    if (fPointer == NULL)
    {
        printf("File can't be opened\n");
        fclose(fPointer);
    }
    int char_count = 0, word_count = 0, line_count = 1;
    int in_word = 0;
    char ch;
    while (!feof(fPointer))
    {
        ch = fgetc(fPointer);
        char_count++;

        if (ch == ' ' || ch == '\t' || ch == '\0' || ch == '\n')
        {
            if (in_word)
            {
                in_word = 0;
                word_count++;
            }
            if (ch = '\0' || ch == '\n')
            {
                line_count++;
            }
        }
        else
        {
            in_word = 1;
        }
    }
    // printf("In the file %s:\n", name_file);
    // printf("Number of characters: %d.\n", char_count);
    // printf("Number of words: %d.\n", word_count);
    // printf("Number of lines: %d.\n\n", line_count);
    // fclose(fPointer);

    return word_count;
}


int setStructureWordForProcessForSplitFileForProcess(StructWordForProcess *wordsForProcess, int num_proc, int *num_words, File *File)
{
    int my_rank = 0;
    int element_in_struct = 0;
    int file = 0;

    int start = 0;

    while (my_rank < num_proc)
    {
        wordsForProcess[element_in_struct].rank = my_rank;
        wordsForProcess[element_in_struct].start = start;

        int dif = num_words[my_rank] - (File[file].tot_words - start);
        if (dif >= 0)
        {
            num_words[my_rank] = num_words[my_rank] - File[file].tot_words + start;
            wordsForProcess[element_in_struct].end = File[file].tot_words - 1;
            strcpy(wordsForProcess[element_in_struct].name_file, File[file].name_file);
            start = 0;
            element_in_struct++;
            file++;
            if (num_words[my_rank] == 0)
            {
                my_rank++;
            }
        }
        else
        {
            wordsForProcess[element_in_struct].end = num_words[my_rank] + start - 1;
            start = wordsForProcess[element_in_struct].end + 1;
            num_words[my_rank] = 0;
            strcpy(wordsForProcess[element_in_struct].name_file, File[file].name_file);
            my_rank++;
            element_in_struct++;
        }
    }
    return element_in_struct;
}


void numWordForProcess(int *num_words, long tot_words, int num_proc)
{
    int module = (tot_words % num_proc);

    if (module == 0)
    {
        for (int i = 0; i < num_proc; i++)
        {
            num_words[i] = tot_words / num_proc;
        }
    }
    else
    {
        for (int i = 0; i < num_proc; i++)
        {
            num_words[i] = tot_words / num_proc;
        }
        int end_module = 0;
        while (module != 0)
        {
            int module2 = end_module % num_proc;
            num_words[module2] = num_words[module2] + 1;
            module--;
            end_module--;
        }
    }
}


int isWordNew(WordFreq *dictionary, char *string, int new_words, int num_proc)
{
    int i;
    if (num_proc == 1)
    {
        for (i = 0; i < new_words+1; i++)
        {
            if (strcmp(dictionary[i].word, string) == 0)
                return i;
        }
        return -3;
    }

    for (i = 0; i < new_words; i++)
    {
        if (strcmp(dictionary[i].word, string) == 0)
            return i;
    }
    return -3;
}

int whichWord(WordFreq **dictionary, char *string, int new_words)
{
    int i;
    for (i = 0; i < new_words; i++)
    {
        if (strcmp(dictionary[i]->word, string) == 0)
            return i;
    }
    return 0;
}

/*
char **split(char *allWords, const char *delim)
{
    char *aux;
    char *p;
    char **res;
    char *all_word[MAXWORDS];
    int n = 0, i;

    assert(aux = strdup(allWords));
    for (p = strtok(aux, delim); p; p = strtok(NULL, delim))
    {
        allWords[n++] = p;
    }
    allWords[n++] = NULL;
    allWords[n++] = aux;

    assert(res = calloc(n, sizeof(char *)));
    for (i = 0; i < n; i++)
    {
        res[i] = allWords[i];
    }

    return res;
}
*/

int wordCount(WordFreq *dictionary, StructWordForProcess *structWord, int count, int num_proc)
{
    int line = 0;
    int index_word = 0;
    int new_word_vector = 0;
    int lineToCompare = 0;
    int line_end;

    for (int i = 0; i < count; i++)
    {
        FILE *fPointer = fopen(structWord[i].name_file, "r");
        char allWords[LENGHTLINE];

        if (fPointer == NULL)
        {
            printf("FILE ERROR\n");
            return -1;
        }
        while ((fgets(allWords, sizeof(allWords), fPointer)) != NULL)
        {
            line++;
            if (line >= structWord[i].start && line <= structWord[i].end)
            {
                lineToCompare = (structWord[i].end - structWord[i].start);
                strtok(allWords, "\n");
                char *p = strtok(allWords, " ");
                while (p != NULL)
                {
                    
                    if ((index_word = isWordNew(dictionary, allWords, lineToCompare, num_proc)) == -3)
                    {
                        strcpy(dictionary[new_word_vector].word, allWords);
                        dictionary[new_word_vector].word_occurency = 1;
                        new_word_vector++;
                    }
                    else
                    {
                        dictionary[index_word].word_occurency++;
                    }
                    p = strtok(NULL, " ");
                }
            }
        }
        line = 0;
        fclose(fPointer);
    }
    return new_word_vector;
}

extern int concatWordCount(WordFreq *dictionary, WordFreq *received_words, int count, int total_new_words)
{

    int j;

    for (int i = 0; i < count; i++)
    {
        for (j = 0; j < total_new_words; j++)
        {
            if (strcmp(received_words[i].word, dictionary[j].word) == 0)
            {
                dictionary[j].word_occurency += received_words[i].word_occurency;
                break;
            }
        }
        if (j == total_new_words)
        {
            strcpy(dictionary[j].word, received_words[i].word);
            dictionary[j].word_occurency = received_words[i].word_occurency;
            total_new_words++;
        }
    }
    return total_new_words;
}

void getDataOfWOrd(WordFreq *dictionary, int total_new_words)
{
    int check = 0;
    FILE *result_cvs;


    result_cvs = fopen("./result_csv","w+");
    fprintf(result_cvs,"word,occurency\n");
    printf("\n");
    for (int n = 0; n < total_new_words; n++)
    {
        fprintf(result_cvs,"%s,%d\n", dictionary[n].word, dictionary[n].word_occurency);
        check += dictionary[n].word_occurency;
    }
    printf("\n");
    printf("CHECK WORD OCCURENCY: %d", check + 1);
    printf("\n");
}

