#ifndef LIST_H   
#define LIST_H

struct Node {
  void* address;
  struct Node *next;
};

struct Node* List_createNode(void* _address);
void List_insertHead (struct Node **headRef, struct Node *node);
void List_insertTail (struct Node **headRef, struct Node *node);
int List_countNodes (struct Node *head);
struct Node* List_findNode(struct Node *head, void* _address);
void List_deleteNode (struct Node **headRef, struct Node *node);

#endif
