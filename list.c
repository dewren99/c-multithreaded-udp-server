#include <stddef.h>
#include "list.h"

static bool INITIAL_SETUP_DONE = false;

static Node nodes[LIST_MAX_NUM_NODES];
static unsigned int nodes_free_i = 0;
static unsigned int nodes_used = 0;

static List heads[LIST_MAX_NUM_HEADS];
static unsigned int heads_free_i = 0;
static unsigned int heads_used = 0;

static void increment_free_node_mem_i(){
    nodes_free_i = (LIST_MAX_NUM_NODES + nodes_free_i + 1) % LIST_MAX_NUM_NODES;
}
static void decrement_free_node_mem_i(){
    nodes_free_i = (LIST_MAX_NUM_NODES + nodes_free_i - 1) % LIST_MAX_NUM_NODES;
}

static void increment_free_head_mem_i(){
    heads_free_i = (LIST_MAX_NUM_HEADS + heads_free_i + 1) % LIST_MAX_NUM_HEADS;
}
static void decrement_free_head_mem_i(){
    heads_free_i = (LIST_MAX_NUM_HEADS + heads_free_i - 1) % LIST_MAX_NUM_HEADS;
}

static void reset_and_free_head(List* pList){
    unsigned int current_head_memory_index = pList->headMemoryIndex;
    heads_used--;
    pList->currentNodeIndex = -1;
    pList->currentNode = NULL;
    pList->head = NULL;
    pList->tail = NULL;
    pList->length = 0;
    pList->used =false;
    decrement_free_head_mem_i();
    heads[heads_free_i].headNextFreeMemoryIndex = current_head_memory_index;
}

List* List_create(){
    if(LIST_MAX_NUM_HEADS == heads_used){
        return NULL;
    }

    if(!INITIAL_SETUP_DONE){
        for(unsigned int i = 0; i<LIST_MAX_NUM_NODES; i++){
            nodes[i].nodeMemoryIndex = i;
            nodes[i].nodeNextFreeMemoryIndex = (i+1) % LIST_MAX_NUM_NODES;
        }
        for(unsigned int i = 0; i<LIST_MAX_NUM_HEADS; i++){
            heads[i].headMemoryIndex = i;
            heads[i].headNextFreeMemoryIndex = (i+1) % LIST_MAX_NUM_HEADS;
        }
        INITIAL_SETUP_DONE = true;
    }
    unsigned int free_mem_i = heads[heads_free_i].headNextFreeMemoryIndex;
    List *list = &(heads[free_mem_i]);
    increment_free_head_mem_i();
    heads_used++;
    
    list->length = 0;
    list->currentNode = NULL;
    list->currentNodeIndex = -1;
    list->tail = NULL;
    list->head = NULL;
    list->used = true;

    return list;
}

int List_count(List* pList){
    return pList->length;
}

void* List_first(List* pList){
    if(!pList || !pList->length){
        pList->currentNode = NULL;
        pList->currentNodeIndex = -1;
        return NULL;
    }
    pList->currentNodeIndex = 0;
    pList->currentNode = pList->head;
    return pList->currentNode->item;
}

void* List_last(List* pList){
    if(!pList || !pList->length){
        pList->currentNode = NULL;
        pList->currentNodeIndex = -1;
        return NULL;
    }
    pList->currentNodeIndex = pList->length - 1;
    pList->currentNode = pList->tail;
    return pList->currentNode->item;
}

void* List_next(List* pList){
    if(
        !pList || 
        !pList->length ||
        pList->currentNodeIndex > (pList->length - 1)
    ){
        return NULL;
    }
    //current item beyond the start of the pList - point to head
    else if(pList->currentNodeIndex < 0){
        pList->currentNodeIndex = 0;
        pList->currentNode = pList->head;
        return pList->currentNode->item;
    }
    else if(pList->currentNode->nextNode){
        pList->currentNodeIndex++;
        pList->currentNode = pList->currentNode->nextNode;
        return pList->currentNode->item;
    }
    //operation advances the current item beyond the end of the pList
    else{
        pList->currentNode = NULL;
        pList->currentNodeIndex = pList->length;
    }
    return NULL;
}

void* List_prev(List* pList){
    if(
        !pList || 
        !pList->length ||
        pList->currentNodeIndex < 0
    ){
        return NULL;
    }
    //current item beyond the end of the pList - point to tail
    else if(pList->currentNodeIndex > (pList->length - 1)){
        pList->currentNodeIndex = pList->length - 1;
        pList->currentNode = pList->tail;
        return pList->currentNode->item;
    }
    else if(pList->currentNode->prevNode){
        pList->currentNode = pList->currentNode->prevNode;
        pList->currentNodeIndex--;
        return pList->currentNode->item;
    }
    //operation advances the current item beyond the start of the pList
    else{
        pList->currentNodeIndex--; //pList->currentNodeIndex = -1
        pList->currentNode = NULL;
    }
    return NULL;
}

