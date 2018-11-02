#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


char* filePath;
char* paths[2];


int getMode(char *string);
int validateMode(char* input, char* output);
int getPaths( char** path );
int validatePath( char* input );

void initialize();



int main() {

    initialize();

    char mode;
    getMode( &mode );

    char* paths[2];
    getPaths( paths );

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

    if( 1 == validateMode( input, mode )){

        printf("mode succesfully validated.  Mode is: %s", mode);
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


int validatePath( char* input ){

    char* path = input;
    // if exists file and sanitize, return path or null

    return 0;
}

int getPaths( char** paths ){

    char input[1024];
    int sentinel = 0;

    while( 2 > sentinel ){

        if( NULL == fgets( input, sizeof( input ), stdin) ){
            perror( "Error Invalid Input:  Please enter a valid path");
            continue;
        }

        fgets( input, sizeof( input ), stdin);

        if( 1 == validatePath( input )){

            perror( "Error Invalid Input:  Please enter a valid path");
            continue;
        }

        paths[sentinel] = input;
        sentinel++;
    }

}