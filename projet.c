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


// Function to find the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}


int calculate_alignment(char *seq1, char *seq2, int **substitution_matrix, int gap_penalty, char **aligned_seq1, char **aligned_seq2) {

    // Get the lengths of the input sequences
    int len_seq1 = sequence_len(NOM1_PAR_DEFAUT);
    int len_seq2 = sequence_len(NOM2_PAR_DEFAUT);
    

    // Initialize the scoring matrix
    int **score_matrix = initialize_matrix(len_seq1, len_seq2, 0);
    
    
    // Fill the scoring matrix using the substitution matrix and gap penalty
    int max_score = 0;
    int max_i = 0;
    int max_j = 0;
    for (int i = 1; i <= len_seq1; i++) {
        for (int j = 1; j <= len_seq2; j++) {
            int match = score_matrix[i - 1][j - 1] + substitution_matrix[seq1[i - 1] - 'A'][seq2[j - 1] - 'A'];
            int delete = score_matrix[i - 1][j] + gap_penalty;
            int insert = score_matrix[i][j - 1] + gap_penalty;
            score_matrix[i][j] = max(0, max(match, max(delete, insert)));
            if (score_matrix[i][j] > max_score) {
                max_score = score_matrix[i][j];
                max_i = i;
                max_j = j;
            }
        }
    }

    // Traceback to find the aligned sequences
    *aligned_seq1 = (char*)malloc((max_i + max_j) * sizeof(char));
    *aligned_seq2 = (char*)malloc((max_i + max_j) * sizeof(char));
    int aligned_i = max_i;
    int aligned_j = max_j;
    int aligned_len = 0;
    while (aligned_i > 0 && aligned_j > 0 && score_matrix[aligned_i][aligned_j] > 0) {
        int current_score = score_matrix[aligned_i][aligned_j];
        int diagonal_score = score_matrix[aligned_i - 1][aligned_j - 1];
        int up_score = score_matrix[aligned_i - 1][aligned_j];
        int left_score = score_matrix[aligned_i][aligned_j - 1];
        if (current_score == diagonal_score + substitution_matrix[seq1[aligned_i - 1] - 'A'][seq2[aligned_j - 1] - 'A']) {
            (*aligned_seq1)[aligned_len] = seq1[aligned_i - 1];
            (*aligned_seq2)[aligned_len] = seq2[aligned_j - 1];
            aligned_i--;
            aligned_j--;
        } else if (current_score == up_score + gap_penalty) {
            (*aligned_seq1)[aligned_len] = seq1[aligned_i - 1];
            (*aligned_seq2)[aligned_len] = '-';
            aligned_i--;
        } else {
            (*aligned_seq1)[aligned_len] = '-';
            (*aligned_seq2)[aligned_len] = seq2[aligned_j - 1];
            aligned_j--;
        }
        aligned_len++;
    }

    // Reverse the aligned sequences
    for (int k = 0; k < aligned_len / 2; k++) {
        char temp = (*aligned_seq1)[k];
        (*aligned_seq1)[k] = (*aligned_seq1)[aligned_len - 1];
        (*aligned_seq1)[aligned_len - 1 - k] = temp;
        temp = (*aligned_seq2)[k];
        (*aligned_seq2)[k] = (*aligned_seq2)[aligned_len - 1 - k];
        (*aligned_seq2)[aligned_len - 1 - k] = temp;
    }

    (*aligned_seq1)[aligned_len] = '\0'; 
    (*aligned_seq2)[aligned_len] = '\0';
    
    for (int i = 0; i < len_seq1; i++) {
        for (int j = 0; j < len_seq2; j++) {
            printf("%d ", score_matrix[i][j]);
        }
        printf("\n");
    }

    /*
    // Free the memory allocated for the scoring matrix
    for (int i = 0; i <= len_seq1; i++) {
        free(scores[i]);
    }
    free(scores);*/

    return max_score; 
}



/*
void print_alignment(char* aligned_seq1, char* aligned_seq2){
    
    return 0;
}*/

int main() {
	char *seq1, *seq2, **aligned_seq1, **aligned_seq2;
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
    
    printf("\n%d \n",calculate_alignment(seq1, seq2, substitution_matrix, -1, aligned_seq1, aligned_seq2));
    
    free(seq1);
    free(seq2);
    free(substitution_matrix);
    
    return 0;
}
