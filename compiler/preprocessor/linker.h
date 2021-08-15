#include <dirent.h>

#include "path_analyser.h"

void insSubFil(FILE *subTmpPtr, FILE *dstFilPtr, FileInfo *curFile, char *fnlPth, char *srcPth, int rptColSft, int datColSft){

    FileInfo *subFilInf = checkFlags(subTmpPtr, fnlPth, 0); //An object that contains the sub-file info!

    printf("\n[Debug] [Sub] Mode: %c\n", subFilInf->mode);
    printf("[Debug] [Sub] Is Full: %d\n", subFilInf->isFull);
    printf("[Debug] [Sub] Current Line Content: %s\n", subFilInf->currLineCon);
    printf("[Debug] [Sub] Current Line Original Content: %s\n", subFilInf->currOLineCon);
    printf("[Debug] [Sub] Path: %s\n", subFilInf->path);

    if(subFilInf->mode == 'U'){ //This is neither a `.mur` file nor a `.lib.mur` file

        //writeLogLine("Compiler Manager", 2, "Unknown input file extension!", 0, 0, 0);

        rpt(REPORT_CODE_ERROR, //This is an error
        REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
        MSG_PPC_LINKER_INCORRECTFILETYPE, //This is the custom error message (check /compiler/errors/messages.h)
        srcPth, //The source of this error
        curFile->currLine, //The line of this error
        curFile->currCol + rptColSft); //The column the error occurs

        //exit(-1); //Exit! Don't worry about the allocated memory, the system is gonna clean it up.

    }else{

        char *tmpSrcStr = malloc(sizeof(char)*(strlen(srcPth) + strlen(fnlPth) + 4));

        char *tmpDatStr = malloc(sizeof(char)*(strlen(fnlPth) + 26 + 3));

        sprintf(tmpDatStr, "%s <Zx%09X> <Zx%09X>", fnlPth, curFile->currLine, curFile->currCol + datColSft);

        int pthDatId = savDat(DATA_PATH, tmpDatStr);

        free(tmpDatStr);

        sprintf(tmpSrcStr, "<%cx%06X>@%s", DATA_PATH, pthDatId, srcPth);

        ppcRead(subFilInf, dstFilPtr, tmpSrcStr); //Let the preprocessor do its thing, again!

        free(tmpSrcStr);

    }

    fclose(subTmpPtr);

}

void rmvPrcSec(FileInfo **curFile, int *len, int *i, int stmIndx){

    int tmp = ++(*i) - stmIndx;

    shfStr((*curFile)->currLineCon, tmp);

    i = 0;
    *len = strlen((*curFile)->currLineCon);

    if(*len == 1)
        strcpy((*curFile)->currLineCon, FILLER_STRING_CHAR_TYP_STR);

    (*curFile)->currCol += tmp;
    (*curFile)->nextCol = (*curFile)->currCol;

}

