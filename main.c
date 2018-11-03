#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

const int MAX_PATH_LENGTH = 256;

int exists( char* file );
int getMode( char* mode );
int getPaths( char paths[2][MAX_PATH_LENGTH] );
void initialize();
int validateMode( char* input, char* output );
int validatePath( char* input, char parsedPath[2][MAX_PATH_LENGTH] );

int main(){

    initialize();

    char mode;
    int notDone;
    char paths[2][256];

    do{

        notDone = getMode( &mode );

    } while( notDone );

    getPaths( paths );

    if( 0 == strcmp( &mode, "s" )){

        //return sync action

    } else if( 0 == strcmp( &mode, "d" )){

        //return duplicate action

    } else{

        //error
    }

    //final output
    return 0;
}

int exists( char* file ){

    if( 0 == access( file, F_OK )){

        return 1;
    }

    return 0;
}

int getMode( char* mode ){
    //On success, returns 0 and stores mode in char* mode
    //On fail returns 1 and the contents of mode are not defined

    char input[3];

    printf( "\nPlease enter file mode [D/S]: " );
    if( NULL == fgets( input, sizeof( input ), stdin )){

        perror( "Please Enter a single character" );
        return 1;
    }

    if( 1 == validateMode( input, mode )){

        perror( "Error Invalid Input" );
        return 1;
    }

    printf( "Mode successfully validated.  Mode is: %s\n", mode );
    return 0;
}

void initialize(){

    char* directories[2] = {"dir1", "dir2"};
    char* filePaths[4] = {"dir1/a.txt", "dir1/b.txt", "dir2/c.txt", "dir2/d.txt"};

    for( int i = 0; i < sizeof( directories ) / sizeof( directories[ 0 ] ); i++ ){

        // We can ignore errors here, since an error (probably) just tells the the dir already exists, which is fine.
        mkdir( directories[ i ], ACCESSPERMS);
    }

    printf( "Current directories and contents that are available for modification:\n" );

    for( int i = 0; i < sizeof( filePaths ) / sizeof( filePaths[ 0 ] ); i++ ){

        printf( "%s\n", filePaths[ i ] );

        if( exists( filePaths[ i ] )){
            continue;
        }

        // We call fopen to create the file, then immediately close the handle because we don't actually want it open.
        FILE* file = fopen( filePaths[ i ], "w+" );
        fclose( file );
    }

}

int validateMode( char* input, char* output ){

    char mode = (char) tolower( input[ 0 ] );

    if( 0 != strcmp( &mode, "s" ) && 0 != strcmp( &mode, "d" )){

        return 1;
    }

    *output = mode;
    return 0;
}


int validatePath( char* input, char parsedPath[2][256] ){

    char invalidSymbols[1] = {'\0'};
    // if exists file and sanitize, return path or null

    char* token = input;
    int sentinel = 0;

    while( token != NULL){

        char* temptok = strsep( &token, "/" );

        // We explicitly only need to care about one step deeper into the file hierarchy
        // so I don't have to malloc this.
        if( 2 > sentinel ){

            strcpy(parsedPath[ sentinel ], temptok);
            sentinel++;

        } else {

            // This means strsep is not yet null, yet we've found another token past the first
            // two,  this is invalid

            return 1;
        }
    }

    // Now we check to ensure that we found at least one "/" (i.e. at least two tokens)
    if( sentinel < 2 ){

        return 1;
    }

    return 0;
}

int getPaths( char paths[2][MAX_PATH_LENGTH] ){
    // On success, returns 0 and stores both file paths in *paths.
    // On fail returns 1 and the contents of **paths is not defined

    char input[MAX_PATH_LENGTH];
    int sentinel = 0;
    char parsedPaths[2][MAX_PATH_LENGTH];

    while( 2 > sentinel ){

        printf( "Enter File path %i: ", sentinel + 1 );
        if( NULL == fgets( input, sizeof( input ), stdin )){
            perror( "Please enter a valid path (dir1/someFile.ext)" );
            continue;
        }

        if( 1 == validatePath( input, parsedPaths)){

            perror( "Please enter a valid path (dir1/someFile.ext)" );
            continue;
        }

//        char str[MAX_PATH_LENGTH];
//        snprintf(str, sizeof(str), "%s/%s", parsedPaths[0], parsedPaths[1]);

        // Now that we have a valid, parsed path, we reconstitute it.
        snprintf(paths[sentinel], sizeof(paths[sentinel]), "%s/%s", parsedPaths[0], parsedPaths[1]);

        //then we strip the trailing newline.
        paths[sentinel][strcspn(paths[sentinel], "\n")] = 0;
        printf("Paths[%i]: %s", sentinel, paths[sentinel]);
        sentinel++;
    }

    return 0;
}