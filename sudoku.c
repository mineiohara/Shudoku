#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>

#define N 16
#define hole 0
#define M (int)sqrt(N)
#define S M-1
#define K (int)(N*N*0.4)
#define T 8

int sudoku2[N][N] = {0};//For Pthread

struct v {
    int row;
};

//Common functions
bool fillRemainingArea(int, int, int[N][N]);
bool isSafe(int, int, int, int[N][N]);
bool UnusedInBox(int, int, int, int[N][N]);
bool UnusedInRow(int, int, int[N][N]);
bool UnusedInCol(int, int, int[N][N]);
void printGrid(int[N][N]);

//Functions for single thread
void fillInndependentArea(int[N][N]);
void makeHoles(int[N][N]);

//Functions for pthread
void *pthreadFillIndependentArea(void*);
void *pthreadMakeHoles(void*);

//Functions for OpenMP
void ompFillIndependentArea(int[N][N]);
void ompMakeHoles(int[N][N]);

//Functions for solver
bool solveSudoku(int, int, int[N][N]);

int main(){
    int i;
    int sudoku1[N][N] = {0};//For Single thread
    int sudoku3[N][N] = {0};//For OpenMP

    struct timespec tic, toc;
    double t[3] = {0};
    pthread_t tid[N], tid2[T];
    pthread_attr_t attr[N], attr2[T];

    //////////////////////////////////Generating Part/////////////////////////////////////////////
    //Single thread
    printf("Generating Sudoku1 by Single Theard\n");
    
    clock_gettime(CLOCK_REALTIME, &tic);
    fillInndependentArea(sudoku1);
    fillRemainingArea(0, M, sudoku1);
    makeHoles(sudoku1);
    clock_gettime(CLOCK_REALTIME, &toc);
    t[0] = (toc.tv_sec - tic.tv_sec) * 1000.0 + (toc.tv_nsec - tic.tv_nsec) / 1000000.0;
    
    printGrid(sudoku1);
    //Solvving sudoku1
    printf("Solving Sudoku1\n");
    if( solveSudoku(0, 0, sudoku1) );
    else printf("\n\nNO SOLUTION\n\n");
    printGrid(sudoku1);
    
    //Pthread
    printf("Generating Sudoku2 by Ptheard\n");
    clock_gettime(CLOCK_REALTIME, &tic);
   
    for(i = 0; i < M; i++){
        struct v *data = (struct v *) malloc(sizeof(struct v));
        data->row = i*M;
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], pthreadFillIndependentArea, data);
    }
    for(i = 0; i < M; i++) pthread_join(tid[i],NULL);

    fillRemainingArea(0, M, sudoku2);
    
    for(i = 0; i < T; i++){
        pthread_attr_init(&attr2[i]);
        pthread_create(&tid2[i], &attr2[i], pthreadMakeHoles, NULL);
    }
    for(i = 0; i < T; i++) pthread_join(tid2[i], NULL);

    clock_gettime(CLOCK_REALTIME, &toc);
    t[1] = (toc.tv_sec - tic.tv_sec) * 1000.0 + (toc.tv_nsec - tic.tv_nsec) / 1000000.0;
    
	printGrid(sudoku2);
	//Solvving sudoku2
    printf("Solving Sudoku2\n");
    if( solveSudoku(0, 0, sudoku2) );
    else printf("\n\nNO SOLUTION\n\n");
    printGrid(sudoku2);
    
    //OpenMP
    printf("Generating Sudoku3 by OpenMP\n");
    
    clock_gettime(CLOCK_REALTIME, &tic);
    ompFillIndependentArea(sudoku3);
    fillRemainingArea(0, M, sudoku3);
    ompMakeHoles(sudoku3);
    clock_gettime(CLOCK_REALTIME, &toc);
    t[2] = (toc.tv_sec - tic.tv_sec) * 1000.0 + (toc.tv_nsec - tic.tv_nsec) / 1000000.0;
    
    printGrid(sudoku3);
    //Solvving sudoku3
    printf("Solving Sudoku3\n");
    if( solveSudoku(0, 0, sudoku3) );
    else printf("\n\nNO SOLUTION\n\n");
    printGrid(sudoku3);
 
    printf("Generate Sudoku1 by Single Thread: %fms\n", t[0]);
    printf("Generate Sudoku2 by Pthread      : %fms\n", t[1]);
    printf("Generate Sudoku3 by OpenMP       : %fms\n", t[2]);
    return 0;
}

bool UnusedInBox(int rowStart, int colStart, int num, int grid[N][N]){
    int i, j;
    for ( i = 0; i < M; i++) for ( j = 0; j < M; j++){
        if (grid[rowStart+i][colStart+j]==num) return false;
 
    }
    return true;
}

void fillInndependentArea(int sudoku1[N][N]){
    int i, j, k, num;

    srand((unsigned int)clock());

    for(i = 0; i< N; i+=M){
        for (j=0; j<M; j++)
        {
            for (k=0; k < M; k++)
            {
                do{
                    num = (int)(rand() % N) + 1;
                }
                while (!UnusedInBox(i, i, num, sudoku1));

                sudoku1[i+j][i+k] = num;
            }
        }
    }
}

