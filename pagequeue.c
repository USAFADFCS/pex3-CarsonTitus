/** pagequeue.c
 * ===========================================================
 * Name: Carson Titus, 04-07-2026
 * Section: CS483 / M3
 * Project: PEX3 - Page Replacement Simulator
 * Purpose: Implementation of the PageQueue ADT — a doubly-linked
 *          list for LRU page replacement.
 *          Head = LRU (eviction end), Tail = MRU end.
 * =========================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "pagequeue.h"

/**
 * @brief Create and initialize a page queue with a given capacity
 */
PageQueue *pqInit(unsigned int maxSize) {
    // TODO: malloc a PageQueue, set head and tail to NULL,
    //       size to 0, maxSize to maxSize, and return the pointer
    PageQueue* queue = (PageQueue*)malloc(sizeof(PageQueue));
    if (queue == NULL){
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->maxSize = maxSize;

    return queue;
}

/**
 * @brief Access a page in the queue (simulates a memory reference)
 */
long pqAccess(PageQueue *pq, unsigned long pageNum) {
    // TODO: Search the queue for pageNum (suggest searching tail->head
    // so you naturally count depth from the MRU end).
    // COMBINATION OF INSERT AND DELETE ELEMENT FUNCTIONS
    long depth = 0;
    PqNode *current = pq->tail;

    while (current != NULL){
        if (current->pageNum == pageNum){
            break; // now current holds the node at the correct depth where the pagenum was found. 
        }
        current = current->prev; //searching from tail->head
        depth++;

    }
    // HIT path (page found at depth d): occurs when current != NULL, correct pagenum was found
    //   - Remove the node from its current position and re-insert
    //     it at the tail (most recently used).
    //   - Return d.
    if (current != NULL){
        if (depth == 0){ // if were already at the tail skip everything else
            return depth; // not having this caused like 90 minutes of infinite loops. 
        }
        if (current == pq->head){ // if the pagenum is found at the head
            pq->head = current->next;
            if (pq->head != NULL){
                pq->head->prev = NULL;
            }
        }
        else { // if the pagenum is found anywhere in the middle
            if (current->next != NULL){
                current->next->prev = current->prev;
            }
            if (current->prev != NULL){
                current->prev->next = current->next;
            }
        }
        // Logic pulled from "insetElementLinkedList" from CS220. 
        // current from CS220 code was the tail, so reaplce with pq->tail in this code
        // newNode from CS220 code was the current/appended node, so replace it with current, what is appended 
        current->prev = pq->tail; //current is looking at old tail now
        pq->tail->next = current; // old tail's next was NULL since it was the tail, now it is 
        current->next = NULL; // current is now new tail so next has to be null (b/c tail's next is always null)
        pq->tail = current; // make the tail current so it is the new tail.
        return depth;
    }
    // MISS path (page not found): occurs when current == NULL, nothing was found
    //   - Allocate a new node for pageNum and insert it at the tail.
    //   - If size now exceeds maxSize, evict the head node (free it).
    //   - Return -1.
    else {
        // allocating new node for pagenum and inserting at tail
        // all pulled from CS220 code replacing vars as stated above in the hit path code. 
        PqNode *newNode = (PqNode*)malloc(sizeof(PqNode));
        newNode->pageNum = pageNum; // lines 97-103 pulled from lines 163-172 of CS220 code.
        newNode->next = NULL;
        newNode->prev = NULL;

        if (pq->tail == NULL){
            pq->head = newNode;
            pq->tail = newNode;
        }
        else {
            newNode->prev = pq->tail;
            pq->tail->next = newNode;
            pq->tail = newNode;
        }

        pq->size++;
        // eviciting head node if over size
        if (pq->size > pq->maxSize){ // checking if the current size is greater than max allowed size
            PqNode *temp = pq->head;
            pq->head = pq->head->next; // pulled directly from "deleteElementLinkedList", changing "list" to "pq"

            if (pq->head != NULL){ // pulled directly from "deleteElementLinkedList", changing "list" to "pq"
                pq->head->prev = NULL;
            }
            free(temp); // freeing the head node since we are over max size
            pq->size--; // decrement the size after removing 
        }
        return -1;
    }
    return -1;
}

/**
 * @brief Free all nodes in the queue and reset it to empty
 */
void pqFree(PageQueue *pq) {
    // pulled from deleteLinkedList function
    if (pq == NULL){ 
        return;
    }
    PqNode *current = pq->head;
    PqNode *temp;
    while (current != NULL){
        temp = current;
        current = current->next;
        free(temp);
    }
    free(pq);
}

/**
 * @brief Print queue contents to stderr for debugging
 */
void pqPrint(PageQueue *pq) {
    // pulled from printLinkedList function
    if (pq == NULL){
        printf("Invalid Page Queue.\n");
        return;
    }
    if (pq->head == NULL){
        printf("Page Queue is empty.\n");
        return;
    }
    PqNode *current = pq->head;
    while (current != NULL){
        printf("%lu", current->pageNum);
        current = current->next;
    }
    printf("NULL\n");
}
