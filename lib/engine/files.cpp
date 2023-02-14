#include "engine/files.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char *getFileData_mustFree(const char *path, long int *fileSizeOut){

	//printf("\nIN FILE READ\n");

	FILE *fileHandle = fopen(path, "rb");
	fseek(fileHandle, 0, SEEK_END);
	long fileSize = ftell(fileHandle);
	fseek(fileHandle, 0, SEEK_SET);

	char *data = (char *)malloc(fileSize + 1);
	fread(data, fileSize, 1, fileHandle);
	fclose(fileHandle);

	data[fileSize] = 0;

	*fileSizeOut = fileSize;

	return data;

	/*
	FILE *fileHandle = NULL;

	fileHandle = fopen(path, "r");

	char currentChar = 0;

	int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	int currentBufferIndex = 0;
	int dataSize = 0;

	char *data = (char *)malloc(1);

	while(!feof(fileHandle)){

		currentChar = fgetc(fileHandle);

		buffer[currentBufferIndex] = currentChar;
		
		currentBufferIndex++;
		dataSize++;

		if(currentBufferIndex == BUFFER_SIZE){
			currentBufferIndex = 0;
			data = (char *)realloc(data, dataSize);
			memcpy(data + (dataSize - BUFFER_SIZE), buffer, BUFFER_SIZE);
		}

	}

	data = (char *)realloc(data, dataSize + 1);
	memcpy(data + (dataSize - currentBufferIndex), buffer, currentBufferIndex);
	data[dataSize] = *"\0";

	*fileSizeOut = dataSize;
	*/
  
	/*
    fseek(fileHandle, 0L, SEEK_END);
    long int fileSize = ftell(fileHandle);
    fseek(fileHandle, 0L, 0);

	data = (char *)malloc(sizeof(char) * fileSize + 2);
	memset(data, *"\0", sizeof(char) * fileSize + 2);

	for(int i = 0; i < fileSize; i++){
		data[i] = fgetc(fileHandle);
		//printf("char!\n");
		//printf("%c\n", data[i]);
		//printf("%i\n", data[i]);
	}

	fclose(fileHandle);

	//data[fileSize] = *"\0";

	*fileSizeOut = fileSize;
	*/

	//printf("\nOUT OF FILE READ\n");

	//return data;

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

	FileLine *lines = (FileLine *)malloc(numberOfLines * sizeof(FileLine));
	memset(lines, 0, numberOfLines * sizeof(FileLine));

	int currentLine = 0;
	int currentChar = 0;
	for(int i = 0; i < dataSize; i++){

		if(data[i] == *"\n"){
			if(lines[currentLine][currentChar - 1] == *"\r"){
				lines[currentLine][currentChar - 1] = *"\0";
			}
			currentLine++;
			currentChar = 0;
		}else{
			if(currentChar < STRING_SIZE){
				lines[currentLine][currentChar] = data[i];
			}
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
