#include <math.h>
#include "messages.h"

FILE *rptFilPtr;

void opnRptFil(){

    char *tmpPth = malloc(sizeof(char)*(strlen(wrkstn.Path) + strlen(wrkstn.Name) + 6));

    sprintf(tmpPth, "%s%c%s.opf", wrkstn.Path, PATH_SLASH_TYP_CHAR, wrkstn.Name);

    rptFilPtr = fopen(tmpPth, "w"); //Create a new data file in "write mode"

    free(tmpPth);

}

void clsRptFil(){

    /*if(rptFilPtr != NULL){

        fclose(rptFilPtr);
        rptFilPtr = NULL;

    }*/

    fclose(rptFilPtr);

}

//char *crt;
#include "../libraries/types/number/Number.h"

void srtRptSrc(char **src, int lin, int col){

    int tmpIndx = getStrIndx(*src, "@"), siz = 1;
    char *tmp = malloc(sizeof(char)*siz);

    int nxtLin = 0, nxtCol = 0;

    tmp[0] = '\0';

    if(tmpIndx == -1){

        siz += strlen(*src) + 10 + numLen(lin) + numLen(col);
        tmp = realloc(tmp, sizeof(char)*siz);

        sprintf(tmp, "\n    in %s:%d;%d", *src, lin, col);

    }else{

        int isSrt = 1;

        do{

            tmpIndx = getStrIndx(*src, "@");

            char *tmpStr = getStrPrt(*src, 0, ((tmpIndx == -1) ? strlen(*src) : tmpIndx), 0);

            if(tmpStr[1] != 'm'){

                char *tmpHexIndx = getStrPrt(tmpStr, 3, 9, 0);

                int tmpId = hexToInt(tmpHexIndx);

                if(tmpStr[1] == DATA_PATH){

                    char *tmpDat = getDat(DATA_PATH, tmpId);

                    if(strcmp(tmpDat, "\0") != 0){

                        //printf("\n--%s--\n", tmpDat);

                        char *tmpSub1Str = getStrPrt(tmpDat, getStrIndx(tmpDat, "<Zx") + 3, getStrIndx(tmpDat, ">"), 0);
                        char *tmpSub2Str = getStrPrt(tmpDat, getLstStrIndx(tmpDat, "<Zx") + 3, getLstStrIndx(tmpDat, ">"), 0);

                        nxtLin = hexToInt(tmpSub1Str);
                        nxtCol = hexToInt(tmpSub2Str);

                        char *tmpPth = getStrPrt(tmpDat, 0, getStrIndx(tmpDat, "<Zx") - 1, 0);

                        free(tmpStr);
                        tmpStr = tmpPth;

                        free(tmpDat);

                    }

                }else if(tmpStr[1] == DATA_ZONE){

                    char *tmpDat = getDat(DATA_ZONE, tmpId);

                    if(strcmp(tmpDat, "\0") != 0){

                        //printf("\n--%s--\n", tmpDat);

                        char *tmpSub1Str = getStrPrt(tmpDat, getStrIndx(tmpDat, "<Zx") + 3, getStrIndx(tmpDat, ">"), 0);
                        char *tmpSub2Str = getStrPrt(tmpDat, getLstStrIndx(tmpDat, "<Zx") + 3, getLstStrIndx(tmpDat, ">"), 0);

                        nxtLin = hexToInt(tmpSub1Str);
                        nxtCol = hexToInt(tmpSub2Str);

                        free(tmpStr);

                        tmpStr = malloc(sizeof(char)*(5));
                        strcpy(tmpStr, "zone");

                        free(tmpDat);

                    }

                }

            }else{

                strcpy(tmpStr, "main");

            }

            siz += strlen(tmpStr) + 12 + numLen(lin) + numLen(col);
            tmp = realloc(tmp, sizeof(char)*siz);

            char *tmpSav = malloc(sizeof(char)*(strlen(tmp) + 1));

            strcpy(tmpSav, tmp);

            sprintf(tmp, "%s\n    %s <%s>:%d;%d", tmpSav, ((isSrt) ? "in" : "at"), tmpStr, lin, col);

            free(tmpSav);

            if(tmpIndx != -1){

                char *tmpPrt = getStrPrt(*src, tmpIndx + 1, strlen(*src), 0);

                strcpy(*src, tmpPrt);

                free(tmpPrt);

            }

            free(tmpStr);

            /*siz += 4;

            sprintf(tmp, "%s\n    %s", tmp, tmpStr);

            free(tmpStr);*/

            lin = nxtLin;
            col = nxtCol;
            nxtLin = 0;
            nxtCol = 0;

            if(isSrt)
                isSrt = 0;

        }while(tmpIndx != -1);

    }

    *src = tmp;

}

void rpt(const int typ, int sct, char *msg, char *src, int lin, int col){

    srtRptSrc(&src, lin, col);

    fprintf(rptFilPtr, "[%s] %s%s\n", (typ == REPORT_CODE_WARNING) ? "Warning" : ((typ == REPORT_CODE_MESSAGE) ?  "Message" : "Error")
                             , msg
                             , src);

    free(src);

    if(typ == REPORT_CODE_ERROR && DEBUG_ALLOW_TERMINATION){

        clsRptFil();

        writeLogLine("Compiler Manager", 0, "Terminating the process. (For more info, check the `.opf` file)", 0, 0, 0);

        exit(-sct*100);

    }
    
}

/*
    rpt(REPORT_CODE_ERROR);
    rpt(REPORT_CODE_WARNING);
    rpt(REPORT_CODE_MESSAGE);

#define REPORT_SECTION_GENERAL 1
#define REPORT_SECTION_PREPROCESSOR 2
#define REPORT_SECTION_LEXER 3
#define REPORT_SECTION_PARSER 4
#define REPORT_SECTION_OPTIMISER 5
#define REPORT_SECTION_EVALUATOR 6
#define REPORT_SECTION_BUNDLER 7
#define REPORT_SECTION_C 8
#define REPORT_SECTION_RUNTIME 9

*/