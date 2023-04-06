# Smith-Waterman Pairwise Sequence Alignment

### Algorithm description

The Smith–Waterman algorithm performs local sequence alignment; that is, for determining similar regions between two strings of nucleic acid sequences or protein sequences. Instead of looking at the entire sequence, the Smith–Waterman algorithm compares segments of all possible lengths and optimizes the similarity measure.

The Smith–Waterman algorithm has several steps:

1. *Determine the substitution matrix and the gap penalty scheme.* 
A substitution matrix assigns each pair of bases or amino acids a score for match or mismatch. A gap penalty function determines the score cost for opening or extending gaps. 

2. *Initialize the scoring matrix.* 
The dimensions of the scoring matrix are 1+length of each sequence respectively. All the elements of the first row and the first column are set to 0. The extra first row and first column make it possible to align one sequence to another at any position, and setting them to 0 makes the terminal gap free from penalty.

3. *Scoring.* 
Score each element from left to right, top to bottom in the matrix, considering the outcomes of substitutions (diagonal scores) or adding gaps (horizontal and vertical scores). If none of the scores are positive, this element gets a 0. Otherwise the highest score is used and the source of that score is recorded.

4. *Traceback.* 
Starting at the element with the highest score, traceback based on the source of each score recursively, until 0 is encountered. The segments that have the highest similarity score based on the given scoring system is generated in this process.

### Implementation

In this project the Smith–Waterman algorithm was performed in C language.

To clone the repository:

```bash
git clone https://github.com/zhukovanadezhda/smith-waterman.git
```

To test the algorithm:

`
gcc -Wall projet.c -o pro ; ./pro <filename1> <filename2>
`

Where `<filename1>` and `<filename2>` are the .fasta files with sequences. Each file should include only one sequence.

### Examples

To test the program two web resources were used. One for the short examples with visualisation of the score matrix and another one is a real Smith–Waterman algorithm from EMBOSS to aligne real sequences.

[Short alignments (<10)](https://gtuckerkellogg.github.io/pairwise/demo/)

[Real alignments](https://www.ebi.ac.uk/Tools/psa/emboss_water/)

#### Example 1: with short example sequences

`gcc -Wall projet.c -o pro ; ./pro examples/example1.fasta examples/example2.fasta`

```
Alignment score: 23
Start position: 4, 1
End position: 9, 5

Alignment:

AWGHE
AW-HE
```

#### Example 2: with sequences 1BTA and 1AKE

`gcc -Wall projet.c -o pro ; ./pro examples/1AKE_1.fasta examples/1BTA_1.fasta`

```
Alignment score: 48
Start position: 45, 6
End position: 109, 74

Alignment:

GKQAKDIMDAGKLVTDELVI-ALVKERI-AQEDCRNGFLLDGFPRTIP--QADAMKEAGI
GEQIRSISDLHQTLKKELALPEYYGENLDALWDCLTGW-VE-YPLVLEWRQFEQSKQLTE

N-VDYVLE-F
NGAESVLQVF
```

#### Example 3: with sequences 1K5D and 1K5C

`gcc -Wall projet.c -o pro ; ./pro examples/1K5D_3.fasta examples/1K5C_1.fasta`

```
Alignment score: 46
Start position: 69, 201
End position: 193, 323

Alignment:

IFTGR-VKDEIPEALRLLLQALLKCP-KLHTVRLSDNAFGPTAQEPLIDFLSKHTPL-EH
IATGKHVSNVVIKG-NTVTRSMYGVRIKAQRTATSASVSGVTYDANTISGIAKYGVLISQ

LYLHNNGLGPQAGAKIARA-LQELAVNKKAKNAPPLRSIICGRNRLENGSMKEWAK-TFQ
SYPDDVG-NPGTGAPFSDVNFTGGATTIKVNNAATRVTVECG-N-C-SGNW-NWSQLTVT

SHRLLHTVK
GGK-AGTIK
```
