#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>


const int MAX_PATH_LENGTH = 256;

void dedup( char source[MAX_PATH_LENGTH], char target[MAX_PATH_LENGTH] );
int exists( char* file );
int getMode( char* mode );
int getPaths( char paths[2][MAX_PATH_LENGTH] );
void initialize( char* directories[], size_t arrLength );
void listDirs( char* directories[], size_t arrLength );
void syncDirs( char source[MAX_PATH_LENGTH], char target[MAX_PATH_LENGTH] );
int validateMode( char* input, char* output );
int validatePath( char* input, char parsedPath[2][MAX_PATH_LENGTH] );

int main(){

    char* directories[3] = {"dir1", "dir2", "dir3"};
    initialize( directories, sizeof( directories ) / sizeof( directories[0] ));

    char mode;
    int notDone;
    char paths[2][MAX_PATH_LENGTH];

    do{

        notDone = getMode( &mode );

    }while( notDone );

    getPaths( paths );

    switch ((int) mode ){

        case (int) 'd':

            dedup( paths[0], paths[1] );
            break;

        case (int) 's':

            //We call it twice to sync in both directions
            syncDirs( paths[0], paths[1] );
            syncDirs( paths[1], paths[0] );
            break;

        default:

            perror( "Unknown error" );
            exit( EXIT_FAILURE );
    }

    char* modeName = mode == 'd' ? "De-duplication" : "Synchronization";
    printf( "%s complete! \n", modeName );
    printf( "\nNew directory structure: \n" );
    listDirs( directories, sizeof( directories ) / sizeof( directories[0] ));
    return 0;
}

void dedup( char source[MAX_PATH_LENGTH], char target[MAX_PATH_LENGTH] ){

    printf( "De-duplicating home/%s and home/%s...\n", source, target );

    DIR* dir;
    struct dirent* dent;

    dir = opendir( source );

    // readdir returns the next entry struct in the dir stream until there are none remaining, then it returns NULL
    while(( dent = readdir( dir )) != NULL){

        char targetPath[MAX_PATH_LENGTH];
        char* name = dent->d_name;
        snprintf( targetPath, sizeof( targetPath ), "%s/%s", target, name );

        // We exclude files that already exist at the target location, and also don't duplicate the
        // '..' and '.' files that mark parents of the current directory.
        if( 0 == exists( targetPath ) || 0 == strcmp( "..", name ) || 0 == strcmp( ".", name )){
            continue;
        }

        if( 0 != remove( targetPath )){

            perror( "Unknown issue deleting file" );
        }
    }

    closedir( dir );
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

    printf( "Mode successfully validated.  Mode is: %c\n", mode[0] );
    return 0;
}

int getPaths( char paths[2][MAX_PATH_LENGTH] ){
    // On success, returns 0 and stores both file paths in *paths.
    // On fail returns 1 and the contents of **paths is not defined

    char input[MAX_PATH_LENGTH];
    int sentinel = 0;
    char parsedPaths[2][MAX_PATH_LENGTH];

    while( 2 > sentinel ){

        printf( "Enter File path %i (e.g. 'home/dir1'): ", sentinel + 1 );
        if( NULL == fgets( input, sizeof( input ), stdin )){

            printf( "Please enter a valid path (home/exampleDir\n" );
            continue;
        }

        if( 1 == validatePath( input, parsedPaths )){

            printf( "Please enter a valid path (home/exampleDir)\n" );
            continue;
        }

        // Now that we have a valid, parsed path, we discard home (since we don't need it) and keep the directory
        // home is initially processed for future expansion, if necessary.
        snprintf( paths[sentinel], sizeof( paths[sentinel] ), "%s", parsedPaths[1] );
        sentinel++;
    }

    return 0;
}

