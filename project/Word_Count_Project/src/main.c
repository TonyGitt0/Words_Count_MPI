#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stddef.h>
#include "utils.h"
#include <mpi.h>

#define TOTALWORDS 100000000
#define MAX_SPLIT 100
#define NUM_FILES 2
#define MASTER 0


int main(int argc, char *argv[])
{
    int myrank, size, count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    MPI_Barrier(MPI_COMM_WORLD);
    double time_start = MPI_Wtime();

    int tag = 1;
    long tot_words_in_files = 0;
    int numSplit = 0;
    int num_file = 0;
    size_t files = 0;
    int words_for_processor[size];
    StructWordForProcess *wordForProcessor = (StructWordForProcess *)malloc(sizeof(StructWordForProcess) * MAX_SPLIT);
    WordFreq *dictionary = (WordFreq *)malloc(sizeof(WordFreq) * TOTALWORDS);
    WordFreq *words_to_master = (WordFreq *)malloc(sizeof(WordFreq) * TOTALWORDS);

    // FIRST STRUCT
    const int n_items_fs = 4;       
    int blocklengths_fs[n_items_fs]; 
   
    MPI_Datatype dt_received, types_fs[n_items_fs];     
    MPI_Aint offsets_fs[n_items_fs], lower_bound, extent;

    offsets_fs[0] = offsetof(StructWordForProcess, name_file);
    types_fs[0] = MPI_CHAR;
    blocklengths_fs[0] = LENGHTFILENAME;

    offsets_fs[1] = offsetof(StructWordForProcess, rank);
    types_fs[1] = MPI_INT;
    blocklengths_fs[1] = 1;

    offsets_fs[2] = offsetof(StructWordForProcess, start);
    types_fs[2] = MPI_INT;
    blocklengths_fs[2] = 1;

    offsets_fs[3] = offsetof(StructWordForProcess, end);
    types_fs[3] = MPI_INT;
    blocklengths_fs[3] = 1;

    MPI_Type_create_struct(n_items_fs, blocklengths_fs, offsets_fs, types_fs, &dt_received);
    MPI_Type_commit(&dt_received);

    // SECOND STRUCT
    int const n_items_ss = 2;
    int blocklengths_ss[n_items_ss];

    MPI_Datatype d_words, types_ss[n_items_ss];
    MPI_Aint offsets_ss[n_items_ss];

    offsets_ss[0] = offsetof(WordFreq, word);
    types_ss[0] = MPI_CHAR;
    blocklengths_ss[0] = SINGLEWORD;

    offsets_ss[1] = offsetof(WordFreq, word_occurency);
    types_ss[1] = MPI_INT;
    blocklengths_ss[1] = 1;

    MPI_Type_create_struct(n_items_ss, blocklengths_ss, offsets_ss, types_ss, &d_words);
    MPI_Type_commit(&d_words);

    if (myrank == 0)
    {
        printf("\n");
        printf("START PROJECT\n");
        printf("FILE ANALYSIS\n");
        printf("------------------------------------------------\n");

        int files_number = numFilesInDirectory(".");
        printf("Number files with the extension (.txt): %d\n", files_number);

        if (argc == 2)
        {
            num_file = atoi(argv[1]);
        }
        else
        {
            MPI_Finalize();
            return 0;
        }
        printf("\n");
        char **list_files = listOfFile(".", &files, num_file);
        printf("\n");

        File specFile[files_number];
        printf("LIST OF FILES CONSIDERED\n");
        printf("------------------------------------------------\n");
        tot_words_in_files = sumAllWordsInDifferentFile(list_files, specFile, &files);
        printf("Number of words in all files: %ld for %d processes\n\n", tot_words_in_files, size);


        printf("FILE PROCESSING\n");
        printf("------------------------------------------------\n");
        numWordForProcess(words_for_processor, tot_words_in_files, size);
        for (int i = 0; i < size; i++)
        {
            printf("number of words is: %d for processor: %d\n", words_for_processor[i], i);
        }

        numSplit = setStructureWordForProcessForSplitFileForProcess(wordForProcessor, size, words_for_processor, specFile);
        printf("Number of Split: %d\n\n", numSplit);
        for (int j = 0; j < numSplit; j++)
        {
            printf("File name: %s    start-point: %d        end-point: %d         for processor:%d\n\n", wordForProcessor[j].name_file, wordForProcessor[j].start, wordForProcessor[j].end, wordForProcessor[j].rank);
        }

        printf("TEST\n");
        printf("------------------------------------------------\n");

        int index_struct = 0;
        int startForZero = 0;
        int total_new_words = 0;

        while (wordForProcessor[index_struct].rank == 0 && index_struct < numSplit)
        {
            index_struct++;
            startForZero++;
        }

        for (int rank = 1; rank < size; rank++)
        {
            int space = 0;
            int index_struct_for_process = index_struct;
            while (wordForProcessor[index_struct].rank == rank)
            {
                index_struct++;
                space++;
            }
            MPI_Send(&wordForProcessor[index_struct_for_process], space, dt_received, rank, tag, MPI_COMM_WORLD);
        }
        total_new_words = wordCount(dictionary, wordForProcessor, startForZero,size);

        for (int rank = 1; rank < size; rank++)
        {
            count = 0;
            MPI_Recv(words_to_master, TOTALWORDS, d_words, rank, tag, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, d_words, &count);

            total_new_words = concatWordCount(dictionary, words_to_master, count, total_new_words);
        }

        getDataOfWOrd(dictionary, total_new_words);
        printf("Total words (no-occurency) in all file is: %d\n", total_new_words);
    }
    else
    {
        count = 0;
        int new_word_single_processor = 0;
        MPI_Recv(wordForProcessor, MAX_SPLIT, dt_received, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, dt_received, &count);

        new_word_single_processor = wordCount(dictionary, wordForProcessor, count ,size);
        MPI_Send(dictionary, new_word_single_processor, d_words, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Type_free(&d_words);
    MPI_Type_free(&dt_received);

    MPI_Barrier(MPI_COMM_WORLD);
    if(myrank == 0)
    {
        double time_end = MPI_Wtime();
        double time = time_end - time_start;
        printf("Time: %lf\n\n", time);
    }
    MPI_Finalize();
    return 0;
}