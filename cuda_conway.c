/* Ethan Kochis
 * CSC470-01: HPC Systems Design & Implementation
 * Final Project - Conway's Game of Life
 * This is the CUDA version
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define ALIVE 1
#define DEAD 0

#define TRUE 1
#define FALSE 0

#define NEIGHBORS 8

// CUDA kernel for determining whether a cell should be dead or alive
__global__ void conway(int *conwayBoard, int *newConwayBoard, int numCols, int numRows, int numCells){
    /** From Wikipedia:
     The universe of the Game of Life is an infinite two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, alive or dead, or "populated" or "unpopulated". Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur:
     
     * Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
     * Any live cell with two or three live neighbours lives on to the next generation.
     * Any live cell with more than three live neighbours dies, as if by overpopulation.
     * Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
     
     The initial pattern constitutes the seed of the system. The first generation is created by applying the above rules simultaneously to every cell in the seed—births and deaths occur simultaneously, and the discrete moment at which this happens is sometimes called a tick (in other words, each generation is a pure function of the preceding one). The rules continue to be applied repeatedly to create further generations.
     
     */
    
    int i = blockIdx.x*blockDim.x + threadIdx.x;
    
    int onLeftEdge = FALSE;
    int onRightEdge = FALSE;
    int onTopEdge = FALSE;
    int onBottomEdge = FALSE;
    
    if(i % numCols == 0)
        onLeftEdge = TRUE;
    
    if(i % numCols == numCols-1)
        onRightEdge = TRUE;
    
    if(i < numCols)
        onTopEdge = TRUE;
    
    if(i >= numCells-numCols)
        onBottomEdge = TRUE;
    
    if (i < numCells){
        
        // populate this array with the possible indices
        int indexArray[NEIGHBORS];
        
        // Cell to the left of the current cell
        if(onLeftEdge)
            indexArray[0] = i + numCols - 1;            // last cell in same row
        else
            indexArray[0] = i - 1;
        
        // Cell to the left and below the current cell
        if(onLeftEdge && onBottomEdge)
            indexArray[1] = numCols - 1;                // cell in the top right corner
        else if(onLeftEdge)
            indexArray[1] = i + (2 * numCols) - 1;      // cell one row down, all the way to the right
        else if(onBottomEdge)
            indexArray[1] = i % numCols - 1;            // cell in first row in the column to the left
        else
            indexArray[1] = i + numCols - 1;
        
        // Cell below the current cell
        if(onBottomEdge)
            indexArray[2] = i % numCols;                // cell in top row, same column
        else
            indexArray[2] = i + numCols;
        
        // Cell to the right and below the current cell
        if(onRightEdge && onBottomEdge)
            indexArray[3] = 0;                          // cell in the top left corner
        else if(onRightEdge)
            indexArray[3] = i + 1;                      // first cell in next row
        else if(onBottomEdge)
            indexArray[3] = i % numCols + 1;            // cell in first row in the column to the right
        else
            indexArray[3] = i + numCols + 1;
        
        // Cell to the right of the current cell
        if(onRightEdge)
            indexArray[4] = i - numCols + 1;            // first cell in same row
        else
            indexArray[4] = i + 1;
        
        // Cell to the right and above the current cell
        if(onRightEdge && onTopEdge)
            indexArray[5] = numCells - numCols;         // cell in bottom left corner
        else if(onRightEdge)
            indexArray[5] = i - (2 * numCols) + 1;      // first cell in row above
        else if(onTopEdge)
            indexArray[5] = numCells - numCols + i + 1; // cell in bottom row in the column to the right
        else
            indexArray[5] = i - numCols + 1;
        
        // Cell above the current cell
        if(onTopEdge)
            indexArray[6] = numCells - numCols + i;     // cell in bottom row, same column
        else
            indexArray[6] = i - numCols;
        
        // Cell to the left and above the current cell
        if(onLeftEdge && onTopEdge)
            indexArray[7] = numCells - 1;               // cell in bottom right corner
        else if(onLeftEdge)
            indexArray[7] = i - 1;                      // cell in above row, all the way to the right
        else if(onTopEdge)
            indexArray[7] = numCells - numCols + i - 1; // cell in bottom row in the column to the left
        else
            indexArray[7] = i - numCols - 1;
        
        // number of alive neighbors
        int numAliveNeighbors = 0;
        // number of dead neighbors
        int numDeadNeighbors = 0;
        
        for(int j = 0; j < NEIGHBORS; j++){
            
            // check if the index is valid
            if(indexArray[j] < 0 || indexArray[j] >= numCells){
                
                // if the index is invalid, treat as a dead neighbor
                numDeadNeighbors++;
                
            }else{
                
                // if the index is valid, perform check
                if(conwayBoard[indexArray[j]] == ALIVE)
                    numAliveNeighbors++;
                else
                    numDeadNeighbors++;
                
            }
            
            // escape condition for the for loop (decrease amount of checks needed)
            if(numAliveNeighbors > 3){
                // if there are more than 3 neighbors, this cell will become DEAD whether or not in was DEAD or ALIVE previously
                break;
            }
                
            
        }
        
        // if the cell is currently dead
        if(conwayBoard[i] == DEAD){
            //and it has 3 alive neighbors
            if(numAliveNeighbors == 3)
                //make it alive
                newConwayBoard[i] = ALIVE;
            else
                //otherwise it remains dead
                newConwayBoard[i] = DEAD;
            
            // if the cell is currently alive
        }else if(conwayBoard[i] == ALIVE){
            
            //and it has 2 or 3 alive negihbors
            if(numAliveNeighbors == 3 or numAliveNeighbors == 2)
                //it remains alive
                newConwayBoard[i] = ALIVE;
            else
                //otherwise it dies
                newConwayBoard[i] = DEAD;
        }
        
    }
    
}

