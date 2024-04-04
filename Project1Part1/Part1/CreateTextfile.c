#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    srand(time(NULL));   // Initialization, should only be called once.
    int r,j=0,k=0;
    int* value;
    long long int L;  // Change data type of L

    // Accept input
    printf("Input L : ");
    scanf("%lld", &L);  // Use "%lld" to read long long int

    // Create L random numbers
    value = calloc(L,sizeof(int));
    while (j < L)
    {
        r = rand() % 101; // Random number from 0-100, using rand() % 101 instead of rand() & 100
        value[j++] = r;
    }
    // Insert -1 into random places 50 times
    while(k < 50){
        r = rand() % L;   // Random number from 0 to L-1, using rand() % L instead of rand() % (L-1)
        if (value[r] != -1){  // Cannot input into same place twice
            value[r] = -1;
            k++;
        }
    }
    //Copy to file
    FILE* fp;
    fp = fopen("numFile.txt","w");
    if (fp == NULL) {  // Check if the file could be opened
        printf("Error opening file!\n");
        return 1;
    }
    for (long long int i = 0; i < L; i++){  // Use long long int in the loop
        fprintf(fp, "%d", value[i]);
        fputc(' ', fp);
    }
    fclose(fp);
    free(value);

    return 0;
}

