//////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1000
#define MATRIX_SIZE 24
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////sequence_len////////////////////////////////////////////////////////   
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
        perror("Error: cannot open file\n");
        exit(1);
    }
    
    while (fscanf(fr, "%c", &c) != EOF) {
        if (c == '>') {
            while (fscanf(fr, "%c", &c) != EOF && c != '\n') { // skip the line
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


//////////////////////////////////read_sequence//////////////////////////////////////////////  


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
        perror("Error: cannot open file\n");
        exit(1);
    } else {
    	printf("Succesfully opened the file %s\n", filename);
    }

    // Determine the length of the protein sequence
    n = sequence_len(filename);

    // Allocate memory for the protein sequence
    t = malloc((n+1) * sizeof(char));
    if (t == NULL) {
        perror("Error: cannot allocate memory\n");
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
    
    t[n] = '\0'; // add the end of the sequence
    printf("Read %d amino acids from the file %s\n", n, filename);  
    printf("Protein sequence: %s\n", t);

    fclose(fr);

    return t;
}


////////////////////////////////read_substitution_matrix///////////////////////////////  


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
        perror("Error: cannot open file\n");
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
    //printf("Substitution matrix:\n");
    
    /*for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }*/
    
    return matrix;
}


////////////////////////////////////initialize_matrix///////////////////////////////////////


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
 
    printf("Succesfully initialized the matrix %d x %d with %d value \n", n, m, value);
    return matrix;
}


///////////////////////////////////////compute_score/////////////////////////////////////////


/*
 * Function:  compute_score
 * ----------------------------------------
 *   Takes a substitution score from the substitution matrix
 *	 for two given characters
 *
 *   Parameters:
 *   
 *     *a:                    a pointer to the first character 
 *	   *b:                    a pointer to the second character 	
 *     **substitution_matrix: a pointer to the substitution matrix 	  
 *
 * 	 Returns:
 *     
 *     score:  an integer representing the substitution score
 *     
 */
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
    
    // If a or b is not a standard amino acid, return an error 
    if (index_a == -1 || index_b == -1) {
        printf("\nError: Unexpected character in the sequence\n");
        return -1;
    }
    
    // Access the score from the substitution matrix
    int score = substitution_matrix[index_a][index_b];
    
    return score;
}


///////////////////////////////////////fill_matrix/////////////////////////////////////////


/*
 * Function:  fill_matrix
 * ----------------------------------------
 *   Fills the score matrix with the score for each character
 *
 *   Parameters:
 *   
 *     *seq1:                 a pointer to the first sequence
 *	   *seq2:                 a pointer to the second sequence 	
 *     **substitution_matrix: a pointer to the substitution matrix 	 
 *     gap_penalty:           an integer representing gap penalty
 *
 * 	 Returns:
 *     
 *     **score_matrix:        a pointer to the score matrix 	
 *     
 */
int **fill_matrix(char *seq1, char *seq2, int **substitution_matrix, int gap_penalty) {
  
    // Get sequence lengths
    int len1 = strlen(seq1);
    int len2 = strlen(seq2);

    // Initialize the score matrix
    int **score_matrix = initialize_matrix(len1 + 1, len2 + 1, 0);

    // Fill the score matrix
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
            
            // Update the score matrix
            score_matrix[i][j] = max_score;
        }
    }
    printf("Succesfully calculated the score matrix %d x %d \n", len1 + 1, len2 + 1);
    /*for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            printf("%d ", score_matrix[i][j]);
        }
        printf("\n");
    }*/
    return score_matrix;
}


///////////////////////////////////////calculate_alignment/////////////////////////////////////////


/*
 * Function:  calculate_alignment
 * ----------------------------------------
 *   Fills the score matrix with the score for each character
 *
 *   Parameters:
 *   
 *     *seq1:                 a pointer to the first sequence
 *	   *seq2:                 a pointer to the second sequence 	
 *     **substitution_matrix: a pointer to the substitution matrix 	 
 *     gap_penalty:           an integer representing gap penalty
  *    **aligned_seq1:        a pointer to the pointer of the first aligned sequence
 *	   **aligned_seq2:        a pointer to the pointer of the second aligned sequence 
 *
 * 	 Returns:
 *     
 *     max_score:             an integer representing rhe alignment score 	
 *     
 */
