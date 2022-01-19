#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define N 81
#define Whole 0
#define M (int)sqrt(N)
#define S M-1

struct Sudoku{
    int grid[N][N];
    int slonGrid[N][N];
    int guessNum[N];
    int gridPos[N*N];
    int difficultyLevel;
    bool gridStatus;
};

//Generator functions
void initialize(struct Sudoku*);
void printGrid(struct Sudoku*);
void createSeed(struct Sudoku*);
bool solveGrid(struct Sudoku*);
bool FindUnassignedLocation(int [N][N], int*, int*);
bool isSafe(int [N][N], int, int, int);
bool usedInRow(int [N][N], int, int);
bool usedInCol(int [N][N], int, int);
bool usedInBox(int [N][N], int, int, int);
void shuffle(int*, int);
void genPuzzle(struct Sudoku*);
void countSoln(struct Sudoku*, int*);

//Solver functions
int isAvailable(int [N][N], int, int, int);
int fillsudoku(int [N][N], int row, int col);

int main(){

    //Generating process
    clock_t tic = clock();

    struct Sudoku map;
    initialize(&map);
    createSeed(&map);
    genPuzzle(&map);
    printGrid(&map);

    clock_t toc=clock();
    printf("\nTotal time elasped is %f\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    printf("\n");

    //Solving process
    tic = clock();

    if( fillsudoku(map.grid, 0, 0) ){
        printGrid(&map);
    }
    else printf("\n\nNO SOLUTION\n\n");

    toc = clock();
    printf("\nTotal time elasped is %f\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    return 0;
}

void initialize(struct Sudoku* map){
    printf("Initializing...\n");

    int i, j, temp;
    srand((unsigned int)time(NULL));
    
    map->difficultyLevel = 0;

    for(i = 0; i < N*N; i++){
        map->gridPos[i] = i;
    }

    shuffle(map->gridPos, N*N);

    for(i = 0; i < N; i++){
        map->guessNum[i] = i + 1;
    }

    shuffle(map->guessNum, N);

    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            map->grid[i][j] = Whole;
        }
    }

    map->gridStatus = true;
}

void printGrid(struct Sudoku* map){
    printf("==========================\n");
    printf("==========Sudoku==========\n\n");
    int i, j, k;

    for(i = 0; i < N; i++){
        printf("|");
        for(j = 0; j < N; j++){
            if(map->grid[i][j] == Whole) {
                if(j == 0 || (j+1) % M == 1) printf("*");
                else printf("  *");
            }
            else if(j == 0 || (j+1) % M == 1) printf("%d", map->grid[i][j]);
            else printf("%3d", map->grid[i][j]);
            if(j % M == S) printf("|");
        }
        printf("\n");
        if(i % M == S){
            for(k = 0; k < N*S; k++){
                printf("-");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void createSeed(struct Sudoku* map){
    printf("Generating solvable map...\n");
    int i, j;
    struct Sudoku* p = map;
    
    solveGrid(p);

    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            map->slonGrid[i][j] = map->grid[i][j];
        }
    }
}

bool solveGrid(struct Sudoku* map){
    int row, col, num;
    struct Sudoku* p = map;

    if(!FindUnassignedLocation(map->grid, &row, &col)) return true;

    for(num = 0; num < N; num++){
        if(isSafe(map->grid, row, col, map->guessNum[num])){
            map->grid[row][col] = map->guessNum[num];
            
            if(solveGrid(p)) return true;

            map->grid[row][col] = Whole;
        }
    }

    return false;
}

bool FindUnassignedLocation(int grid[N][N], int* row, int* col){
    for(*row = 0; *row < N; *(row)+=1){
        for(*col = 0; *col < N; *(col)+=1){
            if(grid[*row][*col] == Whole) return true;
        }
    }

    return false;
}

bool isSafe(int grid[N][N], int row, int col, int num){
    return !usedInRow(grid, row, num) && !usedInCol(grid, col, num) && !usedInBox(grid, row - row % M, col - col % M, num);
}

bool usedInRow(int grid[N][N], int row, int num){
    int col;

    for(col = 0; col < N; col++){
        if(grid[row][col] == num) return true;
    }

    return false;
}

bool usedInCol(int grid[N][N], int col, int num){
    int row;

    for(row = 0; row < N; row++){
        if(grid[row][col] == num) return true;
    }

    return false;
}

bool usedInBox(int grid[N][N], int srow, int scol, int num){
    int row, col;

    for(row = 0; row < M; row++){
        for(col = 0; col < M; col++){
            if(grid[row + srow][col + scol] == num) return true;
        }
    }

    return false;
}

void shuffle(int *array, int n){
    int i, j, temp;

    srand((unsigned int)time(NULL));

    for (i = 0; i < n - 1; i++){
        j = i + rand() / (RAND_MAX / (n - i) + 1);
        temp = array[j];
        array[j] = array[i];
        array[i] = temp;
    }
}

void genPuzzle(struct Sudoku* map){
    printf("Making wholes on map...\n");

    int i, x, y, temp, check;
    struct Sudoku* p = map;

    for(i = 0; i < N*N; i++){
        x = map->gridPos[i] / N;
        y = map->gridPos[i] % N;
        temp = map->grid[x][y];
        map->grid[x][y] = Whole;
        check = Whole;
        countSoln(p, &check);
        if(check != 1) map->grid[x][y] = temp;
    }
}

void countSoln(struct Sudoku* map, int* num){
    int row, col, i;
    struct Sudoku* p = map;
    int* pnum = num;

    if(!FindUnassignedLocation(map->grid, &row, &col)){
        *num+=1;
        return;
    }

    for(i = 0; i < N && *num < S; i++){
        if(isSafe(map->grid, row, col, map->guessNum[i])){
            map->grid[row][col] = map->guessNum[i];
            countSoln(p, pnum);
        }

        map->grid[row][col] = Whole;
    }
}

int isAvailable(int sudoku[N][N], int row, int col, int num){
    int i, j;
    for(i=0; i<N; i++){
        if( (sudoku[row][i] == num) || ( sudoku[i][col] == num )  ) return 0;
    }

    int rowStart = (row/M) * M;
    int colStart = (col/M) * M;

    for(i=rowStart; i<(rowStart+M); ++i){
        for(j=colStart; j<(colStart+M); ++j){
            if( sudoku[i][j] == num ) return 0;
        }
    }
    return 1;
}

int fillsudoku(int sudoku[N][N], int row, int col){
    int i;
    
    if( row<N && col<N ){
        if( sudoku[row][col] != 0 ){
            if( (col+1)<N ) return fillsudoku(sudoku, row, col+1);
            else if( (row+1)<N ) return fillsudoku(sudoku, row+1, 0);
            else return 1;
        }
        else{
            for(i=0; i<N; ++i){
                if( isAvailable(sudoku, row, col, i+1) ){
                    sudoku[row][col] = i+1;

                    if( (col+1)<N ){
                        if( fillsudoku(sudoku, row, col+1) ) return 1;
                        else sudoku[row][col] = 0;
                    }
                    else if( (row+1)<N ){
                        if( fillsudoku(sudoku, row+1, 0) ) return 1;
                        else sudoku[row][col] = 0;
                    }
                    else return 1;
                }
            }
        }
        return 0;
    }
    else return 1;
}