#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <time.h>	

/*
Goal: Rename files in batches.
Function: Supports two renaming modes, timestamp naming and number naming.
Usage scenario: For example, for users who need to batch process a large number of files, this program can improve work efficiency.
*/


void fileRename(const char* path, int num);

int totalFileAmount = 0;

enum renameFormat
{
	TIMESTAMP = 1,
	NUMBER
};

int main()
{
	int optionNumber = 0;
	printf("Option 1: Timestamp naming\nOption 2: Number naming\n");
	scanf("%d", &optionNumber);
	
	// Eliminate newline characters from scanf
	getchar();
	
	printf("Please enter the folder path where the file(s) you want to modify is located. (use \\\\ for path:)\n");
	printf(">>>: ");

	char* searchPath = (char*)calloc(MAX_PATH, sizeof(char));
	if (!searchPath)
	{
		perror("calloc");
		printf("calloc line: %d\n", __LINE__);
		return -1;
	}

	fgets(searchPath, MAX_PATH, stdin);
	searchPath[strlen(searchPath) - 1] = '\0';

	fileRename(searchPath, optionNumber);

	free(searchPath);
	return 0;
}

void fileRename(const char* path, int num)
{
	char* saveSearchPath = (char*)calloc(MAX_PATH, sizeof(char));
	if (!saveSearchPath)
	{
		perror("calloc\n");
		return;
	}

	snprintf(saveSearchPath, MAX_PATH, "%s\\*", path);

	printf("Searching in directory: %s\n", saveSearchPath);

	WIN32_FIND_DATA findFileData;

	HANDLE hFind = FindFirstFileA(saveSearchPath, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Unable to open directory.\n");
		printf("Error code: %d\n", GetLastError());
		return;
	}

	do
	{
		// Ignore . and .. directories
		if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
		{
			continue;
		}

		char* oldNamePath = (char*)calloc(MAX_PATH, sizeof(char));
		if (!oldNamePath)
		{
			perror("calloc\n");
			return;
		}

		snprintf(oldNamePath, MAX_PATH, "%s\\%s", path, findFileData.cFileName);
		
		printf("Old File Name: %s\n", findFileData.cFileName);

		char* newName = (char*)calloc(MAX_PATH, sizeof(char));	
		if (!newName)
		{
			perror("calloc\n");
			printf("Line: %d\n", __LINE__);
			return;
		}

		char* newPath = (char*)calloc(MAX_PATH, sizeof(char));
		if (!newPath)
		{
			perror("calloc\n");
			printf("Line: %d\n", __LINE__);
			return;
		}

		switch (num)
		{
		case TIMESTAMP:
		{
			// Get file time and generate timestamp
			FILETIME fileTime;
			// Get the current system time and store it in the FILETIME structure (initialization)
			GetSystemTimeAsFileTime(&fileTime);

			// Convert FILETIME to local time (avoids time zone issues)
			FILETIME locatFileTime;
			FileTimeToLocalFileTime(&fileTime, &locatFileTime);

			SYSTEMTIME systemTime;
			FileTimeToSystemTime(&locatFileTime, &systemTime);

			snprintf(newName, MAX_PATH, "%d-%02d-%02d_%02d-%02d-%02d_", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

			strncat(newName, findFileData.cFileName, MAX_PATH);

			printf("New File Name: %s\n\n", newName);

			snprintf(newPath, MAX_PATH, "%s\\%s", path, newName);

			if (MoveFileA(oldNamePath, newPath) != 0)
			{
				printf("Renamed: %s -> %s\n", oldNamePath, newPath);
			}
			else
			{
				printf("Failed to rename. Error Code: %d\n", GetLastError());
			}
			
		}

			break;

		case NUMBER:
		{
			totalFileAmount += 1;

			snprintf(newName, MAX_PATH, "%02d_", totalFileAmount);

			strncat(newName, findFileData.cFileName, MAX_PATH);

			printf("New File Name: %s\n\n", newName);

			snprintf(newPath, MAX_PATH, "%s\\%s", path, newName);

			printf("New: %s\n", newPath);

			if (MoveFileA(oldNamePath, newPath) != 0)
			{
				printf("Renamed: %s -> %s\n", oldNamePath, newPath);
			}
			else
			{
				printf("Failed to rename. Error Code: %d\n", GetLastError());
			}

		}

			break;

		default:
			printf("Please select option 1 or option 2\n");
			free(newName);
			free(newPath);
			break;
		}

		// If default has already released space
		// No need to release here
		// Prevent multiple releases
		if (newName != NULL)
		{
			free(newName);
			newName = NULL;
		}

		if (newPath != NULL)
		{
			free(newPath);
			newPath = NULL;
		}

		free(oldNamePath);
		
	} while (FindNextFileA(hFind, &findFileData) != 0);		

	free(saveSearchPath);

	// Close Directory
	FindClose(hFind);

	return;
}