int calculate_alignment(char *seq1, char *seq2, int **substitution_matrix,
                         int gap_penalty, char **aligned_seq1, char **aligned_seq2) {
    int i, j;
    int m = strlen(seq1);
    int n = strlen(seq2);
    int **score_matrix;
    printf("\nCalculating alignment score \n");
    

    // fill the score matrix 
    score_matrix = fill_matrix(seq1, seq2, substitution_matrix, gap_penalty);

    // find the maximum score and its position in the score matrix
    int max_score = 0;
    int max_i = 0;
    int max_j = 0;
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (score_matrix[i][j] > max_score) {
                max_score = score_matrix[i][j]; // alignment score
                max_i = i; //start position
                max_j = j; //start position
            }
        }
    }
    

    // backtrack from the maximum score to build the aligned sequences
    char* align1 = (char*) malloc((m+n) * sizeof(char));
    char* align2 = (char*) malloc((m+n) * sizeof(char));
    int k = 0;
    i = max_i; // set stat position
    j = max_j;
    // from the stat position till the first 0
    while (score_matrix[i][j] != 0) {
        // if we came from the top left (diagonal) 
        if (score_matrix[i-1][j-1] + compute_score(&seq1[i-1], &seq2[j-1], substitution_matrix) == score_matrix[i][j]) {
            align1[k] = seq1[i-1];
            align2[k] = seq2[j-1];
            i--;
            j--;
        // if we came from the top 
        } else if (score_matrix[i-1][j] + gap_penalty == score_matrix[i][j]) {
            align1[k] = seq1[i-1];
            align2[k] = '-';
            i--;
        // if we came from the left 
        } else {
            align1[k] = '-';
            align2[k] = seq2[j-1];
            j--;
        }
        k++;
    }
    printf("Alignment score is calculated\n");
    printf("Alignment score: %d\nStart position: %d, %d\nEnd position: %d, %d\n", max_score, i, j, max_i, max_j);

    // reverse the aligned sequences
    for (i = 0; i < k/2; i++) {
        char tmp = align1[i];
        align1[i] = align1[k-1-i];
        align1[k-1-i] = tmp;
        tmp = align2[i];
        align2[i] = align2[k-1-i];
        align2[k-1-i] = tmp;
    }

    // assign the aligned sequences to the output parameters
    strncpy(*aligned_seq1, align1, k);
    (*aligned_seq1)[k] = '\0';

    strncpy(*aligned_seq2, align2, k);
    (*aligned_seq2)[k] = '\0';

    free(align1);
    free(align2);
    free(score_matrix[0]);
    free(score_matrix);

    return max_score;
}


///////////////////////////////////////////print_seq///////////////////////////////////////////


/*
 * Function:  print_seq
 * ----------------------------------------
 *   Prints alignment with no more than 60 characters per line
 *
 *   Parameters:
 *   
 *     *seq1:                 a pointer to the first sequence
 *	   *seq2:                 a pointer to the second sequence 	
 *
 * 	 Returns:	
 *     
 */
void print_seq(char* seq1, char* seq2) {
    int len1 = strlen(seq1);
    int len2 = strlen(seq2);
    int i = 0, j = 0;
    while (i < len1 || j < len2) {
        for (int k = 0; k < 60; k++) {
            if (i + k < len1) {
                printf("%c", seq1[i+k]);
            } else {
                break;
            }
        }
        printf("\n");
        for (int k = 0; k < 60; k++) {
            if (j + k < len2) {
                printf("%c", seq2[j+k]);
            } else {
                break;
            }
        }
        printf("\n\n");
        i += 60;
        j += 60;
    }
}


///////////////////////////////////////main/////////////////////////////////////////

      
int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Error: 2 files required\n");
        return 1;
    }
    
    char *input_file1 = argv[1];
    char *input_file2 = argv[2];

    char *seq1, *seq2;
    char *aligned_seq1;
    char *aligned_seq2;
    int **substitution_matrix;
    int gap_penalty = -5;

    // Read sequences and substitution matrix from files
    seq1 = read_sequence(input_file1);
    seq2 = read_sequence(input_file2);
    substitution_matrix = read_substitution_matrix("BLOSUM62.txt");
    
    // Allocate memory for aligned_seq1 and aligned_seq2
    aligned_seq1 = (char*) malloc((strlen(seq1) + strlen(seq2) + 1) * sizeof(char));
    aligned_seq2 = (char*) malloc((strlen(seq1) + strlen(seq2) + 1) * sizeof(char));

    int score = calculate_alignment(seq1, seq2, substitution_matrix, gap_penalty, &aligned_seq1, &aligned_seq2);

    printf("\nAlignment: \n\n");
    
    print_seq(aligned_seq1, aligned_seq2);

    // Free memory    
    free(seq1);
    free(seq2);
    free(substitution_matrix[0]);
    free(substitution_matrix);
    free(aligned_seq1);
    free(aligned_seq2);
    
    return 0;
}





