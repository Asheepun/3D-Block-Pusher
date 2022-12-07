#include "engine/files.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char *getFileData_mustFree(const char *path, long int *fileSizeOut){

	char *data = NULL;

	FILE *fileHandle = NULL;

	fileHandle = fopen(path, "r");
  
    fseek(fileHandle, 0L, SEEK_END);
    long int fileSize = ftell(fileHandle);
    fseek(fileHandle, 0L, 0);

	data = (char *)malloc(sizeof(char) * fileSize + 1);
	memset(data, 0, sizeof(char) * fileSize + 1);

	for(int i = 0; i < fileSize; i++){
		data[i] = fgetc(fileHandle);
	}

	fclose(fileHandle);

	*fileSizeOut = fileSize;

	return data;

}

FileLine *getFileLines_mustFree(const char *path, int *numberOfLines_out){

	long int dataSize;

	char *data = getFileData_mustFree(path, &dataSize);
	
	int numberOfLines = 1;
	for(int i = 0; i < dataSize; i++){
		if(data[i] == *"\n"){
			numberOfLines++;
		}
	}

	FileLine *lines = (char (*)[256])malloc(numberOfLines * sizeof(FileLine));
	memset(lines, 0, numberOfLines * STRING_SIZE);

	int currentLine = 0;
	int currentChar = 0;
	for(int i = 0; i < dataSize; i++){

		if(data[i] == *"\n"){
			currentLine++;
			currentChar = 0;
		}else{
			lines[currentLine][currentChar] = data[i];
			currentChar++;
		}

	}

	free(data);

	*numberOfLines_out = numberOfLines;

	return lines;

}

void writeDataToFile(const char *path, char *data_p, long int fileSize){

	FILE *fileHandle = NULL;

	fileHandle = fopen(path, "w");

	for(int i = 0; i < fileSize; i++){
		fputc(data_p[i], fileHandle);
	}
	
	fclose(fileHandle);

}