void* List_curr(List* pList){
    if(
        !pList || 
        !pList->length ||
        !pList->currentNode
    ){
        return NULL;
    }
    return pList->currentNode->item;
}

int List_add(List* pList, void* pItem){
    if(nodes_used >= LIST_MAX_NUM_NODES || !pList || !pItem){
        return LIST_FAIL;
    }
    unsigned int free_mem_i = nodes[nodes_free_i].nodeNextFreeMemoryIndex;
    Node *new_node = &(nodes[free_mem_i]);
    increment_free_node_mem_i();
    new_node->item = pItem;

    nodes_used++;

    // if list empty, head = tail = current node 
    if(!pList->head){ 
        new_node->nextNode = NULL;
        new_node->prevNode = NULL;
        pList->head = new_node;
        pList->tail = new_node;
    }
    // if current pointer is before head
    else if(pList->currentNodeIndex == -1){
        new_node->nextNode = pList->head;
        new_node->prevNode = NULL;
        pList->head->prevNode = new_node;
        pList->head = new_node;
    }
    // if current pointer is tail or after tail
    else if(
        pList->currentNodeIndex >= pList->length ||
        pList->currentNodeIndex+1 == pList->length
    ){ 
        new_node->nextNode = NULL;
        new_node->prevNode = pList->tail;
        pList->tail->nextNode = new_node;
        pList->tail = new_node;
        if(pList->currentNodeIndex >= pList->length){
            pList->currentNodeIndex = pList->length - 1;
        }
    }
    else{
        if(pList->currentNode->nextNode){
            pList->currentNode->nextNode->prevNode = new_node;
        }
        new_node->nextNode = pList->currentNode->nextNode;
        pList->currentNode->nextNode = new_node;
        new_node->prevNode = pList->currentNode;
    }
    //current node set to new node as long as adding operation is successful
    pList->currentNode = new_node;
    pList->currentNodeIndex++;
    pList->length++;
    return LIST_SUCCESS;
}

int List_insert(List* pList, void* pItem){
    if(nodes_used >= LIST_MAX_NUM_NODES || !pList || !pItem){
        return LIST_FAIL;
    }

    unsigned int free_mem_i = nodes[nodes_free_i].nodeNextFreeMemoryIndex;
    Node *new_node = &(nodes[free_mem_i]);
    increment_free_node_mem_i();

    // Node *new_node = &(nodes[nodes_i]);
    new_node->item = pItem;
    // new_node->nodeMemoryIndex = nodes_i;
    // nodes_i++;
    nodes_used++;
    if(!pList->length){
        new_node->nextNode = NULL;
        new_node->prevNode = NULL;
        pList->head = new_node;
        pList->tail = new_node;
        pList->currentNodeIndex = 0;
    }
    // if current pointer is head
    else if(pList->currentNodeIndex == 0){
        pList->currentNode->prevNode = new_node;
        new_node->nextNode = pList->currentNode;
        new_node->prevNode = NULL;
        pList->head = new_node;
    }
    // if current pointer is before head
    else if(pList->currentNodeIndex < 0){
        pList->head->prevNode = new_node;
        new_node->nextNode = pList->head;
        new_node->prevNode = NULL;
        pList->head = new_node;
        pList->currentNodeIndex = 0;
    }
    // if current pointer is after tail
    else if(pList->currentNodeIndex >= pList->length){ 
        new_node->nextNode = NULL;
        new_node->prevNode = pList->tail;
        pList->tail->nextNode = new_node;
        pList->tail = new_node;
    }
    else{
        pList->currentNode->prevNode->nextNode = new_node;
        new_node->nextNode = pList->currentNode;
        new_node->prevNode = pList->currentNode->prevNode;
        pList->currentNode->prevNode = new_node;
    }
    pList->currentNode = new_node;
    pList->length++;
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    if(nodes_used >= LIST_MAX_NUM_NODES || !pList || !pItem){
        return LIST_FAIL;
    }
    unsigned int free_mem_i = nodes[nodes_free_i].nodeNextFreeMemoryIndex;
    Node *new_node = &(nodes[free_mem_i]);
    increment_free_node_mem_i();
    new_node->item = pItem;
    nodes_used++;
    new_node->nextNode = NULL;

    // if list empty head = tail
    if(!pList->length){
        new_node->prevNode = NULL;
        pList->head = new_node;
    }
    else{
        new_node->prevNode = pList->tail;
        pList->tail->nextNode = new_node;
    }
    pList->tail = new_node;
    pList->currentNode = new_node;
    pList->length++;
    pList->currentNodeIndex = pList->length - 1;
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    if(nodes_used >= LIST_MAX_NUM_NODES || !pList || !pItem){
        return LIST_FAIL;
    }

    unsigned int free_mem_i = nodes[nodes_free_i].nodeNextFreeMemoryIndex;
    Node *new_node = &(nodes[free_mem_i]);
    increment_free_node_mem_i();
    new_node->item = pItem;
    nodes_used++;
    new_node->prevNode = NULL;

    // if list empty head = tail
    if(!pList->length){
        new_node->nextNode = NULL;
        pList->tail = new_node;
    }
    else{
        new_node->nextNode = pList->head;
        pList->head->prevNode = new_node;
    }

    pList->head = new_node;
    pList->currentNode = new_node;
    pList->currentNodeIndex = 0;
    pList->length++;
    return LIST_SUCCESS;
}

