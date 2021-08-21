/**
 *  return_statement
 *      : return <SYMBOL>
 *      ;
**/

int isRtnStt(int typ, char *val){

    return (typ == LEXER_SYMBOL &&
        strcmp(val, "return") == 0
    );

}

void crtRtnStt(int linId){ //Create a return_statement

    isrtPrsTrm(PARSER_STATEMENTS_RETURN, "", linId);

}

/**
 *  ref_statement
 *      : return <SYMBOL>
 *      ;
**/

int isRefStt(int typ, char *val){

    return (typ == LEXER_SYMBOL &&
        strcmp(val, "ref") == 0
    );

}

void crtRefStt(int linId){ //Create a ref_statement

    isrtPrsTrm(PARSER_STATEMENTS_REF, "", linId);

}

/**
 *  delete_statement
 *      : delete <SYMBOL>
 *      ;
**/

int isDelStt(int typ, char *val){

    return (typ == LEXER_SYMBOL &&
        strcmp(val, "delete") == 0
    );

}

void crtDelStt(int linId){ //Create a delete_statement

    isrtPrsTrm(PARSER_STATEMENTS_DELETE, "", linId);

}