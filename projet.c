#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
#define NOM1_PAR_DEFAUT "seq1.txt"
#define NOM2_PAR_DEFAUT "seq2.txt"

 
/*
 * Function:  read_sequence 
 * ----------------------------------------
 *   Reads a protein sequence from a fasta file
 *
 *   Parameters:
 *   
 *     filename: the line of charecters representing the 
 *				 fasta file name 
 *
 * 	 Returns:
 *     
 *     The pointer to the table of characters with the protein 
 *     sequence.
 */
char *read_sequence(char *filename) {
    FILE *fr;
    int n = 0, i = 0;
    char *t, c;
    
	printf("\n");
	
    fr = fopen(filename, "r");
    if (fr == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
    	printf("Succesfully opened file %s\n", filename);
    }

    while (fscanf(fr, "%c", &c) != EOF) {
        if (c == '>') {
            while (fscanf(fr, "%c", &c) != EOF && c != '\n') {
            }
        }
        else {
            n++;
        }
    }

    t = malloc(n * sizeof(char));
    if (t == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

    rewind(fr);

    i = 0;
    while (fscanf(fr, "%c", &c) != EOF) {
        if (c == '>') {
            while (fscanf(fr, "%c", &c) != EOF && c != '\n') {
            }
        }
        else {
            t[i] = c;
            i++;
        }
    }
    
    printf("Read %d characters from file %s\n", n, filename);
    printf("DNA sequence: ");
    
    for (i = 0; i < n; i++) {
        printf("%c", t[i]);
    }
    printf("\n");

    fclose(fr);

    return t;
}


/*
 * Function:  read_substitution_matrix 
 * ----------------------------------------
 *   Reads a substitution matrix from the file
 *
 *   Parameters:
 *   
 *     filename: the line of charecters representing  
 *				 the file name 
 *
 * 	 Returns:
 *     
 *     The pointer to the table of characters with the  
 *     protein sequence.
 */
int **read_substitution_matrix(char *filename){
    FILE *fr;
    int n = 0, i = 0, j = 0;
    char *blosum62[21][21], c;
    
    printf("\n");
	
    fr = fopen(filename, "r");
    if (fr == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
    	printf("Succesfully opened file %s\n", filename);
    }
    
    while (fscanf(fr, "%c", &c) == '#') {
        while (fscanf(fr, "%c", &c) != EOF && c != '\n');
    }
	
    for (i = 0; i < 21; i++) {
        for (j = 0; j < 21; j++) {
            fscanf(fr, "%c", &c	);
            if (c != ' '){
            	blosum62[i][j] = c;
            }
        }
    }
    
    printf("Read substitution matrix from file %s\n", filename);
    printf("Substitution matrix: ");
    
    for (i = 0; i < 21; i++) {
    	for (i = 0; i < 21; i++) {
        	printf("%c", blosum62[i][j]);
        }
    }
    printf("\n");

    fclose(fr);
    
    return blosum62;
}
/*
int** initialize_matrix(int n, int m, int value){
    
    return 0;
}

int compute_score(char *a, char *b, int** substitution_matrix){
    
    return 0;
}


int calculate_alignment(char* seq1, char* seq2, int** substitution_matrix, int gap_penalty, int gap_extension, char** aligned_seq1, char** aligned_seq2){
    
    return 0;
}

void print_alignment(char* aligned_seq1, char* aligned_seq2){
    
    return 0;
}*/

int main() {
	char *seq1, *seq2, *substitution_matrix;
	
	substitution_matrix = read_substitution_matrix("BLOSUM62.txt");
	
    seq1 = read_sequence(NOM1_PAR_DEFAUT);
    seq2 = read_sequence(NOM2_PAR_DEFAUT);
    
    free(seq1);
    free(seq2);
    
    return 0;
}
