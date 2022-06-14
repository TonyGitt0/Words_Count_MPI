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
Per eseguire il file in locale c'è la necessità di posizionare i file .txt nello stesso path del file eseguibile.
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
> **Assunzioni**: la seguente implementazione considera l'utilizzo di 24 file di testo composti da 99 parole ripetute per un totale di 50,171 per singolo file. In totale quindi avremo 1.204.104 parole da analizzare. Tali file sono stati autogenerati da uno script Python, dove ogni parola è seguita da uno spazio (compresa l'ultima parola).
