#include <stdio.h>
#include<time.h>
#define N 16
int isAvailable(int sudoku[16][16], int row, int col, int num)
{
int i, j;
for(i=0; i<16; i++){

     if( (sudoku[row][i] == num) || ( sudoku[i][col] == num )  )
        return 0;
}

int rowStart = (row/4) * 4;
int colStart = (col/4) * 4;

for(i=rowStart; i<(rowStart+4); ++i)
{
    for(j=colStart; j<(colStart+4); ++j)
    {
        if( sudoku[i][j] == num )
            return 0;
    }
}

return 1;
}

int fillsudoku(int sudoku[16][16], int row, int col)
{
int i;
if( row<16 && col<16 )
{
    if( sudoku[row][col] != 0 )
    {
        if( (col+1)<16 )
            return fillsudoku(sudoku, row, col+1);
        else if( (row+1)<16 )
            return fillsudoku(sudoku, row+1, 0);
        else
            return 1;
    }
    else
    {
        for(i=0; i<16; ++i)
        {
            if( isAvailable(sudoku, row, col, i+1) )
            {
                sudoku[row][col] = i+1;

                if( (col+1)<16 )
                {
                   if( fillsudoku(sudoku, row, col+1) )
                       return 1;
                    else
                       sudoku[row][col] = 0;
                }
                else if( (row+1)<16 )
                {
                    if( fillsudoku(sudoku, row+1, 0) )
                        return 1;
                    else
                        sudoku[row][col] = 0;
                }
                else
                    return 1;
            }
        }
    }
    return 0;
}
else
{
    return 1;
}
}

int main()
{
clock_t tic=clock();
int i,j;
int sudoku[N][N]=
       {{ 0, 0, 4,14, 0,16, 7, 0, 0, 0,11, 0, 8, 0, 1, 3},
        { 0, 9, 5,11, 4, 0,13, 6,12, 8, 0, 0, 7, 0,10, 0},
        { 0,15, 0,16, 1, 3, 0,12, 0, 0, 0, 0, 0, 9, 0, 0},
        { 8, 0, 0, 0, 0, 0, 0, 9, 0, 0,16,10,13, 6, 0, 0},
        
        { 0, 1, 0, 9, 0, 6, 0, 0,10, 0,12, 0, 0, 0,13, 0},
        {14, 0, 0, 0, 0, 0,16, 0, 0,11, 6, 2, 3, 1, 8, 0},
        { 0, 0, 0, 0,13, 0, 0, 4, 0, 3, 9, 0,16, 0, 0,12},
        { 0, 0, 0, 0, 8, 0, 3, 0, 4, 0,15, 0,11, 5, 2, 0},
        
        { 9, 0, 0, 5,11, 0, 0, 2, 7, 0, 1,16, 0, 0,14,10},
        { 0, 0, 0, 4,14,10, 0, 0, 8, 0, 0, 0,12, 0,16, 0},
        {12, 0,16, 1, 0, 5, 9, 0, 0,15, 0, 0, 0, 2, 0, 4},
        { 0, 0,14, 0, 0, 0, 0, 7, 2, 6, 0, 0, 0, 8, 0, 5},
        
        {10,14, 0, 0,12, 8, 0, 0, 0, 0, 0, 0, 5, 3, 0, 2},
        { 0, 0,12, 0, 0, 2, 0, 0,14,10, 0, 0, 0, 0, 6, 0},
        { 0, 0, 0, 0, 0, 0, 4,11, 0, 0, 0, 0,10, 0,15, 0},
        { 4, 0, 6,13,15, 0, 0, 0, 0, 0, 0, 9, 0,16, 0, 0}};//15
/*FILE *f;
f=fopen("s16.txt","r");


    for(i=0;i<N;i++){

        for(j=0;j<N;j++)
        {
            fscanf(f,"%d",&sudoku[i][j]);

        }
    }*/
if( fillsudoku(sudoku, 0, 0) )
{

    for(i=0; i<16; ++i)
    {
        for(j=0; j<16; ++j){

            printf("%d ", sudoku[i][j]);
            if(sudoku[i][j]<10){

                printf(" ");
            }
            if(j%4==3){
                printf("|");
            }
        }
        printf("\n");
        if(i%4==3){
        printf("----------------------------------------------------");
        }
        printf("\n");
    }
}
else
{
    printf("\n\nNO SOLUTION\n\n");
}
clock_t toc=clock();
printf("\nTotal time elasped is %f", (double)(toc - tic) / CLOCKS_PER_SEC);
return 0;
//fclose(f);
}

