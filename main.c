#include <stdio.h>
#include <stdlib.h>
#include "ProcessManagement.h"
#include "generate_file.h"
#include "data_processing.h"
#include <string.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <L> <H> <PN>\n", argv[0]);
        return 1;
    }

    int L = atoi(argv[1]);
    int H = atoi(argv[2]);
    int PN = atoi(argv[3]);

    // Validate inputs
    if (L < 10000 || H < 30 || H > 60 || PN < 1) {
        printf("Invalid arguments. Ensure L >= 10000, 30 <= H <= 60, and PN >= 1.\n");
        return 1;
    }

    // Generate the data file with L positive integers and H hidden keys
    generateFile(L, H);

    // Get the size of the data file
    int dataSize = getDataSize("input.txt");
    if (dataSize == -1) {
        perror("Failed to get data size");
        return 1;
    }
    
    // Allocate memory for the data array
    int *data = (int *)malloc(sizeof(int) * dataSize);
    if (!data) {
        perror("Failed to allocate memory for data");
        return 1;
    }

    // Read data from the file into the array
    if (readFileIntoArray("input.txt", data, dataSize) < 0) {
        perror("Failed to read file into array");
        free(data);
        return 1;
    }

    // Initialize global results
    int globalMax = data[0]; // Placeholder initialization
    double globalAvg = 0; // Placeholder initialization
    int totalHiddenKeys = 0; // Placeholder initialization

    // Create a process tree and distribute work
    // Note: You need to implement or adjust createProcessTree according to your project specifics
    // This may involve iterating over segments of data and creating child processes for each segment
    createProcessTree(data, dataSize, PN, 0, 0, 1, &globalMax, &globalAvg, &totalHiddenKeys);


    // After all processes complete
    // Here you would compile results from all child processes
    // This might involve reading results from IPC mechanisms and aggregating them

    // Print final results
    printf("Global Maximum: %d\n", globalMax);
    printf("Global Average: %.2f\n", globalAvg / dataSize); // Assuming the average needs to be calculated
    printf("Total Hidden Keys Found: %d\n", totalHiddenKeys);

    free(data);
    return 0;
}
