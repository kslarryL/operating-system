#include "list.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * Allocate memory for a node of type struct Node and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct Node* List_createNode(void* _address){
	struct Node* ptr = (struct Node*)malloc(sizeof(struct Node));
	if(ptr != NULL){
		ptr->address = _address;
		ptr->next = NULL;
	}
	return ptr;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct Node **headRef, struct Node *node){
	node->next = *headRef;
	*headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct Node **headRef, struct Node *node){
	struct Node **temp = headRef;
	if(*headRef == NULL){
		*headRef = node;
	}
	else{
		while((**temp).next != NULL){
			temp = &((**temp).next);
		}
		(**temp).next = node;
	}
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct Node *head){
	struct Node *temp = head;
	int count = 0;
	while(temp != NULL){
		count++;
		temp = temp->next;
	}
	return count;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct Node* List_findNode(struct Node *head, void* _address){
	struct Node* temp = head;
	while(temp != NULL){
		if(temp->address == _address){
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node 
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list 
 * should be set to NULL.
 */
// void List_deleteNode (struct Node **headRef, struct Node *node){
// 	struct Node* temp = *headRef;
// 	if(List_countNodes(*headRef)==1){
// 		free(node);
// 		*headRef = NULL;
// 	}
// 	else if(List_countNodes(*headRef)>1){
// 		if(temp == node){
// 			*headRef = temp->next;
// 			free(node);
// 		}
// 		else{
// 			while(temp->next != NULL){
// 				if(temp->next == node){
// 					temp->next = temp->next->next;
// 					free(node);
// 					break;
// 				}
// 				temp = temp->next;
// 			}
// 		}
// 	}
// 	return;
// }

void List_deleteNode (struct Node **headRef, struct Node *node){
	struct Node* temp = *headRef;
	if(List_countNodes(*headRef)==1){
		free(node);
		*headRef = NULL;
	}
	else if(List_countNodes(*headRef)>1){
		if(temp == node){
			*headRef = temp->next;
			free(node);
		}
		else{
			while(temp->next != NULL){
				if(temp->next == node){
					if(temp->next->next == NULL){//if temp->next is the last node
						temp->next = NULL;
						free(node);
						break;
					}
					else{
						temp->next = temp->next->next;
						free(node);
						break;
					}
				}
				temp = temp->next;
			}
		}
	}
	return;
}
