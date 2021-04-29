#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Deb(){ //Minimal debug

    printf("[Debug] ");
    system("pause");

}

#include "libraries/types/String.h"
//#include "libraries/types/FileContent.h"
#include "libraries/files/general.h"
#include "libraries/files/gen.c"

#include "debug.c"

void compile(FILE *filePtr, char path[], int isFull);

int main(int argc, char *argv[]){ //You can also use `char *envp[]`

    RegDebStr(); //Start the debugging timer

    srand(time(NULL));

    Debug("Starting the compiler", 0);

    char path[255];
    char *pathPtr = path;

    if (argv[1] != NULL)
        strcpy(path, argv[1]);
    else{
        fgets(path, 255, stdin);
        path[strcspn(path, "\n")] = 0;
    }

    DebugWithPath("Received the path: ", path, 0);

    FILE *mainFilePtr = OpnStrm(path);

    compile(mainFilePtr, path, 1); //Initiate the compiling process

    fclose(mainFilePtr);

    RegDebEnd(); //End the debugging timer

    Deb();

    //exit(EXIT_SUCCESS);

    return 0;

}

#include "preprocessor/checker.h"


void compile(FILE *filePtr, char *path, int isFull){ //Compile a file and it's content

    FILE *tmpFilePtr = genFilStr(); //Create a temporary file for the compiling process

    FileInfo *fileInf = checkFlags(filePtr, path, isFull); //A object that contains the file info!

    printf("\nMode: %c\n", fileInf->mode);
    printf("Is Full: %d\n", fileInf->isFull);
    printf("Current Line Content: %s\n", fileInf->currLineCon);
    printf("Path: %s\n", fileInf->path);

    /*char *compiledCode;
    if(isFull){
        FILE *test = genFilStr();
        return NULL;
    }else{
        return compiledCode;
    }*/

}