FileInfo* chkForPprFunc(FileInfo *curFile, FILE *dstFilPtr, char *srcPth){

    int len = strlen(curFile->currLineCon), inStr = 0, inChr = 0;

    for(int i = 0; i < len; i++){

        //(curFile->currLineCon)[i]; //Current character

        if((i - 1 != -1) ? ((curFile->currLineCon)[i - 1] != '\\') : 1) { //Look for the '\' char

            if(!inChr && (curFile->currLineCon)[i] == '"') //String opening/closing
                inStr = !inStr;

            if(!inStr && (curFile->currLineCon)[i] == '\'') //Char opening/closing
                inChr = !inChr;

        }

        int shdChk = !inStr && !inChr;

        if(shdChk && curFile->mode == 'L'){ //The separate zone is not restricted to the root zone!

            if(i + 2 < len && (curFile->currLineCon)[i] == '<' && (curFile->currLineCon)[i + 1] == '<' && (curFile->currLineCon)[i + 2] == '<'){

                //Start a seperate zone
                curFile->isSptZon = 1;

                char *tmpStr = malloc(sizeof(char)*(28));

                sprintf(tmpStr, "<Zx%09X> <Zx%09X>", curFile->currLine, i + curFile->currCol);

                int tmpId = savDat(DATA_ZONE, tmpStr);

                sprintf(curFile->curZonId, "<%cx%06X>@", DATA_ZONE, tmpId);

                free(tmpStr);

                //fprintf(dstFilPtr, "[zone,%d;%d]->\n", curFile->currLine, curFile->currCol + i);

                strcpy(curFile->currLineCon, FILLER_STRING_CHAR_TYP_STR);

                i = len;

            }

            if(i + 2 < len && (curFile->currLineCon)[i] == '>' && (curFile->currLineCon)[i + 1] == '>' && (curFile->currLineCon)[i + 2] == '>'){

                //End a seperate zone
                curFile->isSptZon = 0;

                //fprintf(dstFilPtr, "[zone]\n");

                strcpy(curFile->currLineCon, FILLER_STRING_CHAR_TYP_STR);

                i = len;

            }

        }
        
        if(shdChk && (curFile->currLineCon)[i] == '{'){ //Beaware, a new zone opening has been detected!

            curFile->curZon++;

        }else if(shdChk && (curFile->currLineCon)[i] == '}'){  //Beaware, a new zone closing has been detected!

            curFile->curZon--;

        }else if(shdChk && curFile->curZon == 0 && (i == 0 || isspace((curFile->currLineCon)[i - 1]) || (curFile->currLineCon)[i - 1] == ';')){

            //You can start looking now!

            if(ENABLE_USING_STATEMENT && i + 5 < len && (curFile->currLineCon)[i] == 'u' && (curFile->currLineCon)[i + 1] == 's' && (curFile->currLineCon)[i + 2] == 'i' && (curFile->currLineCon)[i + 3] == 'n' && (curFile->currLineCon)[i + 4] == 'g' && (curFile->currLineCon)[i + 5] == ' '){

                //The "using" statement has been detected!
                writeLogLine("Preprocessor", 0, "A 'using' statement has been detected!", 1, curFile->currLine, curFile->currCol + i);

                int stmIndx = i, lokForNum = 1, isFsh = 0, pthLen = strlen(MUR_LIBRARIES_DIR) + 1;

                char *libPth = malloc(sizeof(char)*pthLen);
                strcpy(libPth, MUR_LIBRARIES_DIR);

                //fprintf(dstFilPtr, "[{%s},%d;%d]-%s\n", srcPth, curFile->currLine, curFile->currCol, curFile->currLineCon);

                for(i += 6; i < len; i++){

                    if((curFile->currLineCon)[i] == ';'){

                        isFsh = 1;
                        break;

                    }

                    if(!isspace((curFile->currLineCon)[i])) {

                        if(lokForNum && isdigit((curFile->currLineCon)[i])) { //Only accept names that start with letters

                            rpt(REPORT_CODE_ERROR, //This is an error
                            REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                            MSG_PPC_LINKER_USING_NODIGITSATSTART, //This is the custom error message (check /compiler/errors/messages.h)
                            srcPth, //The source of this error
                            curFile->currLine, //The line of this error
                            curFile->currCol + i); //The column the error occurs

                        }else if((curFile->currLineCon)[i] != '.' && (!isalpha((curFile->currLineCon)[i]) && !isdigit((curFile->currLineCon)[i]) && (curFile->currLineCon)[i] != '_')){

                            rpt(REPORT_CODE_ERROR, //This is an error
                            REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                            MSG_PPC_LINKER_USING_RESTRICTEDNAMING, //This is the custom error message (check /compiler/errors/messages.h)
                            srcPth, //The source of this error
                            curFile->currLine, //The line of this error
                            curFile->currCol + i); //The column the error occurs

                        }else if(lokForNum && (curFile->currLineCon)[i] == '.'){

                            rpt(REPORT_CODE_ERROR, //This is an error
                            REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                            MSG_PPC_LINKER_USING_EMPTYSECTION, //This is the custom error message (check /compiler/errors/messages.h)
                            srcPth, //The source of this error
                            curFile->currLine, //The line of this error
                            curFile->currCol + stmIndx); //The column the error occurs

                        }else{

                            pthLen++;

                            libPth = realloc(libPth, sizeof(char)*pthLen);

                            if((curFile->currLineCon)[i] == '.')
                                libPth[pthLen - 2] = PATH_SLASH_TYP_CHAR;
                            else
                                libPth[pthLen - 2] = (curFile->currLineCon)[i];

                            libPth[pthLen - 1] = '\0';

                            //MUR_LIBRARIES_DIR;
                            //DIR* dir = opendir("mydir");

                            //Everything seems to be right!
                            //Start searching for the library

                        }

                        lokForNum = 0;

                        if((curFile->currLineCon)[i] == '.'){ //Look inside another folder now!

                            lokForNum = 1;

                        }

                    }

                    //Use "break" once you reach a ";"

                }

                if(lokForNum){

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_USING_EMPTYSECTION, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + stmIndx); //The column the error occurs

                }else if(!isFsh){

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_USING_SEMICOLON, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + len); //The column the error occurs

                }else{

                    pthLen += strlen(MUR_LIB_FILEEXT);

                    char *tmpLibPth = apdStr(libPth, MUR_LIB_FILEEXT);
                    free(libPth);
                    libPth = pthAnl(COMPILER_START_DIR, tmpLibPth);

                    free(tmpLibPth);

                    FILE *libPtr = fopen(libPth, "r");

                    if(libPtr == NULL){

                        char *tmpMsg = malloc(sizeof(char)*(88 + strlen(libPth) + 1));

                        sprintf(tmpMsg, "There was an attempt to access the file <%s>, which does not exist, or cannot be accessed!", libPth);

                        writeLogLine("Preprocessor", 0, tmpMsg, 0, 0, 0);

                        free(tmpMsg);

                        rpt(REPORT_CODE_ERROR, //This is an error
                        REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                        MSG_PPC_LINKER_USING_INACCESSIBLEFILE, //This is the custom error message (check /compiler/errors/messages.h)
                        srcPth, //The source of this error
                        curFile->currLine, //The line of this error
                        curFile->currCol + stmIndx); //The column the error occurs

                    }else{

                        //Include this file!
                        insSubFil(libPtr, //Sub-file pointer
                                  dstFilPtr, //Tmp file pointer
                                  curFile, //Original file info
                                  libPth, //Sub-file path
                                  srcPth, //Source path
                                  stmIndx, //an added value to the column for the `rpt` function
                                  stmIndx); //an added value to the column for the final data

                    }

                }

                free(libPth);

                rmvPrcSec(&curFile,
                          &len,
                          &i,
                          stmIndx);

                /*int tmp = ++i - stmIndx;

                if(strlen(curFile->currLineCon) - tmp >= 0){

                    shfStr(curFile->currLineCon, tmp);

                }

                i = 0;
                len = strlen(curFile->currLineCon);

                if(len == 1)
                    strcpy(curFile->currLineCon, FILLER_STRING_CHAR_TYP_STR);

                curFile->currCol += tmp;
                curFile->nextCol = curFile->currCol;*/

            }else if(ENABLE_IMPORT_STATEMENT && i + 5 < len && (curFile->currLineCon)[i] == 'i' && (curFile->currLineCon)[i + 1] == 'm' && (curFile->currLineCon)[i + 2] == 'p' && (curFile->currLineCon)[i + 3] == 'o' && (curFile->currLineCon)[i + 4] == 'r' && (curFile->currLineCon)[i + 5] == 't' && ((i + 6 < len) ? ((curFile->currLineCon)[i + 6] == ' ' || (curFile->currLineCon)[i + 6] == '"' || (curFile->currLineCon)[i + 6] == ';') : 1)){

                //The "import" statement has been detected!
                writeLogLine("Preprocessor", 0, "An 'import' statement has been detected!", 1, curFile->currLine, curFile->currCol + i);

                int stmIndx = i, lokForStr = 1, pthLen = 0, isDon = 0, fndCls = 0, fndEnd = 0, isFstNoSpc = 1;

                char *tmpStr = malloc(1*sizeof(char));

                for(i += 7; i < len; i++){

                    if((curFile->currLineCon)[i] == ';'){ //Find the end of this command!

                        fndEnd = 1;
                        break;

                    }

                    if(!isspace((curFile->currLineCon)[i])) {

                        if(isFstNoSpc == 1){

                            isFstNoSpc = 0;

                            if((curFile->currLineCon)[i] != '"'){

                                isFstNoSpc = -1;
                                break;

                            }

                        }

                        if(isDon){

                            exit(-100); //Well, this is not supposed to happen!
                            //This may not be possible, IDK.

                        }

                        if(lokForStr && (curFile->currLineCon)[i] == '"'){

                            lokForStr = 0;

                        }else if((curFile->currLineCon)[i] != '"'){ //Could crash when there's no closing quote

                            tmpStr[pthLen] = (curFile->currLineCon)[i];
                            tmpStr = realloc(tmpStr, (++pthLen + 1)*sizeof(char));

                        }else{ //Well, we're done here!

                            isDon = 1;

                            //Start importing this file!
                            tmpStr[pthLen] = '\0';

                            int orgImpPthLen = strlen(tmpStr); //Save the string length!

                            char *fnlPth = pthAnl(wrkstn.Path, tmpStr);

                            char *tmpMsgStr = malloc((77 + strlen(fnlPth) + 2)*sizeof(char));

                            sprintf(tmpMsgStr, "Importing the content of the file <%s> into the temporary output file.", fnlPth);

                            writeLogLine("Preprocessor", 0, tmpMsgStr, 1, curFile->currLine, stmIndx + curFile->currCol);

                            //

                            //sprintf("<%s>@%s", tmpStr, srcPth);
                            //fnlPth;
                            //
                            FILE *subTmpPtr = fopen(fnlPth, "r");

                            if(subTmpPtr == NULL){

                                char *tmpMsg = malloc(sizeof(char)*(88 + strlen(fnlPth) + 1));

                                sprintf(tmpMsg, "There was an attempt to access the file <%s>, which does not exist, or cannot be accessed!", fnlPth);

                                writeLogLine("Preprocessor", 0, tmpMsg, 0, 0, 0);

                                free(tmpMsg);

                                rpt(REPORT_CODE_ERROR, //This is an error
                                REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                                MSG_PPC_LINKER_IMPORT_INACCESSIBLEFILE, //This is the custom error message (check /compiler/errors/messages.h)
                                srcPth, //The source of this error
                                curFile->currLine, //The line of this error
                                curFile->currCol + i - orgImpPthLen); //The column the error occurs

                            }else{

                                //

                            }

                            insSubFil(subTmpPtr, //Sub-file pointer
                                      dstFilPtr, //Tmp file pointer
                                      curFile, //Original file info
                                      fnlPth, //Sub-file path
                                      srcPth, //Source path
                                      i - orgImpPthLen, //an added value to the column for the `rpt` function
                                      stmIndx); //an added value to the column for the final data

/*                            FileInfo *subFilInf = checkFlags(subTmpPtr, fnlPth, 0); //An object that contains the sub-file info!

                            printf("\n[Debug] [Sub] Mode: %c\n", subFilInf->mode);
                            printf("[Debug] [Sub] Is Full: %d\n", subFilInf->isFull);
                            printf("[Debug] [Sub] Current Line Content: %s\n", subFilInf->currLineCon);
                            printf("[Debug] [Sub] Current Line Original Content: %s\n", subFilInf->currOLineCon);
                            printf("[Debug] [Sub] Path: %s\n", subFilInf->path);

                            if(subFilInf->mode == 'U'){ //This is neither a `.mur` file nor a `.lib.mur` file

                                //writeLogLine("Compiler Manager", 2, "Unknown input file extension!", 0, 0, 0);

                                rpt(REPORT_CODE_ERROR, //This is an error
                                REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                                MSG_PPC_LINKER_IMPORT_INCORRECTFILETYPE, //This is the custom error message (check /compiler/errors/messages.h)
                                srcPth, //The source of this error
                                curFile->currLine, //The line of this error
                                curFile->currCol + i - orgImpPthLen); //The column the error occurs

                                //exit(-1); //Exit! Don't worry about the allocated memory, the system is gonna clean it up.

                            }else{

                                char *tmpSrcStr = malloc(sizeof(char)*(strlen(srcPth) + strlen(fnlPth) + 4));

                                char *tmpDatStr = malloc(sizeof(char)*(strlen(fnlPth) + 26 + 3));

                                sprintf(tmpDatStr, "%s <Zx%09X> <Zx%09X>", fnlPth, curFile->currLine, curFile->currCol + stmIndx);

                                int pthDatId = savDat(DATA_PATH, tmpDatStr);

                                free(tmpDatStr);

                                sprintf(tmpSrcStr, "<%cx%06X>@%s", DATA_PATH, pthDatId, srcPth);

                                ppcRead(subFilInf, dstFilPtr, tmpSrcStr); //Let the preprocessor do its thing, again!

                                free(tmpSrcStr);

                            }

                            fclose(subTmpPtr);*/

                            sprintf(tmpMsgStr, "Finished importing the content of the file <%s> into the temporary output file.", fnlPth);

                            writeLogLine("Preprocessor", 0, tmpMsgStr, 1, curFile->currLine, stmIndx + curFile->currCol);

                            free(tmpMsgStr);
                            free(fnlPth);

                        }

                    }
                    
                    //Use "break" once you reach a ";"

                }

                if(isFstNoSpc == -1){

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_IMPORT_NOSTRINGINPUT, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + i); //The column the error occurs

                }else if(!isDon && lokForStr){

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_IMPORT_NOINPUT, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + stmIndx); //The column the error occurs

                }else if(!isDon){

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_IMPORT_CLOSINGQUOTE, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + i); //The column the error occurs

                }else if(!fndEnd || (!fndEnd && sizeof(tmpStr) == sizeof(char))) {

                    rpt(REPORT_CODE_ERROR, //This is an error
                    REPORT_SECTION_PREPROCESSOR, //The error was detected by the preprocessor
                    MSG_PPC_LINKER_IMPORT_SEMICOLON, //This is the custom error message (check /compiler/errors/messages.h)
                    srcPth, //The source of this error
                    curFile->currLine, //The line of this error
                    curFile->currCol + i); //The column the error occurs

                }

                free(tmpStr); //Free the path

                rmvPrcSec(&curFile,
                          &len,
                          &i,
                          stmIndx);

                /*int tmp = ++i - stmIndx;

                if(strlen(curFile->currLineCon) - tmp >= 0){

                    shfStr(curFile->currLineCon, tmp);

                }

                i = 0;
                len = strlen(curFile->currLineCon);

                if(len == 1)
                    strcpy(curFile->currLineCon, FILLER_STRING_CHAR_TYP_STR);

                curFile->currCol += tmp;
                curFile->nextCol = curFile->currCol;*/

            }else if(ENABLE_DEFINE_STATEMENT && i + 6 < len && (curFile->currLineCon)[i] == 'd' && (curFile->currLineCon)[i + 1] == 'e' && (curFile->currLineCon)[i + 2] == 'f' && (curFile->currLineCon)[i + 3] == 'i' && (curFile->currLineCon)[i + 4] == 'n' && (curFile->currLineCon)[i + 5] == 'e' && (curFile->currLineCon)[i + 6] == ' '){

                //The "define" statement has been detected!
                writeLogLine("Preprocessor", 0, "A 'define' statement has been detected!", 1, curFile->currLine, curFile->currCol + i);

            }else if(ENABLE_SETSIZE_STATEMENT && i + 7 < len && (curFile->currLineCon)[i] == 's' && (curFile->currLineCon)[i + 1] == 'e' && (curFile->currLineCon)[i + 2] == 't' && (curFile->currLineCon)[i + 3] == 's' && (curFile->currLineCon)[i + 4] == 'i' && (curFile->currLineCon)[i + 5] == 'z' && (curFile->currLineCon)[i + 6] == 'e' && (curFile->currLineCon)[i + 7] == ' '){

                //The "setsize" statement has been detected!
                writeLogLine("Preprocessor", 0, "A 'setsize' statement has been detected!", 1, curFile->currLine, curFile->currCol + i);

            }

        }

    }

    /*FileInfo *subFilInf = checkFlags(filePtr, full_path, 0); //An object that contains the sub-file info!

    fprintf(dstFilPtr, "[{curFil@%s},%d;%d]->%s\n", srcPth, subFilInf->currLine, subFilInf->currCol, subFilInf->currLineCon);

    writeLogLine("Preprocessor", 0, "Inserted the filtered code into the temporary output file.", 0, 0, 0);*/

    return curFile;

}