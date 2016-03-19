#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define MAX_INPUT_BUFFER_LENGTH 1024
memoryList _memlist = NULL;;

void       freeList(memoryList _memList){
	while (_memList != NULL) {
		struct memoryNode* target = _memList;
		_memList= _memList->next;
		if (target->ptr != NULL) free(target->ptr);
		target->ptr = NULL;
		free(target);
	}
	free(_memList);
}
void*      mallocList(size_t _size, memoryList _memList){
	if (_memList == NULL) return NULL;
	struct memoryNode* currentNode = (struct memoryNode*)malloc(sizeof(struct memoryNode));
	if (currentNode == NULL) return NULL;
	(currentNode->ptr) = malloc(_size);
	(currentNode->size) = _size;
	(currentNode->next) = NULL;
	struct memoryNode* lastNode = _memList;
	while (lastNode->next != NULL) lastNode = lastNode->next;
	lastNode->next = currentNode;
	return currentNode->ptr;
}
size_t     sizeList(memoryList _list){
	size_t cum = 0L;
	while (_list != NULL){
		cum = cum + (_list->size);
		_list = (_list->next);
	}
	return cum;
}	
memoryList newMemoryList() {
	memoryList returnList = (struct memoryNode*)malloc(sizeof(struct memoryNode));
	(returnList->ptr) = NULL;
	(returnList->size) = 0L;
	(returnList->next) = NULL;
	return returnList;
}
char*      concatString(const char* _str1, const char* _str2){
	size_t strsize_1 = strlen(_str1) * sizeof(char) + sizeof(char);
	size_t strsize_2 = strlen(_str2) * sizeof(char) + sizeof(char);
	char* returnStr = (char*)mallocList(strsize_1 + strsize_2, _memlist);
	strcpy(returnStr, _str1);
	strcat(returnStr, _str2);
	return returnStr;
}
char*      reverseString(const char* _str1){
	size_t strsize = strlen(_str1) * sizeof(char)+sizeof(char);
	char* returnStr = (char*)mallocList(strsize, _memlist);
	memset(returnStr, '\0', strsize);
	for (long iter = (long)strlen(_str1)-1; iter >= 0L; iter--){
		returnStr[strlen(_str1)-iter-1] = _str1[iter];
	}
	return returnStr;
}
char*      scanString(){
	char* returnPtr = (char*)mallocList(sizeof(char)*MAX_INPUT_BUFFER_LENGTH, _memlist);
	scanf("%s", returnPtr);
	return returnPtr;
}
void       initMemoryPool(){_memlist = newMemoryList();}
void       exitMemoryPool(){freeList(_memlist);}