/* Mapped-memory example - Writer program
 * Version: March 11, 2018
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "mmap.h"

int getIntegerLength(int);
int checkIfInt(char*);

/* return a uniformly generated random number in the range [low,high]
 */
int random_range (unsigned const low, unsigned const high) {
  unsigned const range = high - low + 1;
  return low + (int) (((double) range) * rand () / (RAND_MAX + 1.0));
}

/* Create and write to a shared file for communication with another process.
 *
 * argv[1] = file name
 *
 * Note: Error processing is omitted
 */
int main (int argc, char* const argv[]) {
  int r;
  int fd;
  char *p;
  int prev;
  int currIndex = 0;
  void* file_memory;
  int n = strtol(argv[2], &p, 10);

  //check for argument
  if(checkIfInt(argv[2]) == 0){
	  printf("failed to parse number of integers\n");
	  return 1;
  }

  if(n <= 0){
  	  printf("invalid number of integers\n");
  	  return 1;
  }

  /* seed the random number generator */
  srand (time (NULL));

  /* open or create a file to hold an unsigned integer  */
  if((fd = open (argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
	 printf("open error: No such file or directory\n");
	 return 1;
  }

  /* write FILESIZE spaces */
  for(int i=0; i<FILESIZE; i++) write(fd," ", 1);
  write(fd," ", 1); /* write a NULL at EOF */

  /* create the memory-mapping
     1st param=start addr of mapping into memory, NULL means chosen by OS
     2nd param=length of map (bytes)
     3rd param=protection
     4th param=options, MAP_SHARED used for interprocess communications
     5th param=file descriptor of mapped file
     6th param=offset from start of file where mapping starts
   */
  if((file_memory = mmap (NULL, FILESIZE, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED ){
  	  printf("error: memory map operations have stoped\n");
  	  return 1;
  }
  close (fd);

  for(int i = 0; i < n; i++){
    r=random_range(-100, 100);
    //Put random integer to mmap
    if(i == 0){
      sprintf((char*) file_memory, "%d", r);
      prev = r;
      currIndex = getIntegerLength(prev);
    }
    else{
      sprintf((char*) file_memory+currIndex, "%d", r);
      prev = r;
      currIndex = currIndex + getIntegerLength(prev);
    }
  }

  /* release the memory  */
  munmap (file_memory, FILESIZE);
}

int getIntegerLength(int num){
	int len=1;

	if(num == 0){
		return 1;
	}
	else if(num < 0){
		num = num * -1;
		len++;
	}
	while(num > 9){
		len++;
		num = num / 10;
	}

  //for when there's a null value in the string we add to the file
	return len+1;
}

int checkIfInt(char *string){
	int integerTrue = 1;
	while(*string != '\0'){
		if(*string < '0' || *string > '9'){
			if(*string == '.' && integerTrue == 1){integerTrue = 0;}
			else{integerTrue = 0;}
		}
		string++;
	}
	return integerTrue;
}
