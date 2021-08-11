#include "../libraries/regex/reg.h"
#include "chk.h"

//1000 `value` 0 0 0 0 | 0 0 0 0 <0.'file1'.'file2'> 0x000000000 1x000000000

FILE* lexProc(TmpFileStruc cFileObj){

    char *tmpLexPthStr = apdStr(cFileObj.pth, ".lxic");

    FILE *lexFil = fopen(tmpLexPthStr, "w"); //Create a new lexer file in "write mode"

    char *tmpStr = malloc(sizeof(char)*MAX_LINE_LENGTH), *curLin = NULL;

    fgets(tmpStr, MAX_LINE_LENGTH, cFileObj.ptr); //Get the first line
    if(tmpStr[strlen(tmpStr) - 1] == '\n')
        tmpStr[strlen(tmpStr) - 1] = '\0'; //Remove the new line character (\n), and replace it with a line end character (\0)!

    while(getStrIndx(tmpStr, "[FileEnd]") != 0){

        curLin = getStrPrt(tmpStr, getStrIndx(tmpStr, "]") + 3, strlen(tmpStr), 0); //Get the current line content

        char *curFil = getStrPrt(tmpStr, 2, getStrIndx(tmpStr, "}"), 0);
        //"[{main},%d;%d]->%s

        char *tmpColStr = getStrPrt(tmpStr, getStrIndx(tmpStr, ";") + 1, getStrIndx(tmpStr, "]"), 0), *tmpLinStr = getStrPrt(tmpStr, getStrIndx(tmpStr, ",") + 1, getStrIndx(tmpStr, ";"), 0);

        int col = atoi(tmpColStr), lin = atoi(tmpLinStr);

        free(tmpColStr);
        free(tmpLinStr);

        int newLin = 1, lopLen = strlen(curLin);

        for(int i = 0; i < lopLen; i++, col++){ //Use this loop to scan every character one by one!

            char currChar = curLin[i];
            int whtSpcBef = (i != 0) ? (isspace(curLin[i - 1]) != 0) : 0;

            if(isspace(currChar)) { //Whitespace!

                //Do nothing!

            } else if(regChk("[.0-9]", &currChar)) { //Number

                int alowDot = 1;

                if(curLin[i] == '.' && !isdigit(curLin[i + 1])){

                    //false alarm
                    fprintf(lexFil, "%d `%c` %d %d %d 0 | %d 0 0 0 <%s> 0x%09X 1x%09X\n", LEXER_OPERATOR, curLin[i], newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), !alowDot, "0", lin, col); //This is an operator!

                }else{

                    if(curLin[i] == '.')
                        alowDot = 0;

                    fprintf(lexFil, "%d `%c", LEXER_NUMBER, curLin[i]);

                    int delt = 1;

                    while(delt + i < strlen(curLin) && (isdigit(curLin[i + delt]) || ((alowDot) ? curLin[i + delt] == '.' : 0))){

                        fprintf(lexFil, "%c", curLin[i + delt]);

                        if(curLin[i + delt] == '.')
                            alowDot = 0;

                        delt++;

                    }

                    delt--;

                    i += delt;

                    fprintf(lexFil, "` %d %d %d 0 | %d 0 0 0 <%s> 0x%09X 1x%09X\n", newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), !alowDot, "0", lin, col);
                    //                    ^ if this value is set to '0', then this number is an integer
                    //                      if it's set to '1', then this number is of the type double/float

                    col += delt;

                }


            } else if(curLin[i] == '"') { //String!

                int delt = 1;

                fprintf(lexFil, "%d `", LEXER_STRING);

                while(i + delt < strlen(curLin) && (curLin[i + delt] != '"' || curLin[i + delt - 1] == '\\')){

                    fprintf(lexFil, "%c", curLin[i + delt]);

                    delt++;

                }

                col++;
                i += delt;

                fprintf(lexFil, "` %d %d %d 0 | 0 0 0 0 <%s> 0x%09X 1x%09X\n", newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), "0", lin, col);

                col += delt - 1;

            } else if(curLin[i] == '\'') { //Character!

                int delt = 1;

                fprintf(lexFil, "%d `", LEXER_CHAR);

                while(i + delt < strlen(curLin) && (curLin[i + delt] != '\'' || curLin[i + delt - 1] == '\\')){

                    fprintf(lexFil, "%c", curLin[i + delt]);

                    delt++;

                }

                col++;
                i += delt;

                fprintf(lexFil, "` %d %d %d 0 | %d 0 0 0 <%s> 0x%09X 1x%09X\n", newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), (delt == 1) ? 0 : ((delt == 2) ? 1 : ((delt == 3 && curLin[i - 2] == '\\') ? 2 : 3)), "0", lin, col);

                col += delt - 1;

            } else if((curLin[i] == 'f' && curLin[i + 1] == 'a' && curLin[i + 2] == 'l' && curLin[i + 3] == 's' && curLin[i + 4] == 'e' && !isThrMor(curLin[i + 5])) || (curLin[i] == 't' && curLin[i + 1] == 'r' && curLin[i + 2] == 'u' && curLin[i + 3] == 'e' && !isThrMor(curLin[i + 4]))){ //Boolean

                fprintf(lexFil, "%d `%s` %d %d %d 0 | 0 0 0 0 <%s> 0x%09X 1x%09X\n", LEXER_BOOLEAN, (curLin[i] == 'f') ? "false" : "true", newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), "0", lin, col); //This is an operator!

                if(curLin[i] == 'f'){

                    i += 4;

                    col += 4;

                }else{

                    i += 3;

                    col += 3;

                }

            } else if(isSyb(currChar)) { //Symbol

                fprintf(lexFil, "%d `%c", LEXER_SYMBOL, curLin[i]);

                int delt = 1;

                while(i + delt < strlen(curLin) && isSybE(curLin[i + delt])){

                    fprintf(lexFil, "%c", curLin[i + delt]);

                    delt++;

                }

                delt--;

                i += delt;

                fprintf(lexFil, "` %d %d %d 0 | 0 0 0 0 <%s> 0x%09X 1x%09X\n", newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), "0", lin, col);

                col += delt;

            } else if(isKnwnSpclChr(currChar)) { //Operator!

                fprintf(lexFil, "%d `%c` %d %d %d 0 | 0 0 0 0 <%s> 0x%09X 1x%09X\n", LEXER_OPERATOR, curLin[i], newLin, whtSpcBef, (isspace(curLin[i + 1]) != 0), "0", lin, col);

            } else {

                writeLogLine("Lexer", 2, "An unknown character has been detected!", 1, lin, col);
                exit(2001);

            }

            //fprintf(lexFil, "%d `%s` 0 0 0 0 | 0 0 0 0 <%s> 0x%09X 1x%09X\n", 1000, "", "0", 0, 0);
            //symbol -> 1001, string -> 1002, char -> 1003, number -> 1004, operator -> 1005
            //LEXER_SYMBOL, LEXER_STRING, LEXER_CHAR, LEXER_NUMBER, LEXER_OPERATOR
            //1000 `[value]` 0 0 0 0 | 0 0 0 0 <[file]> 0x000000000 1x000000000
            //1000 `[value]` 0 0 0 0 | 0 0 0 0 <[file]> 0x000000000 1x000000000
            //1000 `[value]` 0 0 0 0 | 0 0 0 0 <[file]> 0x000000000 1x000000000
            //^typ ^value    ^scrFile ^srcLine(0) ^srcColumn(1)

            if(newLin)
                newLin = 0;

        }

        //Get the next line!
        fgets(tmpStr, MAX_LINE_LENGTH, cFileObj.ptr);

        if(tmpStr[strlen(tmpStr) - 1] == '\n')
            tmpStr[strlen(tmpStr) - 1] = '\0'; //Remove the new line character (\n), and replace it with a line end character (\0)!

        free(curFil);
        free(curLin);

    }

    fclose(lexFil); //Close the lexer file stream

    lexFil = fopen(tmpLexPthStr, "r"); //Open a new lexer stream in "read mode"

    free(tmpStr);
    free(tmpLexPthStr);

    return lexFil;

}