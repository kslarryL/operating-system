#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "myalloc.h"
#include "list.h"

bool available;

void acquire() {
    while (!available); //busy wait
    available = false;
}

void release() {
    available = true;
}

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
    struct Node* used_blocks_list;
    struct Node* empty_blocks_list;
};

struct Myalloc myalloc;

size_t get_total_Empty_Size(){
    size_t total_empty_block_size = 0;
    struct Node* ptr_to_EmptyList = myalloc.empty_blocks_list;
    while(ptr_to_EmptyList != NULL){
        total_empty_block_size += ((size_t*)ptr_to_EmptyList->address)[0];
        ptr_to_EmptyList = ptr_to_EmptyList->next;
    }
    total_empty_block_size += 8 * List_countNodes(myalloc.empty_blocks_list);
    return total_empty_block_size;
}

size_t get_total_Used_Size(){
    size_t total_used_block_size = 0;
    struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
    while(ptr_to_UsedList != NULL){
        total_used_block_size += ((size_t*)ptr_to_UsedList->address)[0];
        //printf("%ld\n", ((size_t*)ptr_to_UsedList->address)[0]);////////////////////////////////////////
        ptr_to_UsedList = ptr_to_UsedList->next;
    }
    total_used_block_size += 8 * List_countNodes(myalloc.used_blocks_list);
    return total_used_block_size;
}

size_t get_smallest_Empty_Block_size(){
    if(myalloc.empty_blocks_list == NULL){
        return 0;
    }
    size_t smallest_size = ((size_t*)myalloc.empty_blocks_list->address)[0];
    struct Node* ptr_to_EmptyList = myalloc.empty_blocks_list;
    while(ptr_to_EmptyList != NULL){
        if(((size_t*)ptr_to_EmptyList->address)[0] < smallest_size){
            smallest_size = ((size_t*)ptr_to_EmptyList->address)[0];
        }
        ptr_to_EmptyList = ptr_to_EmptyList->next;
    }
    smallest_size += 8;
    return smallest_size;
}

size_t get_largest_Empty_Block_size(){
    if(myalloc.empty_blocks_list == NULL){
        return 0;
    }
    size_t largest_size = ((size_t*)myalloc.empty_blocks_list->address)[0];
    struct Node* ptr_to_EmptyList = myalloc.empty_blocks_list;
    while(ptr_to_EmptyList != NULL){
        if(((size_t*)ptr_to_EmptyList->address)[0] > largest_size){
            largest_size = ((size_t*)ptr_to_EmptyList->address)[0];
        }
        ptr_to_EmptyList = ptr_to_EmptyList->next;
    }
    largest_size += 8;
    return largest_size;
}

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.memory = malloc((size_t)myalloc.size);
    myalloc.used_blocks_list = NULL;
    
    myalloc.empty_blocks_list = (struct Node*)malloc(sizeof(struct Node));
    myalloc.empty_blocks_list->address = myalloc.memory;
    myalloc.empty_blocks_list->next = NULL;
    
    memset(myalloc.memory,0,myalloc.size);
    size_t size_in_block = myalloc.size - 8;//header
    ((size_t*)myalloc.memory)[0] = size_in_block;
    
    available = true;
    // Add some other initialization 
}

void destroy_allocator() {
    free(myalloc.memory);
    while(myalloc.used_blocks_list != NULL){
        struct Node* temp = myalloc.used_blocks_list;
        myalloc.used_blocks_list = myalloc.used_blocks_list->next;
        free(temp);
    }
    while(myalloc.empty_blocks_list != NULL){
        struct Node* temp = myalloc.empty_blocks_list;
        myalloc.empty_blocks_list = myalloc.empty_blocks_list->next;
        free(temp);
    }
    // free other dynamic allocated memory to avoid memory leak
}

