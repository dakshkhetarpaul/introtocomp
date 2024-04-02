#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

// Function to get the size of the data file
int getDataSize(const char *filename);

// Function to read data from the file into an array
int readFileIntoArray(const char *filename, int *data, int size);

#endif
 
