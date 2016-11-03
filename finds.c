    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <string.h>
    #include "Directories.h"
    

    int main(int argc, char * argv[])
    {
    int symLink;
    char * pathname;
    char * extension = malloc(1);
    char * searchString;
    char arg;
    extension[0] = NULL;



    while((arg = getopt(argc, argv, "p:f:ls:")) != -1)
    {
        switch(arg)
        {
            case 'p':
                if(optarg == NULL || optarg[0] == '-'){
                    fprintf(stderr,"No pathname specified\n");
                    exit(-1);
                }          
                pathname = optarg;
                break;
            case 'f':
                extension = optarg;
                if(extension == NULL){
                    fprintf(stderr," No file type specified for -f Flag\n");
                    exit(-1);
                }
                if(extension[0] != 'c' && extension[0] != 'h' && extension[0] != 'S')
                {
                    fprintf(stderr, "Invalid file modifier specified");
                    return -1;
                }
                break;
            case 'l':
                symLink = 1;
                break;
            case 's':
                searchString = optarg;
                break;
            case '?':
                if(optopt == 'p')
                {
                    fprintf(stderr, "No pathname was specified");
                }
                else if(optopt == 'f')
                {
                    fprintf(stderr, "No file type specified for -f");
                }
                else if(optopt == 's')
                {
                    fprintf(stderr, "No search string was specified");
                }
                
                return -1;
        }

    }
      find(pathname, searchString, symLink, extension);
    }