void* allocate(int _size) {
    //...
    acquire();
    //...
    void* ptr = NULL;

    if(myalloc.empty_blocks_list == NULL){
        printf("ERROR: NO ENOUGH SPACE\n");
        //...
        release();
        //...
        return ptr;
    }

    struct Node* temp = myalloc.empty_blocks_list;
    struct Node* lastNode = temp;
    if(myalloc.aalgorithm == FIRST_FIT){//first fit
        bool bestfit = false;
        while(((size_t*)temp->address)[0] < _size){
            if(((size_t*)temp->address)[0] == _size){
                bestfit = true;
                break;
            }
            lastNode = temp;
            temp = temp->next;
            if(temp == NULL){
                printf("ERROR: NO ENOUGH SPACE\n");
                //...
                release();
                //...
                return NULL;
            }
        }


        size_t original_size = ((size_t*)temp->address)[0];//record original temp's size
        if(original_size - _size < 8){
            _size = original_size;
            bestfit = true;
        }


        ((size_t*)temp->address)[0] = _size;

        //printf("allocated %ld byte\n", ((size_t*)temp->address)[0]);

        if(myalloc.used_blocks_list == NULL){
            myalloc.used_blocks_list = temp;
        }
        else{
            struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
            while(ptr_to_UsedList->next != NULL){
                ptr_to_UsedList = ptr_to_UsedList->next;
            }
            ptr_to_UsedList->next = temp;
        }

        if(lastNode == temp){
            if(get_total_Used_Size() == myalloc.size){
                myalloc.empty_blocks_list = NULL;
            }
            else{
                myalloc.empty_blocks_list = (struct Node*)malloc(sizeof(struct Node));
                myalloc.empty_blocks_list->address = temp->address + _size + 8;
                myalloc.empty_blocks_list->next = NULL; 
                ((size_t*)myalloc.empty_blocks_list->address)[0] = original_size - _size - 8;//set header
            }
        }
        else{
            if(bestfit){
                lastNode->next = temp->next;
            }
            else{
                struct Node* remain_block = (struct Node*)malloc(sizeof(struct Node));
                remain_block->address = temp->address + _size;
                lastNode->next = remain_block;
                remain_block->next = temp->next;
                ((size_t*)remain_block->address)[0] = original_size - _size - 8;
            }
        }

        temp->next = NULL;
        ptr = temp->address + 8;
    }
    else if(myalloc.aalgorithm == BEST_FIT){
        bool bestbestfit = false;
        struct Node* available_empty_block_arr[List_countNodes(myalloc.empty_blocks_list)];
        int size_of_arr = 0;
        while(temp != NULL){
            if(((size_t*)temp->address)[0] > _size){
                available_empty_block_arr[size_of_arr] = temp;
                size_of_arr++;
            }
            else if(((size_t*)temp->address)[0] == _size){
                bestbestfit = true;
                break;
            }
            lastNode = temp;
            temp = temp->next;
        }
        if(!bestbestfit && (size_of_arr > 0)){
            //find smallest one
            size_t smallest_size = ((size_t*)available_empty_block_arr[0]->address)[0];
            int index_of_smallest_size = 0;
            for(int i=0;i<size_of_arr;i++){
                if(((size_t*)available_empty_block_arr[i]->address)[0] < smallest_size){
                    index_of_smallest_size = i;
                }
            }
            temp = available_empty_block_arr[index_of_smallest_size];
            if(smallest_size - _size < 8){
                _size = smallest_size;
                bestbestfit = true;
            }
        }
        if(temp == NULL){
            printf("ERROR: NO ENOUGH SPACE\n");
            //...
            release();
            //...
            return NULL;
        }


        size_t original_size = ((size_t*)temp->address)[0];//record original temp's size
        ((size_t*)temp->address)[0] = _size;

        //printf("allocated %ld byte\n", ((size_t*)temp->address)[0]);

        if(myalloc.used_blocks_list == NULL){
            myalloc.used_blocks_list = temp;
        }
        else{
            struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
            while(ptr_to_UsedList->next != NULL){
                ptr_to_UsedList = ptr_to_UsedList->next;
            }
            ptr_to_UsedList->next = temp;
        }

        if(lastNode == temp){
            if(get_total_Used_Size() == myalloc.size){
                myalloc.empty_blocks_list = NULL;
            }
            else{
                myalloc.empty_blocks_list = (struct Node*)malloc(sizeof(struct Node));
                myalloc.empty_blocks_list->address = temp->address + _size + 8;
                myalloc.empty_blocks_list->next = NULL; 
                ((size_t*)myalloc.empty_blocks_list->address)[0] = original_size - _size - 8;//set header
            }
        }
        else{
            if(bestbestfit){
                lastNode->next = temp->next;
            }
            else{
                struct Node* remain_block = (struct Node*)malloc(sizeof(struct Node));
                remain_block->address = temp->address + _size;
                lastNode->next = remain_block;
                remain_block->next = temp->next;
                ((size_t*)remain_block->address)[0] = original_size - _size - 8;
            }
        }

        temp->next = NULL;
        ptr = temp->address + 8;
    }
    else if(myalloc.aalgorithm == WORST_FIT){
        struct Node* available_empty_block_arr[List_countNodes(myalloc.empty_blocks_list)];
        int size_of_arr = 0;
        while(temp != NULL){
            if((((size_t*)temp->address)[0] >= _size)){
                available_empty_block_arr[size_of_arr] = temp;
                size_of_arr++;
            }
            temp = temp->next;
        }
        if(size_of_arr > 0){
            //find biggest one
            size_t biggest_size = ((size_t*)available_empty_block_arr[0]->address)[0];
            int index_of_biggest_size = 0;
            for(int i=0;i<size_of_arr;i++){
                if(((size_t*)available_empty_block_arr[i]->address)[0] > biggest_size){
                    index_of_biggest_size = i;
                }
            }
            temp = available_empty_block_arr[index_of_biggest_size];
            if(biggest_size - _size < 8){
                _size = biggest_size;
            }
        }
        if(temp == NULL){
            printf("ERROR: NO ENOUGH SPACE\n");
            //...
            release();
            //...
            return NULL;
        }

        //find last node
        struct Node* ptr_to_EmptyList = myalloc.empty_blocks_list;
        if(ptr_to_EmptyList != temp){
            while(ptr_to_EmptyList != NULL){
                if(ptr_to_EmptyList->next == temp){
                    lastNode = ptr_to_EmptyList;
                    break;
                }
                ptr_to_EmptyList = ptr_to_EmptyList->next;
            }
        }
        

        bool bestfit = false;
        if(((size_t*)temp->address)[0] == _size){
            bestfit = true;
        }

        size_t original_size = ((size_t*)temp->address)[0];//record original temp's size
        ((size_t*)temp->address)[0] = _size;

        //printf("allocated %ld byte\n", ((size_t*)temp->address)[0]);

        if(myalloc.used_blocks_list == NULL){
            myalloc.used_blocks_list = temp;
        }
        else{
            struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
            while(ptr_to_UsedList->next != NULL){
                ptr_to_UsedList = ptr_to_UsedList->next;
            }
            ptr_to_UsedList->next = temp;
        }

        if(lastNode == temp){
            if(get_total_Used_Size() == myalloc.size){
                myalloc.empty_blocks_list = NULL;
            }
            else{
                myalloc.empty_blocks_list = (struct Node*)malloc(sizeof(struct Node));
                myalloc.empty_blocks_list->address = temp->address + _size + 8;
                myalloc.empty_blocks_list->next = NULL; 
                ((size_t*)myalloc.empty_blocks_list->address)[0] = original_size - _size - 8;//set header
            }
        }
        else{
            if(bestfit){
                lastNode->next = temp->next;
            }
            else{
                struct Node* remain_block = (struct Node*)malloc(sizeof(struct Node));
                remain_block->address = temp->address + _size;
                lastNode->next = remain_block;
                remain_block->next = temp->next;
                ((size_t*)remain_block->address)[0] = original_size - _size - 8;
            }
        }

        temp->next = NULL;
        ptr = temp->address + 8;
    }

    // }
    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    //...
    release();
    //...

    return ptr;
}

