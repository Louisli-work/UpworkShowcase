#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <stdlib.h>


/*
Program Function Description:
This program is designed specifically for text files and offers the following functionalities:

Batch processing of file encryption and decryption.
Renaming encrypted or decrypted files.
Deleting old files after encryption or decryption.
Known Limitation:
Currently, the tool loses the saved file name mapping after the user exits the program. This issue may affect subsequent encryption or decryption operations in some cases.

Planned Improvement:
We are actively working on an optimization to ensure the persistence of the file name mapping, preventing data loss even after the program exits.
*/


void print_help();
void print_tips();
char* getFolderPath(char* dir_path);
void getPassWord(unsigned int* password);
char* createSearchPath(char* path);
char** getFileName(int* total, char* path);
char** read_src_file(char* path, char** originalName, int total, unsigned int* length);

// Encryption
char** fileEncryption(char** fileContent, unsigned int* length, int total, unsigned int password);
// Decryption
char** fileDecryption(char** fileContent, unsigned int* length, int total, unsigned int password);
char** createNewPath(char* path, int total, char** originalName, char** copyOriginalName, int userSelect);
void saveFile(int total, unsigned int* length, char** fileContent, char** newFilePath);
// Free memory
void freeSpace(char** space, int total);
void deleteFile(char** newPath, char* folderPath, char** originalName, int total);


char** copyOriginalName = NULL;

enum userOption
{
	ENCRYPTION = 1,
	DECRYPTION,
	EXIT
};

int main()
{
	while (1)
	{
		int userSelect = 0;
		unsigned int passWord = 0;
		char** originalName = NULL;
		int totalFile = 0;
		unsigned int fileLength[100] = { 0 };

		// Original file content
		char** src_file_content = NULL;

		// New file path
		char** newFilePath = NULL;

		print_help();
		
		// Get user selection
		scanf("%d", &userSelect);

		getchar();

		// The path where the file is located
		char* folderPath = NULL;
		// Search Path
		char* searchPath = NULL;

		switch (userSelect)
		{
		case ENCRYPTION:
		{
			print_tips();
			folderPath = getFolderPath(folderPath);

			printf("Please enter the encrypted password (Only number)\n");
			getPassWord(&passWord);

			searchPath = createSearchPath(folderPath);

			originalName = getFileName(&totalFile, searchPath);

			// Backup file name
			copyOriginalName = (char**)calloc(totalFile, sizeof(char*));
			if (!copyOriginalName)
			{
				perror("calloc\n");
				return NULL;
			}

			for (int i = 0; i < totalFile; i++)
			{
				copyOriginalName[i] = (char*)calloc(strlen(originalName[i]), sizeof(char));
				if (!copyOriginalName)
				{
					perror("calloc\n");
					return NULL;
				}

				strcpy(copyOriginalName[i], originalName[i]);
			}

			src_file_content = read_src_file(folderPath, originalName, totalFile, fileLength);

			src_file_content = fileEncryption(src_file_content, fileLength, totalFile, passWord);

			newFilePath = createNewPath(folderPath, totalFile, originalName,copyOriginalName, userSelect);

			saveFile(totalFile, fileLength, src_file_content, newFilePath);

			deleteFile(newFilePath, folderPath, originalName, totalFile);
		
		}
		free(folderPath);
		free(searchPath);

		freeSpace(originalName, totalFile);
		freeSpace(src_file_content, totalFile);
		freeSpace(newFilePath, totalFile);
			break;

		case DECRYPTION:
		{
			print_tips();
			folderPath = getFolderPath(folderPath);

			printf("Please enter the decrypted password (Only number)\n");
			getPassWord(&passWord);

			searchPath = createSearchPath(folderPath);

			originalName = getFileName(&totalFile, searchPath);

			src_file_content = read_src_file(folderPath, originalName, totalFile, fileLength);

			src_file_content = fileDecryption(src_file_content, fileLength, totalFile, passWord);

			newFilePath = createNewPath(folderPath, totalFile, originalName, copyOriginalName, userSelect);

			saveFile(totalFile, fileLength, src_file_content, newFilePath);

			deleteFile(newFilePath, folderPath, originalName, totalFile);
		}

		free(folderPath);
		free(searchPath);

		freeSpace(originalName, totalFile);
		freeSpace(src_file_content, totalFile);
		freeSpace(newFilePath, totalFile);
			break;

		case EXIT:

			if (folderPath != NULL)
			{
				free(folderPath);
			}

			if (searchPath != NULL)
			{
				free(searchPath);
			}

			freeSpace(originalName, totalFile);
			freeSpace(src_file_content, totalFile);
			freeSpace(newFilePath, totalFile);
			freeSpace(copyOriginalName, totalFile);

			printf("The program exited successfully\n");

			return 0;

		default:
			printf("Tip: The program does not seem to have this option. Please try again.\n\n");
			break;
		}

	}

	return 0;
}


void print_help()
{
	printf("************** 1. File encryption **************\n");
	printf("************** 2. File decryption **************\n");
	printf("************** 3. Exit Program    **************\n");

	return;
}

char* getFolderPath(char* dir_path)
{
	dir_path = (char*)calloc(MAX_PATH, sizeof(char));
	if (dir_path == NULL)
	{
		perror("calloc\n");
		return NULL;
	}

	fgets(dir_path, MAX_PATH, stdin);
	dir_path[strlen(dir_path) - 1] = '\0';

	return dir_path;
}

void print_tips()
{
	printf("Please enter the folder path of the file to be encrypted/decrypted. Please use \\\\ as the path\n");
	printf(">>>: ");
}

void getPassWord(unsigned int* password)
{
	scanf("%u", &(*password));

	return;
}


