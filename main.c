#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


char* filePath;
char* paths[2];


int validateMode(char* input);
char* validatePath( char* input );

void initialize();

int getMode(char *string);

int main() {

    initialize();

    char mode;

    getmode( &mode );


    int sentinel = 0;
    while( 2 > sentinel ){

        printf( "Enter file path %d to edit", sentinel+1 );
        if( NULL != validatePath( filePath )){

            paths[sentinel] = validatePath( filePath );
            sentinel++;
        }

    }

    if ( 0 == strcmp(&mode, "s") ){

        //return sync action

    } else if( 0 == strcmp(&mode, "d") ){

        //return duplicate action

    } else {

        //error
    }


    //final output
    return 0;
}

int getMode( char* mode ){
    //On success, returns 0 and stores mode in char* mode
    //On fail returns 1 and the mode contents are not defined

    char input[2];

    if ( NULL == fgets( input, 2, stdin ) ){

        perror( "Error Invalid Input:  Please Enter a single character");
        return 0;
    }

    fgets( input, 2, stdin );

    if( 1 == validateMode( input, mode ){

        return 1;
    }

    validateMode( input, mode );
    return 0;
}

void initialize() {

    //make folders if not exist
    //make files if not exist
    //print out file/folder options
}

int validateMode( char* input, char* output ){


    char mode = (char)tolower( input[0] );

    if( 0 != strcmp(&mode, "s") && 0 != strcmp(&mode, "d") ){

        perror( "Error Invalid Input:  Only enter 'D', 'S', 'd', or 's' (without the quotes)");
        return 1;
    }

    *output = mode;
    return 0;
}

int validatePath( char* input, char* output ){

    if ( NULL == fgets( input, sizeof(input), stdin ) ){

        perror( "Error Invalid Input:  Please Enter a valid path");
        exit( EXIT_FAILURE );
    }

    char* path = input;
    // if exists file and sanitize, return path or null

    return path;
}