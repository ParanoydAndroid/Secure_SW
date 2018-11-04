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

    switch ((int) mode ){

        case (int) 'd':

            printf( "case d" );

            // I guess just read directory and then try to delete every file found, accounting for "."
            break;

        case (int) 's':

            printf( "case s" );

            // Pretty easy, see note in iniliazation about this.
            break;

        default:

            perror( "Unknown error" );
            break;

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

    snprintf( mode, sizeof( mode ), "%c", mode[0] );
    printf( "Mode successfully validated.  Mode is: %s\n", mode );
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
            perror( "Please enter a valid path (home/exampleDir" );
            continue;
        }

        if( 1 == validatePath( input, parsedPaths )){

            perror( "Please enter a valid path (home/exampleDir)" );
            continue;
        }

//        char str[MAX_PATH_LENGTH];
//        snprintf(str, sizeof(str), "%s/%s", parsedPaths[0], parsedPaths[1]);

        // Now that we have a valid, parsed path, we reconstitute it.
        snprintf( paths[sentinel], sizeof( paths[sentinel] ), "%s/%s", parsedPaths[0], parsedPaths[1] );

        //then we strip the trailing newline.
        paths[sentinel][strcspn( paths[sentinel], "\n" )] = 0;
        printf( "Paths[%i]: %s\n", sentinel, paths[sentinel] );
        sentinel++;
    }

    return 0;
}

void initialize(){

    char* directories[3] = {"dir1", "dir2", "dir3"};
    char* filePaths[12] = {"dir1/a.txt", "dir1/b.txt", "dir1/c.txt",
                           "dir2/c.txt", "dir2/d.txt", "dir2/e.txt",
                           "dir3/e.txt", "dir3/f.txt", "dir3/a.txt"};

    for( int i = 0; i < sizeof( directories ) / sizeof( directories[0] ); i++ ){

        // We can ignore errors here, since an error (probably) just tells the the dir already exists, which is fine.
        mkdir( directories[i], ACCESSPERMS);
    }

    // Also note this is basically the sync loop.  For NULL != readdir(paths[1]], if exists then continue, else fopen
    printf( "Current directories and contents that are available for modification:\n" );

    for( int i = 0; i < sizeof( filePaths ) / sizeof( filePaths[0] ); i++ ){

        printf( "home/%s\n", filePaths[i] );

        if( exists( filePaths[i] )){
            continue;
        }

        // We call fopen to create the file, then immediately close the handle because we don't actually want it open.
        FILE* file = fopen( filePaths[i], "w+" );
        fclose( file );
    }

}

int validateMode( char* input, char* output ){

    char mode = (char) tolower( input[0] );

    if( 0 != strcmp( &mode, "s" ) && 0 != strcmp( &mode, "d" )){

        return 1;
    }

    *output = mode;
    return 0;
}

int validatePath( char* input, char parsedPath[2][256] ){

    char* invalidSymbols = "\n\r\0";
    char* token = input;
    int sentinel = 0;

    while( token != NULL){

        // This splits strings based on the provided delimiter and returns each substring (iteratively) as a token.
        char* temptok = strsep( &token, "/" );

        // We explicitly only need to care about one step deeper into the file hierarchy
        // so I can use a static array instead of generecizing with a malloc()
        if( 2 > sentinel ){

            temptok[strcspn( temptok, "\n" )] = 0;
            strcpy( parsedPath[sentinel], temptok );
            sentinel++;

        } else{

            // ERROR
            // This means strsep is not yet null, yet we've found another token past the first
            // two,  this is invalid

            return 1;
        }
    }

    // Now that we've parsed the directory path, we do a multitude of checks on the second component
    // We mostly ignore the first because it's just a reference to /home

    // First, we check to ensure that we found at least one "/" (i.e. at least two tokens)
    if( sentinel < 2 ){

        return 1;
    }

    // We now check for invalid characters in the path provided
    for( int i = 0; i < sentinel; i++ ){

        // We can't strcmp with parsedPath[] directly, since we needa char* not a char*[], so we first store it
        // in a new char** then check it for invalid symbols
        char* parseCheck = malloc( sizeof( parsedPath[i] ));
        strcpy( parseCheck, parsedPath[i] );
        char* temp = strsep( &parseCheck, invalidSymbols );

        // TODOD: Don't forge tto remove this.
        printf( "%s\n", temp );
        if( 0 != strcmp( parsedPath[i], temp )){

            // Our string that parsed on invalid symbols found some, so it was an invalid path provided to us
            return 1;
        }
    }

    // Finally, we check to ensure the referenced directory actually exists.
    if( opendir( parsedPath[1] ) == NULL){

        return 1;
    }

    return 0;
}
