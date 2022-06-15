# Words_Count_MPI - Esame Programmazione Concorrente Paeallela e sul Cloud 

Studente: Antonio Fasulo

Matricola: 0522501312

ID Project 01312 % 5 == 2 

## Indice
* [Introduzione](#introduzione)
* [Istruzioni per l'esecuzione](#istruzioni-per-lesecuzione)
	* [Compilazione](#compilazione)
	* [Esecuzione Locale](#esecuzione-locale)
	* [Esecuzione sul Cluster](#esecuzione-sul-cluster)
* [Descrizione della Soluzione](#descrizione-della-soluzione)
	* [File Analysis](#file-analysis)
	* [File Processing](#file-processing)
	* [Test](#test)


## Introduzione
**The Word Count**, consente di contare il numero di parole in un documento o testo. Il conteggio delle parole può essere necessario quando un testo deve contenere un numero specifico di parole. Questo caso è prevalentemente presente nel mondo accademico, nei procedimenti legali, nel giornalismo e nella pubblicità. 
Il conteggio delle parole è comunemente usato dai traduttori per determinare il prezzo di un lavoro di traduzione. Questo approccio può anche essere usato per calcolare misure di leggibilità e per misurare la velocità di battitura e di lettura (di solito in parole al minuto). L'obiettivo di tale lavoro è quello di riprodurre il problema del conteggio delle parole utilizzando MPI (Message Passing Interface), che è di fatto uno standard per la comunicazione tra nodi appartenenti a un cluster di macchine che eseguono un programma parallelo multi-nodo.


## Istruzioni per l'esecuzione
### Compilazione
La compilazione viene eseguita con due comandi separati, in quanto affinché il progetto funzioni, utilizzeremo un file di appoggio in cui sono state implementare tutte le funzioni utili al obiettivo finele.
```
mpicc -c -g main.c utils.c
```
```
mpicc -o <nome-file-eseguibile> main.o utils.o
```
### Esecuzione Locale 
Per eseguire il file in locale c'è la necessità di posizionare i file .txt nella stessa directory del file eseguibile.
```
mpirun --allow-run-as-root -np <np> <nome-file-esguibile> <num_files>
```
### Esecuzione sul Cluster 
Per eseguire il programma su cluster i file .txt dovranno essere presenti sia sul nodo MASTER che su tutti gli altri nodi SLAVE, oltre ai file anche l'eseguibile deve essere passato agli SLAVE.
```
mpirun --allow-run-as-root -np <np> --hostfile <host> <nome-file-esguibile> <num_files>
```
Sostituisci:
- **np**: numero di processori;
- **num_files**: numero di file da voler analizzare;
- **nome-file-eseguibile**: il nome del file eseguibile che creiamo;
- **host**: il path dell'hostfile.

## Descrizione della Soluzione
> **Assunzioni**: la seguente implementazione considera l'utilizzo di 24 file di testo composti da 99 parole ripetute per un totale di 50,171 per singolo file. In totale quindi avremo 1.204.104 parole da analizzare. Tali file sono stati autogenerati da uno script Python, dove ogni parola è seguita da uno spazio (compresa l'ultima parola). Il nodo MASTER parteciperà con gli SLAVE alla computazione.

Inizialmente, prima che il processo di Word Count abbia inizio, abbiamo bisogno di inizializzare due strutture importanti: `dt_recived` e `d_words`. La prima struttura viene utilizzata per inviare dal MASTER i dati relativi al file che dovrà leggere un determinato SLAVE. I dati che verranno inseriti in tale struttura sono i seguenti:
1. **il nome file, indica il nome del file analizzato;
3. **il rank, indica il processore a cui sarà assegnata l'analisi del file considerato;
5. **il punto di start, indica l'inizio da cui il processore dovrà analizzare il file considerato;
6. **il punto di end, indica la fine in cui il processore terminerà di analizzare il file considerato;

```c
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
```

## File Analysis
Una volta create queste due strutture il nodo MASTER inizia la fase di ANALYSIS sui relativi file .txt. Tale analisi comprende il controllo dell'inserimento su riga di comando del numero di file da voler analizzare e mostra la lista dei file presi in considerazione ed il numero di parole totali che compongono questi. 


## File Processing
La fase di ANALYSIS è seguita dalla fase di PROCECSSING dei file. Durante la fase di PROCECSSING attraverso la funzione `numWordsForProcess` indichiamo il numero di parole che saranno destinate al n-esimo processore, mentre, con la funzione `setStructureWordForProcessForSplitFileForProcess` rempiamo la struttura `StructWordForProcess`.


Nella funzione `numWordsForProcess` passeremo come parametri: num_words che rappresenta l'array contenete le parole per ogni processore, tot_words che rappresente il totale delle parole distribuite tra i diversi file considerati e num_proc che rappresenta il numero di processori che elaboreranno i file.
```c
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
```


Sulla seconda funzione ci soffermeremo un secondo in quanto risulta essere fondamentale per gli obiettivi del Word Count. Tale funzione prende in input la struttura vuota `StructWordForProcess`, il numero di processori utilizzato, l'array riempito con la funzione precedente contente il numero di parole per l'n-esimo processore e la struttura `specFile` che è stata settata nella fase di analisi attraverso il conteggio totale delle parole presenti in tutti i file. Inizialmente controlliamo se le parole contenute nell'array, relative all'n-esimo  processore, non siano minori di 0 una volta sottratte a queste il numero di parole contenute nel file (da cui sottraiamo altre parole `start` che potrebbero essere state già assegnate a qualcun altro). Se tale differenza risulta essere positiva, sottraiamo dall'array le parole del file aggiungendo a queste le parole indicate da `start` (consideriamo il caso in cui il processor debba leggere dalla metà del file) e rempiamo i campi della struttura. Fatto ciò incrementiamo:
1. **il numero di elmenti presenti nella struct;
2. **il numero di file (passiamo al file successivo,in quanto terminate le parole disponibili da questo);
3. **e verifichiamo che l'e-nesimo processore possa analizzare altre parole;

Diversamente, se tale differenza risulta essere negativa significa che il file non riesce ad analizzare, per motivi di capienza, le restati parole dell'n-esimo file. Impostiamo per quel determinato elemento nella struct la variabile `end` (che indicherà le parole che il file riesce ad analizzare), impostiamo la variabile `start` ad una parola dopo `l'end` dell'elemento precedente e settiamo a zero il numero di parole che il processo precedente può analizzare (in quanto esaurite). Successivamente incrementiamo il numero di elmenti nella struttura e il vaolore `my_rank` che indica il processore che effettuerà l'elaborazione.

Il metodo ritorna il numero di split che sono stati effettuati tra il numero di parole dei diversi file e i processori.
```c
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

```
L'atto finale della fase di PROCECSSING consente di mostrare a video come le parole dei diversi file verranno suddivise tra gli n processori. L'ultima fase è la fase di TEST.

## Test