void deallocate(void* _ptr) {
    //...
    acquire();
    //...
    //assert(_ptr != NULL);

    if(_ptr == NULL){
        printf("ERROR: Failed to deallocate\n");
        //...
        release();
        //...
        return;
    }
    struct Node* newEmptyBlock = List_createNode(_ptr - 8);
    List_insertTail(&(myalloc.empty_blocks_list),newEmptyBlock);
    struct Node* search_result = List_findNode(myalloc.used_blocks_list,_ptr - 8);
    List_deleteNode(&(myalloc.used_blocks_list),search_result);
    
    size_t limit = ((size_t*)newEmptyBlock->address)[0];
    void* next_block_header_address = newEmptyBlock->address + 8 + limit;
    search_result = List_findNode(myalloc.empty_blocks_list,next_block_header_address);
    if(search_result != NULL){
        ((size_t*)newEmptyBlock->address)[0] = limit + 8 + ((size_t*)search_result->address)[0];//set new size
        List_deleteNode(&(myalloc.empty_blocks_list),search_result);//delete the next empty block
    }

    //compact this block and last empty block
    struct Node* temp = myalloc.empty_blocks_list;
    while(temp != NULL){
        if(temp->address + 8 + ((size_t*)temp->address)[0] == newEmptyBlock->address){
            ((size_t*)temp->address)[0] += (8 + ((size_t*)newEmptyBlock->address)[0]);
            List_deleteNode(&(myalloc.empty_blocks_list),newEmptyBlock);
            break;
        }
        temp = temp->next;
    }
    
    //...
    release();
    //...

    // Free allocated memory
    // Note: _ptr points to the user-visible memory. The size information is
    // stored at (char*)_ptr - 8.
}