void* List_remove(List* pList){
    // pList->currentNodeIndex < 0 and pList->currentNodeIndex >= pList->length checks are unnecessary
    // since !pList->currentNode check takes care of things but
    // you can technically force currentNode to have a value and have the currentNodeIndex point beyond the list limit
    if(
        !pList || 
        !pList->length ||
        !pList->currentNode ||
        pList->currentNodeIndex < 0 ||
        pList->currentNodeIndex >= pList->length
    ){
        return NULL;
    }
    void* current_item = pList->currentNode->item;
    const unsigned int current_node_memory_index = pList->currentNode->nodeMemoryIndex;

    if(pList->currentNode == pList->head){
        pList->head = pList->currentNode->nextNode;
        pList->currentNode->nextNode->prevNode = NULL;
    }
    else if(pList->currentNode->prevNode){
        pList->currentNode->prevNode->nextNode = pList->currentNode->nextNode? pList->currentNode->nextNode : NULL;
    }

    if(pList->currentNode == pList->tail){
        pList->tail = pList->currentNode->prevNode;
    }
    else if(pList->currentNode->nextNode){
        pList->currentNode->nextNode->prevNode = pList->currentNode->prevNode? pList->currentNode->prevNode : NULL;
    }
 
    pList->currentNode = pList->currentNode->nextNode;
    pList->length--;
    nodes_used--;

    decrement_free_node_mem_i();
    nodes[nodes_free_i].nodeNextFreeMemoryIndex = current_node_memory_index;

    return current_item;
}

void List_concat(List* pList1, List* pList2){
    if(
        !pList1 || 
        !pList2 ||
        !pList1->used || 
        !pList2->used
    ){
        return;
    }

    if(pList1->tail){
        pList1->tail->nextNode = pList2->head;
    }
    if(pList2->head){
        pList2->head->prevNode = pList1->tail;
    }
    if(pList2->tail){
        pList1->tail = pList2->tail;
    }
    if(!pList1->length){
        pList1->head = pList2->head;
    }

    pList1->length += pList2->length;

    reset_and_free_head(pList2);
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    if(!pList || !pItemFreeFn || !pList->used){
        return;
    }

    Node *p = pList->head;
    while(p){
        unsigned int current_node_memory_index = p->nodeMemoryIndex;
        (*pItemFreeFn)(p->item);
        p = p->nextNode;

        nodes_used--;
        decrement_free_node_mem_i();
        nodes[nodes_free_i].nodeNextFreeMemoryIndex = current_node_memory_index;
    }

    reset_and_free_head(pList);
}

void* List_trim(List* pList){
    if(!pList || !pList->tail){
        return NULL;
    }
    unsigned int current_node_memory_index = pList->tail->nodeMemoryIndex;
    void* item = pList->tail->item;

    if(pList->tail == pList->head){
        pList->head = pList->tail->prevNode;
    }
    pList->tail = pList->tail->prevNode;
    if(pList->tail){
        pList->tail->nextNode = NULL;
    }
    pList->currentNode = pList->tail;
    pList->length--;
    
    nodes_used--;
    decrement_free_node_mem_i();
    nodes[nodes_free_i].nodeNextFreeMemoryIndex = current_node_memory_index;
    return item;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    if(!pList || !pList->length){
        return NULL;
    }

    Node *p = pList->currentNode;
    if(pList->currentNodeIndex < 0){
        p = pList->head;
        pList->currentNodeIndex = 0;
    }

    void *item_found = NULL;
    while(p){
        const bool success = pComparator(p->item, pComparisonArg);
        if(success){
            item_found = p->item;
            break;
        }
        p = p->nextNode;
        pList->currentNode = p;
        pList->currentNodeIndex++;
    }
    return item_found;
}
