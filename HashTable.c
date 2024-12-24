#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define EMPTY -1

int** initHashTable(int bucketNum, int slotNum) {
    int** hashTable = (int**) malloc (sizeof(int*) * bucketNum);
    for (int i=0; i<bucketNum; i++) {
        hashTable[i] = (int*) malloc (sizeof(int) * slotNum);
        for (int j=0; j<slotNum; j++) {
            hashTable[i][j] = EMPTY;
        }
    }
    return hashTable;
}

void search(int** hashTable, int target, int bucketNum, int slotNum) {
    int index = target % bucketNum;
    for (int i=0; i<bucketNum; i++) {
        for (int j=0; j<slotNum; j++) {
            if(hashTable[index][j] == target) {
                printf("%d %d\n", index, j);
                return;
            }
        }
        index = (index + 1) % bucketNum;
    }
}

int** insert(int** hashTable, int target, int bucketNum, int slotNum) {
    int index = target % bucketNum;
    for (int i=0; i<bucketNum; i++) {
        for (int j=0; j<slotNum; j++) {
            if (hashTable[index][j] == EMPTY) { // find empty place
                hashTable[index][j] = target;
                return hashTable;
            }
        }
        index = (index + 1) % bucketNum;
    }
    return hashTable; // full hashTable
}

int** rehashing(int** hashTable, int index, int bucketNum, int slotNum) {
    for (int i=0; i<bucketNum; i++) {
        for (int j=0; j<slotNum; j++) {
            if (hashTable[index][j] != EMPTY && hashTable[index][j] % bucketNum != index) {
                int tmp = hashTable[index][j];
                hashTable[index][j] = EMPTY;
                hashTable = insert(hashTable, tmp, bucketNum, slotNum);
            }
        }
        index = (index + 1) % bucketNum;
    }
    return hashTable;
}

int** delete(int** hashTable, int target, int bucketNum, int slotNum) {
    int index = target % bucketNum;
    for (int i=0; i<bucketNum; i++) {
        for (int j=0; j<slotNum; j++) {
            if (hashTable[index][j] == target) {
                // printf("delete %d %d\n", index, j);
                hashTable[index][j] = EMPTY;
                return hashTable;
            }
        }
        index = (index + 1) % bucketNum;
    }
    return hashTable; // not delete
}

void printHashTable(int** hashTable, int bucketNum, int slotNum) {
    for (int i=0; i < bucketNum; i++) {
        for(int j=0; j < slotNum; j++) {
            printf("%d ", hashTable[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int bucketNum = -1, slotNum = -1;
    char* operation = (char*) malloc (sizeof(char) * 10);

    scanf("%s %d", operation, &bucketNum);
    scanf("%s %d", operation, &slotNum);
    if (bucketNum == -1 || slotNum == -1) return 1;
    int** hashTable = initHashTable(bucketNum, slotNum);
    
    int targetValue = -1;
    while (scanf("%s", operation) != EOF) {
        if (strcmp(operation, "exit") == 0) {
            break;
        }
        else if (strcmp(operation, "insert") == 0) {
            scanf("%d", &targetValue);
            hashTable = insert(hashTable, targetValue, bucketNum, slotNum);
        }
        else if (strcmp(operation, "delete") == 0) {
            scanf("%d", &targetValue);
            hashTable = delete(hashTable, targetValue, bucketNum, slotNum);
        }
        else if (strcmp(operation, "search") == 0) {
            scanf("%d", &targetValue);
            search(hashTable, targetValue, bucketNum, slotNum);
        }
    }

    // printHashTable(hashTable, bucketNum, slotNum);
}