bool UnusedInRow(int i,int num, int grid[N][N]){
    int j;
    for (j = 0; j<N; j++) if (grid[i][j] == num) return false;
    return true;
}

bool UnusedInCol(int j,int num, int grid[N][N]){
    int i;
    for (i = 0; i<N; i++) if (grid[i][j] == num) return false;
    return true;
}

bool isSafe(int i,int j,int num, int grid[N][N]){
    return (UnusedInRow(i, num, grid) && UnusedInCol(j, num, grid) && UnusedInBox(i-i%M, j-j%M, num, grid));
}

bool fillRemainingArea(int i, int j, int grid[N][N]){
    int num;
    if (j >= N && i < N - 1){
        i++;
        j = 0;
    }

    if (i>=N && j>=N) return true;
 
    if (i < M){
        if (j < M) j = M;
     }
    
    else if (i < N - M){
        if (j == (int)(i / M) * M)  j =  j + M;
    }
    
    else{
        if (j == N-M){
            i = i + 1;
            j = 0;

            if (i>=N) return true;
        }
    }
 
    for (num = 1; num<=N; num++){

        if (isSafe(i, j, num, grid)){
            grid[i][j] = num;
            
            if (fillRemainingArea(i, j+1, grid)) return true;
 
            grid[i][j] = hole;
        }
    }
        
    return false;
}

void *pthreadFillIndependentArea(void* param){
    int j, k, num;
    struct v *data = param;

    for (j=0; j<M; j++){
        for ( k=0; k < M; k++){
            srand((unsigned int)(clock() * k));
            do{
                num = (int)(rand() % N) + 1;
            }
            while (!UnusedInBox(data->row, data->row, num, sudoku2));

            sudoku2[data->row+j][data->row+k] = num;
        }
    }
    pthread_exit(0);
}

void ompFillIndependentArea(int sudoku3[N][N]){
    int i, j, k, num;

    #pragma parallel for
    for(i = 0; i< N; i+=M){
        // srand((unsigned int)clock() * i);
        for (j=0; j<M; j++){
            for (k=0; k < M; k++){
                srand((unsigned int)(clock() * k));
                do{
                    num = (int)(rand() % N) + 1;
                }
                while (!UnusedInBox(i, i, num, sudoku3));

                sudoku3[i+j][i+k] = num;
            }
        }
    }
}

void makeHoles(int grid[N][N]){
    int i, j, k, cell;

    for(k = 0; k < K; k++){
        srand((unsigned int)clock() * k);
        cell = (int)(rand() % (N*N));
        i = (cell / N);
        j = (cell % N);
    
        while(grid[i][j] == 0){
            cell = (int)(rand() % (N*N));
            i = (cell / N);
            j = (cell % N);
        }
    
        grid[i][j] = 0;
    }
}

void ompMakeHoles(int sudoku3[N][N]){
    int i, j, k, cell;

    #pragma omp parallel for
    for(k = 0; k < K; k++){
        srand((unsigned int)(clock() * k));
        cell = (int)(rand() % (N*N));
        i = (cell / N);
        j = (cell % N);
    
        while(sudoku3[i][j] == 0){
            cell = (int)(rand() % (N*N));
            i = (cell / N);
            j = (cell % N);
        }
    
        sudoku3[i][j] = 0;
    }
}

void *pthreadMakeHoles(void *param){
    int i, j, k, cell;

    for(k = 0; k < (int)(K/T); k++){
        srand((unsigned int)(clock() * k));
        cell = (int)(rand() % (N*N));
        i = (cell / N);
        j = (cell % N);
    
        while(sudoku2[i][j] == 0){
            cell = (int)(rand() % (N*N));
            i = (cell / N);
            j = (cell % N);
        }
    
        sudoku2[i][j] = 0;
    }
    pthread_exit(0);
}

bool solveSudoku(int row, int col, int grid[N][N])
{
    int num;
    if (row == N - 1 && col == N)   return true;

    if (col == N)
    {
        row++;
        col = 0;
    }
    if (grid[row][col] > 0)     return solveSudoku(row, col + 1, grid);
    for (num = 1; num <= N; num++)
    {
        if (isSafe(row, col, num, grid)==1)
        {
            grid[row][col] = num;
            if (solveSudoku(row, col + 1, grid)==1)
                return true;
        }
        grid[row][col] = 0;
    }
    return false;
}

void printGrid(int grid[N][N]){
    int i, j, k;
   
    for(i = 0; i <= N*3+M; i++){
        printf("-");
    }
    printf("\n");
    for(i = 0; i < N; i++){
        printf("|");
        for(j = 0; j < N; j++){
            if(grid[i][j] == hole) {
                printf("  *");
            }
            else printf("%3d", grid[i][j]);
            if(j % M == S) printf("|");
        }
        printf("\n");
        if(i % M == S){
            for(k = 0; k <= N*3+M; k++){
                printf("-");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}
