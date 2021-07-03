typedef struct {
    char *type; //symbol, string, character, number & operator!
    char *value;
    int srcColumn;
    int srcLine;
    char *srcFile;
} M_Token;

FILE* lexProc(TmpFileStruc cFileObj){

    FILE *lexFil = fopen(apdStr(cFileObj.pth, ".lxic"), "w"); //Create a new lexer file in "write mode"

    char *tmpStr = malloc(sizeof(char)*MAX_LINE_LENGTH), *curLin = malloc(sizeof(char)*MAX_LINE_LENGTH);

    fgets(tmpStr, MAX_LINE_LENGTH, cFileObj.ptr); //Get the first line
    if(tmpStr[strlen(tmpStr) - 1] == '\n')
        tmpStr[strlen(tmpStr) - 1] = '\0'; //Remove the new line character (\n), and replace it with a line end character (\0)!

    while(getStrIndx(tmpStr, "[FileEnd]") != 0){

        curLin = getStrPrt(tmpStr, getStrIndx(tmpStr, "]->") + 3, strlen(tmpStr), 0); //Get the current line content

        char *curFil;
        int col, lin;

        fprintf(lexFil, "%s\n", curLin); //Debug

        for(int i = 0; i < strlen(curLin); i++){ //Use this loop to scan every character one by one!

            printf("%c, ", curLin[i]);//Debug

        }

        //Get the next line!
        fgets(tmpStr, MAX_LINE_LENGTH, cFileObj.ptr);
        if(tmpStr[strlen(tmpStr) - 1] == '\n')
            tmpStr[strlen(tmpStr) - 1] = '\0'; //Remove the new line character (\n), and replace it with a line end character (\0)!

    }

    fclose(lexFil); //Close the lexer file stream
    lexFil = fopen(apdStr(cFileObj.pth, ".lxic"), "r"); //Open a new lexer stream in "read mode"

    return lexFil;

}


/*

```
using system.io,
      a;
import "file.mur",
        "k.mur";
setsize int 10,
        double 100;

group Name {
    class Name {
        function::type name(){
            //
        }
    }
}
```
=>
```
(symbol)("using") (symbol)("system")("operator")(".")(symbol)("io")("operator")(",")
      (symbol)("a")("operator")(";")
(symbol)("import") (string)("file.mur"),
[...]

(symbol)("group") (symbol)("Name") (operator)("{")
    [...]
(operator)("}")
```
*/