#ifndef __MINI__MEMORY__H__
#define __MINI__MEMORY__H__

#include <stdlib.h>

struct memoryNode {
	void*				ptr;
	size_t				size;
	struct memoryNode* 	next;
};
typedef struct memoryNode* memoryList;
void freeList(memoryList);
void* mallocList(size_t, memoryList);
size_t sizeList(memoryList);
memoryList newMemoryList();

void freeList(memoryList _memList){
	while (_memList != NULL) {
		struct memoryNode* target = _memList;
		_memList= _memList->next;
		if (target->ptr != NULL) free(target->ptr);
		target->ptr = NULL;
		free(target);
	}
	free(_memList);
}
void* mallocList(size_t _size, memoryList _memList){
	if (_memList == NULL) return NULL;
	struct memoryNode* currentNode = (struct memoryNode*)
									 malloc(sizeof(struct memoryNode));
	if (currentNode == NULL) return NULL;
	(currentNode->ptr) = malloc(_size);
	(currentNode->size) = _size;
	(currentNode->next) = NULL;
	struct memoryNode* lastNode = _memList;
	while (lastNode->next != NULL) lastNode = lastNode->next;
	lastNode->next = currentNode;
	return currentNode->ptr;
}
size_t sizeList(memoryList _list){
	size_t cum = 0L;
	while (_list != NULL){
		cum = cum + (_list->size);
		_list = (_list->next);
	}
	return cum;
}
memoryList newMemoryList() {
	memoryList returnList = (struct memoryNode*)
		                    malloc(sizeof(struct memoryNode));
	(returnList->ptr) = NULL;
	(returnList->size) = 0L;
	(returnList->next) = NULL;
	return returnList;
}

#endif