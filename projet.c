#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define MATRIX_SIZE 24

#define NOM1_PAR_DEFAUT "seq1.txt"
#define NOM2_PAR_DEFAUT "seq2.txt"
    
/*
 * Function:  sequence_len
 * ----------------------------------------
 *   Calculates a protein sequence's length from a fasta file
 *
 *   Parameters:
 *   
 *     filename: a line of charecters representing the 
 *				 fasta file name 
 *
 * 	 Returns:
 *     
 *     n:        an integer representing the sequence length
 *     
 */
int sequence_len(char *filename) { 
    FILE *fr;
    int n = 0;
    char c;
	
    fr = fopen(filename, "r");
    if (fr == NULL) {
        perror("Error opening file");
        exit(1);
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
    printf("Calculated the length of the sequence: %d amino acids\n", n);
    return n;    
 
}
/*
 * Function:  read_sequence 
 * ----------------------------------------
 *   Reads a protein sequence from a fasta file
 *
 *   Parameters:
 *   
 *     filename: a line of charecters representing the 
 *				 fasta file name 
 *
 * 	 Returns:
 *     
 *     t:        a pointer to the table of characters with 
 *               the protein sequence.
 *     
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
    	printf("Succesfully opened the file %s\n", filename);
    }

    n = sequence_len(filename);

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
    
    printf("Read %d amino acids from the file %s\n", n, filename);
    printf("Protein sequence: ");
    
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
 *      filename: a line of charecters representing  
 *				  the file name 
 *
 *   Returns:
 *     
 *      matrix:   a pointer to the table of characters 
 *                with the protein sequence.
 *     
 */
int** read_substitution_matrix(char *filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int row = 0, col = 0, value;
    int **matrix;
    
    printf("\n");

    // memory allocation

    matrix = (int **)malloc(MATRIX_SIZE * sizeof(int *));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrix[i] = (int *)malloc(MATRIX_SIZE * sizeof(int));
    }

    // file opening

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
        printf("Successfully opened file %s\n", filename);
    }

    // skipping useless lines

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue; // skip comments and empty lines
        }
        if (row > MATRIX_SIZE) {
            break; // reached the end of the matrix
        }
        col = 0; // reset column counter for each row
        char *token = strtok(line, " ");
        while (token != NULL) {
            if (col == 0) {
                col++;
                token = strtok(NULL, " ");
                continue; // skip first column
            }
            sscanf(token, "%d", &value);
            if (row != 0) { // skip first row
                matrix[row - 1][col - 1] = value;
            }
            col++;
            token = strtok(NULL, " ");
        }
        row++;
    }

    fclose(file);
    
    printf("Read substitution matrix from file %s\n", filename);
    printf("Substitution matrix:\n");
    
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    
    return matrix;
}



/*
 * Function:  initialize_matrix 
 * ----------------------------------------
 *   Initialize a matrix 
 *
 *   Parameters:
 *   
 *          n: an integer representing the number of 
               rows of the matrix
            m: an integer representing the number of 
               columns of the matrix
        value: an integer representing the initial 
               value of the matrix
 *
 *   Returns:
 *     
 *    matrix:  a pointer to the table of characters
 *     
 */
int **initialize_matrix(int n, int m, int value) {
    // Allocate memory for the pointers to rows
    int** matrix = (int**) malloc(n * sizeof(int*));
    if (matrix == NULL) {
        printf("Error: Failed to allocate memory for matrix rows.\n");
        exit(1);
    }

    // Allocate memory for the matrix elements and initialize with value
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*) malloc(m * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Error: Failed to allocate memory for matrix elements.\n");
            exit(1);
        }
        for (int j = 0; j < m; j++) {
            matrix[i][j] = value;
        }
    }
    
    printf("\n");
    printf("Succesfully initialized the matrix %d x %d with %d value \n", n, m, value);
    return matrix;
}



int compute_score(char *a, char *b, int **substitution_matrix) {
    // Define the order of amino acids
    char amino_acids[] = "ARNDCQEGHILKMFPSTWYVBZX*";
    
    // Convert characters a and b to their serial numbers
    int index_a = -1, index_b = -1;
    for (int i = 0; i < 24; i++) {  
        if (*a == amino_acids[i]) {
            index_a = i;
        }
        if (*b == amino_acids[i]) {
            index_b = i;
        }
    }
    
    // If a or b is not a standard amino acid, return an error code
    if (index_a == -1 || index_b == -1) {
        printf("Unexpected character in sequence");
        return -1;
    }
    
    // Access the score from the substitution matrix
    int score = substitution_matrix[index_a][index_b];
    
    return score;
}



int calculate_alignment(char* seq1, char* seq2, int** substitution_matrix, int gap_penalty, int gap_extension, char** aligned_seq1, char** aligned_seq2){
    
    return 0;
}

/*
void print_alignment(char* aligned_seq1, char* aligned_seq2){
    
    return 0;
}*/

int main() {
	char *seq1, *seq2;
	int n, m, **substitution_matrix, **matrix;
	
    substitution_matrix = read_substitution_matrix("BLOSUM62.txt");
	
    seq1 = read_sequence(NOM1_PAR_DEFAUT);
    seq2 = read_sequence(NOM2_PAR_DEFAUT);
    
    n = sequence_len(NOM1_PAR_DEFAUT);
    m = sequence_len(NOM2_PAR_DEFAUT);
    
    matrix = initialize_matrix(n, m, 0);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    
    printf("\n%d \n", compute_score("W","W",substitution_matrix));
    
    free(seq1);
    free(seq2);
    free(substitution_matrix);
    
    return 0;
}
