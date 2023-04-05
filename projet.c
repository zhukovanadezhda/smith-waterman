#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
        perror("Error: cannot open file");
        exit(1);
    }
    
    while (fscanf(fr, "%c", &c) != EOF) {
        if (c == '>') {
            while (fscanf(fr, "%c", &c) != EOF && c != '\n') {
            }
        }
        else {
            if (isalpha(c)) { // Only count non-newline characters as amino acids
                n++;
            }
        }
    }

    printf("Calculated the length of the sequence: %d amino acids\n", n);
    fclose(fr);
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
        perror("Error: cannot open file");
        exit(1);
    } else {
    	printf("Succesfully opened the file %s\n", filename);
    }

    // Determine the length of the protein sequence
    n = sequence_len(filename);

    // Allocate memory for the protein sequence
    t = malloc((n+1) * sizeof(char));
    if (t == NULL) {
        perror("Error: cannot allocate memory");
        exit(1);
    }

    // Reset the file pointer and read the protein sequence
    rewind(fr);

    while (fscanf(fr, "%c", &c) != EOF) {
        if (c == '>') {
            // Skip the header line
            while (fscanf(fr, "%c", &c) != EOF && c != '\n') {
            }
        }
        else {
            if (isalpha(c)) { // Only store non-newline characters as amino acids
                t[i] = c;
                i++;
            }
        }
    }
    
    t[n] = '\0'; // Add a null terminator to the end of the sequence
    printf("Read %d amino acids from the file %s\n", n, filename);  
    printf("Protein sequence: %s\n", t);

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
int **read_substitution_matrix(char *filename) {
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
        perror("Error: cannot open file");
        exit(1);
    } else {
        printf("Successfully opened the file %s\n", filename);
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
    
    printf("Read substitution matrix from the file %s\n", filename);
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
        printf("Error: Failed to allocate memory for matrix rows\n");
        exit(1);
    }

    // Allocate memory for the matrix elements and initialize with value
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*) malloc(m * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Error: Failed to allocate memory for matrix elements\n");
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
        printf("\nError: Unexpected character in the sequence\n");
        return -1;
    }
    
    // Access the score from the substitution matrix
    int score = substitution_matrix[index_a][index_b];
    
    return score;
}


int **fill_matrix(char *seq1, char *seq2, int **substitution_matrix, int gap_penalty) {
  
    // Get sequence lengths
    int len1 = strlen(seq1);
    int len2 = strlen(seq2);

    // Initialize score matrix
    int **score_matrix = initialize_matrix(len1 + 1, len2 + 1, 0);

    // Fill score matrix
    int i, j, max_score;
    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            // Compute score for each possible path
            int match_score = score_matrix[i-1][j-1] + compute_score(&seq1[i-1], &seq2[j-1], substitution_matrix);
            int delete_score = score_matrix[i-1][j] + gap_penalty;
            int insert_score = score_matrix[i][j-1] + gap_penalty;

            // Take maximum score among paths, or 0 if negative
            max_score = match_score;
            if (delete_score > max_score) max_score = delete_score;
            if (insert_score > max_score) max_score = insert_score;
            if (max_score < 0) max_score = 0;
            
            // Update score matrix
            score_matrix[i][j] = max_score;
        }
    }
    printf("\nSuccesfully calculated the score matrix :\n");
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            printf("%d ", score_matrix[i][j]);
        }
        printf("\n");
    }
    return score_matrix;
}




/*
void print_alignment(char* aligned_seq1, char* aligned_seq2){
    
    return 0;
}*/

           
int main() {
    char *seq1, *seq2;
    int **substitution_matrix;
    int gap_penalty = -2;

    // Read sequences and substitution matrix from files
    seq1 = read_sequence("seq1.txt");
    seq2 = read_sequence("seq2.txt");
    substitution_matrix = read_substitution_matrix("BLOSUM62.txt");
    

    // Fill score matrix using Smith-Waterman algorithm
    int **score_matrix = fill_matrix(seq1, seq2, substitution_matrix, gap_penalty);



    // Free memory
    free(seq1);
    free(seq2);
    free(substitution_matrix[0]);
    free(substitution_matrix);
    free(score_matrix[0]);
    free(score_matrix);

    return 0;
}

