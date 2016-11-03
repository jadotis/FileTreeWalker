#include <stdio.h>
#include <dirent.h>
#include "Directories.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

void find(char *pathname, char *searchString, int symLink, char *fileType)
{
  linkInfo info;
  info.inodes = malloc(sizeof(long) * 100);
  info.availableSlots = 100;
  info.numSlots = 100;
  info.count = 0;

  findInFile(&info, pathname, searchString, symLink, 0, fileType);
}

int arrayContains(unsigned long *buffer, unsigned long val, int numSlots)
{

  for (int i = 0; i < numSlots; i++)
  {
    printf("val is: %lu   buffer[%i] is: %lu\n", val, i, buffer[i]);
    if (buffer[i] == val)
    {
      return 1;
    }
    continue;
  }
  return -1;
}

int findInFile(linkInfo *linkInfo, char *pathname, char *searchString, int symLink, int useStat, char *fileType)
{
  DIR *dptr;
  struct dirent *directory;
  struct stat statbuf;
  char *filePointer[4096] ;
  int statRet;

  //dptr = opendir(pathname);
  if ((dptr = opendir(pathname)) == NULL)
  {
    printf("%s", strerror(errno));
    closedir(dptr);
    return -1;
  }
  while ((directory = readdir(dptr)) != NULL) //start reading the directory
  {
    if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)
    {
      continue;
    }
    if(filePointer)
    {
      memset(filePointer, 0, 4096);
    }
    sprintf(filePointer, "%s%s%s", pathname, "/", directory->d_name);

    if (useStat == 1)
    {
      statRet = stat(filePointer, &statbuf);
    }
    else
    {
      statRet = lstat(filePointer, &statbuf);
    }
    //fprintf(stdout,"FilePointer: %s\n",filePointer);
    if (statRet < 0)
    {
      write(1, "error stating\n", 14);
    }
    if (S_ISREG(statbuf.st_mode))
    {
      //fprintf(stdout, "It is a file: %s \n", filePointer);
      readFile(filePointer, searchString, fileType);
    }
    if (S_ISDIR(statbuf.st_mode))
    {
      //printf("It is another directory! Lets read it\n");
      findInFile(linkInfo, filePointer, searchString, symLink, useStat, fileType);
    }
    if (S_ISLNK(statbuf.st_mode))
    {
      if (symLink == 0)
      {
        continue;
      }
      else if (arrayContains(linkInfo->inodes, statbuf.st_ino, linkInfo->numSlots) != 1)
      {
        linkInfo->inodes[linkInfo->numSlots - linkInfo->availableSlots] = statbuf.st_ino;
        linkInfo->availableSlots--;
        if (linkInfo->availableSlots == 0)
        {
          linkInfo->availableSlots = 100;
          linkInfo->numSlots += 100;
          realloc(linkInfo->inodes, linkInfo->numSlots);
        }
        findInFile(linkInfo, filePointer, searchString, symLink, 1, fileType);
      }
    }
    else
    {
      continue;
    }
  }
  closedir(dptr);
  return -1;
}

void SetToLower(char *line)
{
  for (int i = 0; line[i]; i++)
  {
    line[i] = tolower(line[i]);
  }
}

int hasWildCard(char *buffer, char *searchString)
{
  int wildCard = 0;
  for (int i = 0; i < strlen(searchString); i++)
  {
    if (searchString[i] == '*' || searchString[i] == '?' || searchString[i] == '.')
    {
      wildCard = 1;
      break;
    }
  }
  return wildCard;
}

int ScanFile(char *buffer, char *searchString)
{
  char *searchCopy = searchString;
  int wildCard = 0;
  SetToLower(buffer);
  SetToLower(searchString);
  wildCard = hasWildCard(buffer, searchString);
  if (wildCard == 0)
  {
    return strstr(buffer, searchString) != NULL;
  }
  else
  {
    int bool = 0;
    int bufferPos = 0;
    char prevChar = searchString[0];
    char regex;
    while (buffer[0] != 0 && searchString[0] != 0)
    {
      int isPeriod = searchString[0] == '.';

      if (isPeriod)
      {
        //printf("WE are in period\n");
        bool = 1;
        buffer = &buffer[1];
        searchString = &searchString[1];
        regex = NULL;
      }
      else if (buffer[0] == searchString[0])
      {
        //printf("WE are in regular char = %c\n", searchString[0]);
        if (searchString[1] != NULL && searchString[1] == '*')
        {
          while (buffer[0] != '\0' && buffer[0] == searchString[0])
          {
            buffer = &buffer[1];
          }
          searchString = &searchString[2];
          bool = 1;
        }
        else if (searchString[1] != NULL && searchString[1] == '?')
        {
          int atLeastOnce = 0;
          while (buffer[0] != '\0' && buffer[0] == searchString[0])
          {
            atLeastOnce = 1;
            buffer = &buffer[1];
          }
          if (atLeastOnce == 1)
          {
            searchString = &searchString[2];
            bool = 1;
          }
          else
          {
            bool = 0;
            searchString = searchCopy;
          }
        }
        else
        {
          bool = 1;
          buffer = &buffer[1];
          searchString = &searchString[1];
          regex = NULL;
        }
      }
      else
      {
        buffer = &buffer[1];
        bool = 0;
        searchString = searchCopy;
      }
    }
    return bool && searchString[0] == NULL;
  }
  return 0;
}

void readFile(char *pathname, char *searchString, char *fileType)
{
  int length = strlen(pathname);
  if (fileType[0] != '\0' && (pathname[length - 2] != '.' || pathname[length - 1] != fileType[0]))
  {
     return;
    
  }
  char buffer[10000];
  char original[10000];
  int lineCounter = 1;
  FILE *fptr;
  if ((fptr = fopen(pathname, "r")) <= 0)
  {
    fprintf(stderr, "The File could not be read\n");
    fclose(fptr);
    return;
  }
  while ((fgets(buffer, 10000, fptr)) != NULL )
  {
    memcpy(original, buffer, strlen(buffer));
    int ret = ScanFile(buffer, searchString);

    if (ret == 1)
    {
      printf("Found a match in: %s\n", pathname);
      printf("line: %i   %s", lineCounter, buffer);
    }
    lineCounter++;
    memset(buffer, 0, strlen(buffer));
    memset(original, 0, strlen(original));
  }
  fclose(fptr);  
}
