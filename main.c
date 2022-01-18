#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 9

int get_rand( int min_val, int max_val);
void shuffle(int *array, int size);
int isSafe(int grid[N][N], int row, int col, int num);
void printer(int array[N][N]);

int main(){
    int temp[N*N];
    int shudoku[N][N];
    int i, j, check;
    
    for(i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            temp[i*9+j] = j + 1;
        }
    }

    while(1)
    {
        shuffle(temp, N*N);


        for(i = 0; i < N; i++)
        {
            for(j = 0; j < N; j++)
            {
                shudoku[i][j] = temp[i*9+j];
            }
        }

        for(i = 1; i <= N; i++)
        {
            if(isSafe(shudoku, N, N, i) == 1)
            {
                check = 1;
                break;
            }
        }
        if(check == 1) 
        {
            printf("Fail\n");
            check = 0;
            printer(shudoku);
            sleep(1);
        }
        else
        {
            printf("Pass!\n");
            break;
        }
    }

    return 0;
}

int isSafe(int grid[N][N], int row, int col, int num)
{
    int i, j;
    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (i = 0; i <= 8; i++)
        if (grid[row][i] == num) return 1;

    for (i = 0; i <= 8; i++)
        if (grid[i][col] == num) return 1;

    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num) return 1;

    return 0;
}

int get_rand( int min_val, int max_val ) {
    // srand((unsigned int)time(NULL));
    srand(1);
    return (int)( rand() / ( (double)RAND_MAX+1.0 ) * ( max_val+1 - min_val ) + min_val );
}

/* 要素数sizeの配列arrayの要素をシャッフルする関数 */
void shuffle(int *array, int size)
{
    int i, r, temp;
    srand((unsigned int)time(NULL));

    for (i = 0; i < size; i++) {
        r = get_rand( i, size );
        temp = array[i];
        array[i] = array[r];
        array[r] = temp;
    }
}

// void shuffle(int array[]) {
//     srand((unsigned int)time(NULL));
//     for(int i = 0; i < N*N; i++)
//     {
//         int j = rand()%N*N;
//         int t = array[i];
//         array[i] = array[j];
//         array[j] = t;
//     }
// }

void printer(int array[N][N])
{
    int i, j;

    for(i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}