// CUDA kernel to copy individual array indices from "new" board to "old" board
__global__ void copyNewConwayIntoOldConway(int *conwayBoard, int *newConwayBoard, int numCells){
    
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    // If the i is valid, copy array index i from "new" board to "old" board
    if (i < numCells)
        conwayBoard[i] = newConwayBoard[i];
    
}


// output the generation file
void outputGenFile(char *outputFileName, int *conwayBoard, int numRows, int numCols){
    
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

// generate the PNG file by creating a Python child process
void generatePNGFile(char *genNumberAsString, int genNumber, pid_t *all_pids){
    
    // arguments for Python program to generate PNG file using matplotlib
    char *python_args[]={"python3", "make_PNG_Grid.py", genNumberAsString, NULL};
    
    // fork a child process
    pid_t python_pid = fork();
    
    // if child, execute the python program
    if(python_pid == 0){
        execvp(python_args[0], python_args);
        printf("Python child %i failed to execute...\n", genNumber);
        exit(1);
    }
    
    // save the pid to wait on later
    all_pids[genNumber] = python_pid;
    
}

// Print statement if invalid command line usage or input file fail
void printInfo(){
    
    printf("Usage: ./conway <numRows> <numCols> <numGenerations> <optional:inputFile>\n");
    printf("numRows: The number of rows.\n");
    printf("numCols: The number of columns.\n");
    printf("numGenerations: The number of generations to iterate through.\n");
    printf("optional:inputFile: Optionally, you may specify an input file that has the format:\n");
    printf("Note: The numbers in the inputFile will overwrite whatever you enter at the command line for <numRows> and <numCols>\n");
    printf("numRows numCols\n");
    printf("Row1 with numCols entries\n");
    printf("Row2 with numCols entries\n");
    printf("...\n");
    printf("Row{numRows-1} with numCols entries\n");
    printf("Row{numRows} with numCols entries\n");
    printf("Where each entry is either 0 (DEAD) or 1 (ALIVE)\n");
    
}



int main(int argc, char* argv[]){
    
    if(argc < 4 || argc > 5){
        
        printInfo();
        printf("Exiting...\n");
        return 1;
        
    }
    
    // Time variables
    struct timeval startTime, endTime;
    
    // Store start time
    gettimeofday(&startTime, NULL);
    
    // Parameters
    // number of rows
    int numRows = atoi(argv[1]);
    // number of columns
    int numCols = atoi(argv[2]);
    // number of generations
    int numGenerations = atoi(argv[3]);
    // total number of cells
    int numCells = numRows * numCols;
    
    // the results folder
    const char resultsFolder[] = "results/";
    
    // check if results folder exists
    struct stat st;
    if (stat(resultsFolder, &st) == -1) {
        // create it if it doesn't
        mkdir(resultsFolder, 0777);
    }else{
        // print out error message and quit
        printf("The \"results\" folder still exists.\n");
        printf("Please rename or delete it and run again.\n");
        return 1;
    }
    
    // pointers for the boards
    int *conwayBoard;       // used in general case
    int *newConwayBoard;    // used to ensure race conditions do not occur in CUDA kernel function
    
    // allocate memory for the boards (on both CPU and GPU)
    cudaMallocManaged(&conwayBoard, numCells*sizeof(int));
    cudaMallocManaged(&newConwayBoard, numCells*sizeof(int));
    
    // if there are 4 arguments, create a random board
    if(argc == 4){
        
        srand(time(NULL));
    
        for(int i = 0; i < numCells; i++) {
            
            int oneNum = rand() % 2;
            int twoNum = rand() % 2;
            
            conwayBoard[i] = oneNum && twoNum;
            
        }
        
    }else if(argc == 5){
        
        // if there is an input file specified
        struct stat input;
        if (stat(argv[4], &input) == -1) {
            // input file does not exist
            printf("The specified input file %s does not exist.\n", argv[4]);
            printf("Exiting...\n");
            cudaFree(conwayBoard);
            cudaFree(newConwayBoard);
            return 1;
            
        }
        
        // open file
        FILE *inputFile = fopen(argv[4], "r");
        
        // get the number of rows and number of columns
        fscanf(inputFile, "%d %d\n", &numRows, &numCols);
        for(int row = 0; row < numRows; row++){
            // fill in board
            for(int col = 0; col < numCols; col++){
                if(col != numCols - 1)
                    fscanf(inputFile, "%d ", &conwayBoard[(row * 10) + col]);
                else
                    fscanf(inputFile, "%d", &conwayBoard[(row * 10) + col]);
            }
            
            fscanf(inputFile, "\n");
            
        }
        // close file
        fclose(inputFile);
        
    }
    
    
    // store all the process IDs of the children spawned to generate the grid in Python using matplotlib
    // include generation 0
    pid_t all_pids[numGenerations+1];
    
    // output gen0 file
    outputGenFile("results/gen0.txt", conwayBoard, numRows, numCols);
    
    generatePNGFile("0", 0, all_pids);
    
    // loop through generations
    for(int genNumber = 1; genNumber <= numGenerations; genNumber++){

        // each row is a block
        int blockSize = numRows;
        int numBlocks = (numCells + blockSize - 1) / blockSize;
        // lanuch CUDA kernel that figures out whether or not each cell is dead or alive
        conway<<<numBlocks, blockSize>>>(conwayBoard, newConwayBoard, numCols, numRows, numCells);

        // Wait for GPU to finish before accessing on host
        cudaDeviceSynchronize();
        
        // lanuch CUDA kernel that copies the new conway board into the original conway board
        // (the new conway board ensures there are no race conditions when updating conwayBoard)
        copyNewConwayIntoOldConway<<<numBlocks, blockSize>>>(conwayBoard, newConwayBoard, numCells);
        
        // Wait for GPU to finish before accessing on host
        cudaDeviceSynchronize();
        
        // get the current generation number in string format
        char *generationNumberAsString = (char *) malloc(sizeof(char) * 20);
        sprintf(generationNumberAsString, "%d", genNumber);
        
        // create the output file name: results/gen<genNumber>.txt
        char outputFileName[25];
        strcpy(outputFileName, resultsFolder);
        strcat(outputFileName, "gen");
        strcat(outputFileName, generationNumberAsString);
        strcat(outputFileName, ".txt");
        
        // output the text file for this generation
        outputGenFile(outputFileName, conwayBoard, numRows, numCols);
        // output the PNG file for this generation
        generatePNGFile(generationNumberAsString, genNumber, all_pids);
        
        // frees the current generation number in string format
        free(generationNumberAsString);

    }

    // free shared memory from the boards between CPU and GPU
    cudaFree(conwayBoard);
    cudaFree(newConwayBoard);
    
    gettimeofday(&endTime, NULL);
    
    double elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;  // sec to ms
    elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;      // us to ms
    
    printf("Completed iterations, printing elapsed time:\n");
    printf("%.3lf\n", elapsedTime);
    
    // wait for all Python child processes to finish before making GIF
    for(int i = 0; i < numGenerations; i++){
//        printf("Waiting on process ID: %i\n", (int) all_pids[i]);
        waitpid(all_pids[i], NULL, WUNTRACED);
    }
    
//    printf("Converting to GIF...\n");
    
    // use convert program from imagemagick package to make GIF
    char *convert_args[]={"convert", "-delay", "40", "-loop", "1", "results/gen*.png", "conway.gif", NULL};
    
    // fork a child for the convert program
    pid_t convert_pid = fork();
    
    // if child, execute the convert program
    if(convert_pid == 0){
        execvp(convert_args[0], convert_args);
        printf("Child process for convert failed to execute...\n");
        exit(1);
    }else{
        
        // if parent, wait for child to finish
        waitpid(convert_pid, NULL, WUNTRACED);
        
    }
    
    printf("Conway's Game of Life using CUDA has finished\n");
    
}
