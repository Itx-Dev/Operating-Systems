#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>


int throwError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

/**
 * @brief Break given command down into an array of strings and return the array of strings
 * 
 * @param givenLine 
 * @param storageArray 
 * @return char** Array of Strings with current line
 */
char** parseStringIntoArray(char *givenLine, char** storageArray) {
    char* splitString = malloc(32 * sizeof(char));
    int argumentIndex = 0;

    // Split String into tokens by spaces
    splitString = strtok(givenLine, " \t>");

    // Parse string given by user or batch file and split into string array
    while (splitString != NULL) {

        // Store split string into a string array
        storageArray[argumentIndex] = splitString;

        // Test for NULL pointer
        splitString = strtok(NULL, " \t>");
        argumentIndex++;
    }

    return storageArray;
}

/**
 * @brief Remove new line character from string.
 * @param string 
 */
int removeNewLine(char* string) {
    size_t length = strlen(string);
    if (length > 0 && string[length - 1] == '\n') {
        string[length - 1] = '\0';
    }
    return 0;
}

// Function to remove trailing spaces from a string
void removeTrailingSpaces(char* str) {
    int length = strlen(str);
    int i;

    // Find the index of the last non-space character
    for (i = length - 1; i >= 0; i--) {
        if (str[i] != ' ') {
            break;
        }
    }

    // Null-terminate the string at the last non-space character
    str[i + 1] = '\0';
}

int searchForParallelCommands(char* readInLine) {
    int ampersandCount = 0;
    int commandIndex = 0;
    // Look for ampersand
    for (int i = 0; i < strlen(readInLine); i++) {
        if (readInLine[i] == '&') {
            if (i == 0) {
                return -1;
            } else {
                ampersandCount++;
            }
        }
    }

    // If ampersand found return 1
    if (ampersandCount == 0) {
        return 0;
    } else if (ampersandCount > 0) {
        return 1;
    }
}

/**
 * @brief Searches for redirection symbol (>)
 * @param outputString, the address of a pointer where to store the value of the string after the (>) symbol
 * @param inputString, the address of a pointer where to store the value of the string before the (>) symbol
 * @param readInLine, the current line read in by the shell
 * @return int, 0 if no (>) symbol found, -1 if error occurs, else the index of the symbol
 */
int searchForRedirection(char* readInLine, char** outputString, char** inputString) {
    // Remove spaces in line read in
    removeNewLine(readInLine);

    int redirectionIndex = 0;
    int redirectionBoolean = 0;
    int pathIndex = 0;
    int argIndex = 0;
    char* input = malloc(32 * sizeof(char));
    char* output = malloc(32 * sizeof(char));
    
    removeTrailingSpaces(readInLine);
    // Find Redirection Symbol
    for (int i = 0; i <= strlen(readInLine) - 1; i++) {
        // Update new string with every character after the redirection symbol

        // If redirection symbol found set index and set bool value to true
        if (readInLine[i] == '>') {
            // Set Index of redirection symbols 
            if (redirectionBoolean == 1) {
                throwError();
                return -1;
            }

            redirectionIndex = i;
            // Set that (>) was found
            redirectionBoolean = 1;
            // Reset index to store args after (>) symbol
            pathIndex = 0;
        } 
        // Store command before (>) symbol
        else if (redirectionBoolean == 0) {
            // Store string
            input[pathIndex] = readInLine[i];
            pathIndex++;
        } 
        // Store args after (>) symbol
        else if (redirectionBoolean == 1) {
            // If the first directed output file string ends and there is still more characters
            if (readInLine[i] == ' ' && strlen(output) != 0) {
                return -1;
            }
            output[pathIndex] = readInLine[i];
            pathIndex++;
        }
    }

    // If > found but no output given throw error 
    if (redirectionBoolean == 1 && strlen(output) == 0) {
        return -1;
    }
    removeNewLine(input);

    // Update string passed in through parameter
    *inputString = input;
    *outputString = output;

    // Free memory
    input = NULL; output = NULL;
    free(input); free(output);

    return redirectionIndex;
}

/**
 * @brief Replace every string in string array with NULL
 * 
 * @param directory 
 * @param amountOfSearchPaths 
 * @return char** 
 */
char** clearDirectories(char** directory, int amountOfSearchPaths) {
    // Set entire array to NULL
    for (int i = 0; i < amountOfSearchPaths; i++) {
        directory[i] = NULL;
    }
    return directory;
}