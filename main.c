#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


char buf[2];
char* filePath;
char* paths[2];

char validateMode(char* input);
char* validatePath( char* input );

void initialize();

int main() {

    initialize();

    printf("Enter file mode [S/D]: ");
    validateMode( buf );

    int sentinel = 0;

    while( 2 > sentinel ){

        printf( "Enter file path %d to edit", sentinel+1 );
        if( NULL != validatePath( filePath )){

            paths[sentinel] = validatePath( filePath );
            sentinel++;
        }

    }

    return 0;
}

void initialize() {

    //make folders if not exist
    //make files if not exist
    //print out file/folder options
}

char validateMode( char* input ){

    if ( NULL == fgets( input, 2, stdin ) ){

        perror( "Error Invalid Input:  Please Enter a single character");
        exit( EXIT_FAILURE );
    }

    char mode = tolower(buf[1]);

    if( 0 != strcmp(&mode, "s") && 0 != strcmp(&mode, "d") ){

        perror( "Error Invalid Input:  Only enter 'D', 'S', 'd', or 's' (without the quotes)");
        exit( EXIT_FAILURE );
    }

    return mode;
}

char* validatePath( char* input ){

    if ( NULL == fgets( input, sizeof(input), stdin ) ){

        perror( "Error Invalid Input:  Please Enter a valid path");
        exit( EXIT_FAILURE );
    }

    char* path = input;
    // if exists file and sanitize, return path or null

    return path;
}