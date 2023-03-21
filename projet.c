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
    int i = 0, j = 0, col = 0, row = 0, **blosum62 = NULL;
    char c, line[1024];
    
    printf("\n");
	
    fr = fopen(filename, "r");
    if (fr == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
    	printf("Succesfully opened file %s\n", filename);
    }
    
    blosum62 = (int**) malloc(100 * sizeof(int*));
    for (int i = 0; i < 100; i++) {
        blosum62[i] = (int*) malloc(100 * sizeof(int));
        }
    
    
    if (fscanf(fr, "%c", &c) != EOF && c == '#') {
        while (fscanf(fr, "%c", &c) != EOF && c != '\n');
    }
	
	while (fscanf(fr, "%c", &c) != EOF && c != '\n');
	
	
    row = 0;
    while (fgets(line, 1024, fr) != NULL && row < 20) {
        col = 0;
        for (int i = 0; line[i] != '\0' && col < 20; i++) {
            c = line[i];
            if (c == '#') {
                break;
            } else if (c == ' ' || c == '\t') {
                continue;
            } else if (c >= 'A' && c <= 'Z' && col == 0) {
                continue;
            } else if (c >= 'A' && c <= 'Z' && col > 0) {
                blosum62[row][col-1] = atoi(&line[i+1]);
                col++;
            } else if (c == '\n') {
                continue;
            } else {
                blosum62[row][col] = atoi(&line[i]);
                col++;
            }
        }
        row++;
    }
    
    printf("Read substitution matrix from file %s\n", filename);
    printf("Substitution matrix: ");
    
    for (i = 0; i < 20; i++) {
    	for (i = 0; i < 20; i++) {
        	printf("%d", blosum62[i][j]);
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
	//char *seq1, *seq2;
	int **substitution_matrix;
	
	substitution_matrix = read_substitution_matrix("BLOSUM62.txt");
	
    //seq1 = read_sequence(NOM1_PAR_DEFAUT);
    //seq2 = read_sequence(NOM2_PAR_DEFAUT);
    
    //free(seq1);
    //free(seq2);
    free(substitution_matrix);
    
    return 0;
}
