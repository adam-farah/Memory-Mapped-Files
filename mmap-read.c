/* Mapped-memory example - Reader program
 * Version: March 11, 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mmap.h"

int getIntegerLength(int);

int main (int argc, char* const argv[]) {
  int fd;
  char *p;
  int num;
  int prev;
  void* file_memory;
  int currIndex = 0;
  int n = strtol(argv[2], &p, 10);

  //check for argument
 	if(n <= 0){
    printf("failed to parse number of integers\n");
    return 1;
 	}

  /* open the file  */
  if( (fd = open (argv[1], O_RDWR, S_IRUSR | S_IWUSR)) == -1){
	  printf("open error: No such file or directory\n");
	  return 1;
  }

  /* create the memory-mapping */
  file_memory = mmap (NULL, FILESIZE, PROT_READ | PROT_WRITE,
		      MAP_SHARED, fd, 0);
  close (fd);


  /* read and print the integer */
  for(int i = 0; i < n; i++){
	  if(i == 0){
  		sscanf (file_memory, "%d", &num);
  		printf ("%d", num);
  		prev = num;
  		currIndex = getIntegerLength(prev);
	  }

	  else{
  		sscanf (file_memory+currIndex, "%d", &num);
  		printf ("%d", num);
  		prev = num;
  		currIndex = currIndex + getIntegerLength(prev);
	  }
  }
  printf("\n");
  /* release the memory */
  munmap (file_memory, FILESIZE);
  return 0;
}

int getIntegerLength(int num){
	int length=1;
	if(num == 0){
    return 1;
  }
	else if(num < 0){
		num = num * -1;
		length++;
	}
	while(num > 9){
		length++;
		num = num / 10;
	}

  //for when there's a null value in the string we add to the file
	return length+1;
}
