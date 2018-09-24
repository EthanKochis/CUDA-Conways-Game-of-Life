/* Ethan Kochis
 * CSC470-01: HPC Systems Design & Implementation
 * Final Project - Conway's Game of Life
 *
 * This is the CUDA version, run on STEM Cluster
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// output the input file
void outputInputFile(char *outputFileName, int *conwayBoard, int numRows, int numCols){
    
    // open the file
    FILE *outputFile = fopen(outputFileName, "w");
    
    // save the number of rows and columns
    fprintf(outputFile, "%d %d\n", numRows, numCols);
    for(int row = 0; row < numRows; row++){
        // save board
        for(int col = 0; col < numCols; col++){
            if(col != numCols - 1)
                fprintf(outputFile, "%d ", conwayBoard[(row * 10) + col]);
            else
                fprintf(outputFile, "%d", conwayBoard[(row * 10) + col]);
            
        }
        
        fprintf(outputFile, "\n");
        
    }
    
    //close file
    fclose(outputFile);
    
}

// Print statement if invalid command line usage or input file fail
void printInfo(){
    
    printf("Creates an input file for both conway and non_cuda_conway to use.\n");
    printf("Usage: ./make_input <numRows> <numCols>\n");
    printf("numRows: The number of rows.\n");
    printf("numCols: The number of columns.\n");
    
}


int main(int argc, char* argv[]){
    
    if(argc != 3){
        
        printInfo();
        printf("Exiting...\n");
        return 1;
        
    }
    
    // Parameters
    // number of rows
    int numRows = atoi(argv[1]);
    // number of columns
    int numCols = atoi(argv[2]);
    // total number of cells
    int numCells = numRows * numCols;
    
    // input file path
    char inputFilePath[] = "input.txt";
    
    // pointers for the boards
    int *conwayBoard;       // used in general case
    
    conwayBoard = malloc(sizeof(int) * numCells);
    
    srand(time(NULL));
    
    for(int i = 0; i < numCells; i++) {
        
        int oneNum = rand() % 2;
        int twoNum = rand() % 2;
        
        conwayBoard[i] = oneNum && twoNum;
        
    }
    
    outputInputFile(inputFilePath, conwayBoard, numRows, numCols);
        
    
}