int compact_allocation(void** _before, void** _after) {

    //...
    acquire();
    //...

    int compacted_size = List_countNodes(myalloc.used_blocks_list);

    // size_t total_empty_block_size = 0;
    // struct Node* ptr_to_EmptyList = myalloc.empty_blocks_list;
    // while(ptr_to_EmptyList != NULL){
    //     total_empty_block_size += ((size_t*)ptr_to_EmptyList->address)[0];
    // }
    // int num_of_EmptyBlock = List_countNodes(myalloc.empty_blocks_list);
    // total_empty_block_size += ((num_of_EmptyBlock-1)*8);

    // compact allocated memory
    // update _before, _after and compacted_size
    //void* _before[0] = myalloc.memory + 8;
    
    for(int i=0;i<compacted_size;i++){
        void* smallest_address = myalloc.used_blocks_list->address;
        struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
        while(ptr_to_UsedList != NULL){
            if(ptr_to_UsedList->address < smallest_address){
                smallest_address = ptr_to_UsedList->address;
            }
            ptr_to_UsedList = ptr_to_UsedList->next;
        }
        _before[i] = smallest_address;
        ptr_to_UsedList = List_findNode(myalloc.used_blocks_list,smallest_address);
        List_deleteNode (&myalloc.used_blocks_list,ptr_to_UsedList);
    }

    size_t sizeArr_of_usedBlocks[compacted_size];
    for(int i=0;i<compacted_size;i++){
        sizeArr_of_usedBlocks[i] = ((size_t*)_before[i])[0];
    }

    void* ptr_to_memory = myalloc.memory;
    for(int i=0;i<compacted_size;i++){
        memcpy(ptr_to_memory,_before[i],sizeArr_of_usedBlocks[i] + 8);
        _after[i] = ptr_to_memory;
        struct Node* newNode = List_createNode(ptr_to_memory);
        List_insertTail(&myalloc.used_blocks_list,newNode);
        ptr_to_memory += (((size_t*)ptr_to_memory)[0] + 8);
    }

    struct Node* ptr_to_UsedList = myalloc.used_blocks_list;
    size_t total_used_block_size = 0;
    while(ptr_to_UsedList != NULL){
        total_used_block_size += ((size_t*)ptr_to_UsedList->address)[0];
        ptr_to_UsedList = ptr_to_UsedList->next;
    }
    void* empty_Block_address = myalloc.memory + total_used_block_size + (compacted_size * 8);
    ((size_t*)empty_Block_address)[0] = myalloc.size - (total_used_block_size + (compacted_size * 8)) - 8;//set empty block's header
    int count = List_countNodes(myalloc.empty_blocks_list);
    for(int i=0;i<count;i++){//delete all old empty block nodes
        List_deleteNode (&myalloc.empty_blocks_list,myalloc.empty_blocks_list);
    }
    struct Node* newNode = List_createNode(empty_Block_address);
    List_insertTail(&myalloc.empty_blocks_list,newNode);

    for(int i=0;i<compacted_size;i++){
        _before[i] += 8;
        _after[i] += 8;
    }


    //...
    release();
    //...

    return compacted_size;
}



int available_memory() {
    int available_memory_size = get_total_Empty_Size();
    // Calculate available memory size
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = get_total_Used_Size();
    int allocated_chunks = List_countNodes(myalloc.used_blocks_list);
    int free_size = get_total_Empty_Size();
    int free_chunks = List_countNodes(myalloc.empty_blocks_list);
    int smallest_free_chunk_size = get_smallest_Empty_Block_size();
    int largest_free_chunk_size = get_largest_Empty_Block_size();

    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}



