#include <stdio.h>
#include "myalloc.h"

int main(int argc, char* argv[]) {
    initialize_allocator(100, FIRST_FIT);
    // initialize_allocator(100, BEST_FIT);
    // initialize_allocator(100, WORST_FIT);
    printf("Using first fit algorithm on memory size 100\n");

    int* p[50] = {NULL};
    for(int i=0; i<10; ++i) {
        p[i] = allocate(sizeof(int));
        if(p[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    }

    print_statistics();

    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0)
            continue;

        printf("Freeing p[%d]\n", i);
        deallocate(p[i]);
        p[i] = NULL;
    }

    print_statistics();

    printf("available_memory %d\n", available_memory());

    void* before[100] = {NULL};
    void* after[100] = {NULL};
    int count = compact_allocation(before, after);
    
    for (int i=0; i<count; ++i)
    {
        printf("Before: %p , After: %p\n", before[i], after[i]);
    }

    print_statistics();

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    destroy_allocator();

    return 0;
}

// int main(int argc, char* argv[]){
//     initialize_allocator(100, WORST_FIT);
//     printf("Using worst fit algorithm on memory size 100\n");

//     int* p[50] = {NULL};
//     for(int i=0; i<10; ++i) {
//         p[i] = allocate(sizeof(int));
//         if(p[i] == NULL) {
//             printf("Allocation failed\n");
//             continue;
//         }
//         //printf("here\n");
//         *(p[i]) = i;
//         printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
//     }

//     print_statistics();

//     printf("Freeing p[%d]\n", 0);
//     deallocate(p[0]);
//     p[0] = NULL;
//     printf("Freeing p[%d]\n", 2);
//     deallocate(p[2]);
//     p[2] = NULL;
//     printf("Freeing p[%d]\n", 3);
//     deallocate(p[3]);
//     p[3] = NULL;

//     print_statistics();

//     printf("available_memory %d\n", available_memory());

//     p[0] = allocate(sizeof(int));
//     if(p[0] == NULL) {
//         printf("Allocation failed\n");
//     }
//     *(p[0]) = 0;
//     printf("p[%d] = %p ; *p[%d] = %d\n", 0, p[0], 0, *(p[0]));

//     print_statistics();

//     // You can assume that the destroy_allocator will always be the 
//     // last funciton call of main function to avoid memory leak 
//     // before exit

//     destroy_allocator();

//     return 0;
// }

// int main(int argc, char* argv[]){
//     initialize_allocator(100, BEST_FIT);
//     printf("Using best fit algorithm on memory size 100\n");

//     int* p[50] = {NULL};
//     for(int i=0; i<10; ++i) {
//         p[i] = allocate(sizeof(int));
//         if(p[i] == NULL) {
//             printf("Allocation failed\n");
//             continue;
//         }
//         //printf("here\n");
//         *(p[i]) = i;
//         printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
//     }

//     print_statistics();

//     printf("Freeing p[%d]\n", 0);
//     deallocate(p[0]);
//     p[0] = NULL;
//     printf("Freeing p[%d]\n", 1);
//     deallocate(p[1]);
//     p[1] = NULL;
//     printf("Freeing p[%d]\n", 3);
//     deallocate(p[3]);
//     p[3] = NULL;
//     printf("Freeing p[%d]\n", 5);
//     deallocate(p[5]);
//     p[5] = NULL;
//     printf("Freeing p[%d]\n", 6);
//     deallocate(p[6]);
//     p[6] = NULL;

//     print_statistics();

//     printf("available_memory %d\n", available_memory());

//     p[0] = allocate(sizeof(int));
//     if(p[0] == NULL) {
//         printf("Allocation failed\n");
//     }
//     *(p[0]) = 0;
//     printf("p[%d] = %p ; *p[%d] = %d\n", 0, p[0], 0, *(p[0]));

//     print_statistics();

//     // You can assume that the destroy_allocator will always be the 
//     // last funciton call of main function to avoid memory leak 
//     // before exit

//     destroy_allocator();

//     return 0;
// }