char* createSearchPath(char* path)
{
	char* searchPath = (char*)calloc(MAX_PATH, sizeof(char));
	if (!searchPath)
	{
		perror("calloc\n");
		return NULL;
	}

	snprintf(searchPath, MAX_PATH, "%s\\*", path);

	return searchPath;
}

char** getFileName(int* total, char* path)
{
	char** orgName = NULL;
	(*total) = 0;

	orgName = (char**)calloc(100, sizeof(char));
	if (orgName == NULL)
	{
		perror("calloc\n");
		return NULL;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFileA(path, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE || hFind == ERROR_FILE_NOT_FOUND)
	{
		printf("Unable to open directory.\n");
		return NULL;
	}

	do
	{
		if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
		{
			continue;
		}

		if (*total < 100)
		{
			orgName[*total] = (char*)calloc(1, strlen(findFileData.cFileName) + 1);
			if (orgName[*total] == NULL)
			{
				perror("calloc\n");
				return NULL;
			}

			strcpy(orgName[*total], findFileData.cFileName);

			(*total)++;
		}

	} while (FindNextFileA(hFind, &findFileData) != 0);

	FindClose(hFind);

	return orgName;
}


char** read_src_file(char* path, char** originalName, int total, unsigned int* length)
{
	unsigned long fileLength = 0;

	char** searchPath = (char**)calloc(total, sizeof(char*));
	if (!searchPath)
	{
		perror("calloc\n");
		return NULL;
	}

	for (int i = 0; i < total; i++)
	{
		searchPath[i] = (char*)calloc(MAX_PATH, sizeof(char));
		if (searchPath[i] == NULL)
		{
			perror("calloc\n");
			return NULL;
		}

		snprintf(searchPath[i], MAX_PATH, "%s\\%s", path, originalName[i]);
	}

	char** fileContent = (char**)calloc(total, sizeof(char*));
	if (!fileContent)
	{
		perror("calloc\n");
		return NULL;
	}

	for (int i = 0; i < total; i++)
	{
		FILE* fp = NULL;
		fp = fopen(searchPath[i], "r");
		if (fp == NULL)
		{
			perror("fopen\n");
			return NULL;
		}

		fseek(fp, 0, SEEK_END);
		fileLength = ftell(fp);
		//fseek(fp, 0, SEEK_SET);
		rewind(fp);

		length[i] = fileLength;

		fileContent[i] = (char*)calloc(fileLength + 1, sizeof(char));

		if (fileContent[i] == NULL)
		{
			perror("calloc\n");
			return NULL;
		}

		fread(fileContent[i], sizeof(char), fileLength, fp);

		fclose(fp);
		
	}

	freeSpace(searchPath, total);

	return fileContent;
}

char** fileEncryption(char** fileContent, unsigned int* length, int total, unsigned int password)
{
	for (int i = 0; i < total; i++)
	{
		for (unsigned int j = 0; j < length[i]; j++)
		{
			fileContent[i][j] += password;
		}

	}

	return fileContent;
}

char** fileDecryption(char** fileContent, unsigned int* length, int total, unsigned int password)
{
	for (int i = 0; i < total; i++)
	{
		for (unsigned int j = 0; j < length[i]; j++)
		{
			fileContent[i][j] -= password;
		}
	}
	
	return fileContent;
}

char** createNewPath(char* path, int total, char** originalName,char** copyOriginalName, int userSelect)
{
	char** newPath = (char**)calloc(total, sizeof(char*));
	if (newPath == NULL)
	{
		perror("calloc\n");
		return NULL;
	}

	for (int i = 0; i < total; i++)
	{
		newPath[i] = (char*)calloc(MAX_PATH, sizeof(char));
		if (!newPath[i])
		{
			perror("calloc\n");
			return NULL;
		}

		if (userSelect == ENCRYPTION)
		{
			snprintf(newPath[i], MAX_PATH, "%s\\Encryption_%s", path, originalName[i]);
		}
		else if (userSelect == DECRYPTION)
		{
			snprintf(newPath[i], MAX_PATH, "%s\\Decryption_%s", path, copyOriginalName[i]);
		}

		//printf("Path = %s\n", newPath[i]);

	}

	return newPath;
}


void saveFile(int total, unsigned int* length, char** fileContent, char** newFilePath)
{
	for (int i = 0; i < total; i++)
	{
		FILE* fp = NULL;
		fp = fopen(newFilePath[i], "w");
		if (fp == NULL)
		{
			perror("fopen\n");
			return NULL;
		}

		fwrite(fileContent[i], sizeof(char), length[i], fp);

		fclose(fp);
	}
	
	return;
}

void freeSpace(char** space, int total)
{
	if (space != NULL)
	{
		for (int i = 0; i < total; i++)
		{
			free(space[i]);
		}

		free(space);
	}

	return;
}

void deleteFile(char** newPath, char* folderPath, char** originalName, int total)
{
	char** oldpath = (char**)calloc(total, sizeof(char*));
	if (oldpath == NULL)
	{
		perror("calloc\n");
		return;
	}

	for (int i = 0; i < total; i++)
	{
		oldpath[i] = (char*)calloc(MAX_PATH, sizeof(char));
		if (oldpath[i] == NULL)
		{
			perror("calloc\n");
			return;
		}
		// Build the old file path that needs to be deleted
		snprintf(oldpath[i], MAX_PATH, "%s\\%s", folderPath, originalName[i]);

		if (strcmp(newPath[i], oldpath[i]) != 0)
		{
			if (remove(oldpath[i]) != 0)
			{
				perror("remove()\n");
			}
		}
		// After the file is deleted, the corresponding array element space is released
		free(oldpath[i]);

	}
	// Release array
	free(oldpath);

	return;
}