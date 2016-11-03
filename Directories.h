#ifndef Directories

#define Directories

#endif


typedef struct 
{
  unsigned long *inodes;
  int availableSlots;
  int numSlots;
  int count;
} linkInfo;

extern void find(char * pathname, char *seachString, int symLink, char * fileType);
int findInFile(linkInfo * linkInfo, char *pathname, char *searchString, int symLink, int useStat, char * fileType);
extern void readFile(char* pathname, char * searchString, char * fileType);
extern void readFile(char *pathname, char *searchString, char *fileType);