void initialize( char* directories[], size_t arrLength ){

    char* filePaths[9] = {"dir1/a.txt", "dir1/b.txt", "dir1/c.txt",
                          "dir2/c.txt", "dir2/d.txt", "dir2/e.txt",
                          "dir3/e.txt", "dir3/f.txt", "dir3/a.txt"};

    for( int i = 0; i < arrLength; i++ ){

        // We can ignore errors here, since an error (probably) just tells the the dir already exists, which is fine.
        mkdir( directories[i], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    for( int i = 0; i < sizeof( filePaths ) / sizeof( filePaths[0] ); i++ ){

        if( exists( filePaths[i] )){
            continue;
        }

        // We call fopen to create the file, then immediately close the handle because we don't actually want it open.
        FILE* file = fopen( filePaths[i], "w+" );
        fclose( file );
    }

    printf( "\nCurrent directories and contents that are available for modification:\n" );
    listDirs( directories, arrLength );
}

void listDirs( char* directories[], size_t arrLength ){

    DIR* dir;
    struct dirent* dent;

    for( int i = 0; i < arrLength; ++i ){

        dir = opendir( directories[i] );

        while(( dent = readdir( dir )) != NULL){

            // Ignore the root and parent directory listings.
            if( 0 == strcmp( "..", ( dent->d_name )) || 0 == strcmp( ".", dent->d_name )){
                continue;
            }

            char targetPath[MAX_PATH_LENGTH];
            snprintf( targetPath, sizeof( targetPath ), "home/%s/%s", directories[i], ( dent->d_name ));

            printf( "%s\n", targetPath );
        }

        printf( "\n" );
    }

    closedir(dir);
}

void syncDirs( char source[MAX_PATH_LENGTH], char target[MAX_PATH_LENGTH] ){

    printf( "Synchronizing home/%s and home/%s...\n", source, target );

    DIR* dir;
    struct dirent* dent;

    dir = opendir( source );

    // readdir returns the next entry struct in the dir stream until there are none remaining, then it returns NULL
    while(( dent = readdir( dir )) != NULL){

        char targetPath[MAX_PATH_LENGTH];
        char* name = dent->d_name;
        snprintf( targetPath, sizeof( targetPath ), "%s/%s", target, name );

        if( exists( targetPath ) || 0 == strcmp( "..", name ) || 0 == strcmp( ".", name )){
            continue;
        }

        // We call fopen to create the file, then immediately close the handle because we don't actually want it open.
        FILE* file = fopen( targetPath, "w+" );
        fclose( file );
    }

    closedir( dir );
}

int validateMode( char* input, char* output ){

    char modeStr[2];
    snprintf( modeStr, sizeof( modeStr ), "%c", tolower( input[0] ));

    // TODO: convert this into a straight character comparison
    if( 0 != strcmp( modeStr, "s" ) && 0 != strcmp( modeStr, "d" )){

        return 1;
    }

    *output = modeStr[0];
    return 0;
}

int validatePath( char* input, char parsedPath[2][MAX_PATH_LENGTH] ){

    char* invalidSymbols = "\n\r\0";
    int sentinel = 0;

    // OnlineGDB has a memory access problem if I try to use the original pointer directly, so we
    // duplicate it instead.
    char* token = strdup( input );

    while( token != NULL){

        // This splits strings based on the provided delimiter and returns each substring (iteratively) as a token.
        char* temptok = strsep( &token, "/" );

        // We explicitly only need to care about one step deeper into the file hierarchy
        // so I can use a static array instead of generecizing with a malloc()
        if( 2 > sentinel ){

            // Strips the trailing newline and changes it into a \0
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

        // We can't strcmp with parsedPath[] directly, since we need a char* not a char*[], so we first store it
        // in a new char* then check it for invalid symbols
        char* parseCheck = malloc( sizeof( parsedPath[i] ));
        strcpy( parseCheck, parsedPath[i] );
        char* temp = strsep( &parseCheck, invalidSymbols );

        if( 0 != strcmp( parsedPath[i], temp )){

            // Our string that parsed on invalid symbols found some, so it was an invalid path provided to us
            return 1;
        }
    }

    // Finally, we check to ensure the referenced directories actually exist.
    if( 0 != strcmp( parsedPath[0], "home" )){

        return 1;
    }

    if( opendir( parsedPath[1] ) == NULL){

        return 1;
    }

    closedir( parsedPath[1] );

    return 0;
}
