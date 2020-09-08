void redirectOutputToFile(char* shellCommand, char* fileName);
void redirectFileToOutput(char* shellCommand, char* fileName);
void processRedirectionNoPipe(char** commandArray, int arrayLength);