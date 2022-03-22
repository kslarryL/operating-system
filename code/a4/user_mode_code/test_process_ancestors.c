#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "process_ancestors.h"

#define _PROCESS_ANCESTORS_ 438

int main()
{
	struct process_info info_array[10];
	long size = 10;
	long num_filled;
	// int i;
	// normal test:
	printf("Test 1: ");
	printf("..Diving to kernel level\n");
	int result = syscall(_PROCESS_ANCESTORS_, info_array, size, &num_filled);
	int my_errno = errno;
	printf("..Rising to user level w/ result = %d\n", result);
	if(result < 0){
		printf("Failed\n");
		printf(", errno = %d", my_errno);
		return 0;
	}
	else{
		my_errno = 0;
		// printf("\n%ld\n",num_filled);
		// for(i = 0;i<num_filled;++i){
		// 	printf("%d....%ld\n", i, info_array[i].pid);
		// }
	}
	printf("Passed\n\n");
	

	// check info_array error:
	printf("Test 2: ");
	printf("..Diving to kernel level\n");
	result = syscall(_PROCESS_ANCESTORS_, NULL, size, &num_filled);
	my_errno = errno;
	printf("..Rising to user level w/ result = %d\n", result);
	if(result == 0){
		printf("Failed\n");
		return 0;
	}
	// printf(", errno = %d\n", my_errno);
	printf("Passed\n\n");
	
	
	// check size error:
	printf("Test 3: ");
	size = -3;
	printf("..Diving to kernel level\n");
	result = syscall(_PROCESS_ANCESTORS_, info_array, size, &num_filled);
	my_errno = errno;
	printf("..Rising to user level w/ result = %d\n", result);
	if(result == 0){
		printf("Failed\n");
		return 0;
	}
	// printf(", errno = %d\n", my_errno);
	printf("Passed\n\n");


	// check num_filled error:
	printf("Test 4: ");
	printf("..Diving to kernel level\n");
	result = syscall(_PROCESS_ANCESTORS_, info_array, size, NULL);
	my_errno = errno;
	printf("..Rising to user level w/ result = %d\n", result);
	if(result == 0){
		printf("Failed\n");
		return 0;
	}
	// printf(", errno = %d\n", my_errno);
	printf("Passed\n\n");
	
	return 0;	
}
