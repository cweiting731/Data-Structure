#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INF 10000
#define MAX_DEGREE 30

typedef struct Node Node;
int compareVal(const void *a, const void *b);
int compareDegree(const void *a, const void *b);
Node* createNode(int val);
Node* insert(Node* root, Node* newNode);
Node* extractMin(Node* root);
Node* consolidation(Node* root);
void printHeap(Node* root);
void test(Node* root);
Node* removeFromLayerAndInsertRootAndReturnParent(Node* root, Node* targetNode);
void decrease(Node* root, int target, int decreaseValue);

typedef struct Node {
    int val, degree;
    bool flag;
    Node* parent;
    Node* left;
    Node* right;
    Node* child;
}Node;

Node* INFNODE;
Node* minNode;
int rootLength = 0;

Node* createNode(int val) {
    Node* node = (Node*) malloc (sizeof(Node));
    node -> val = val;
    node -> left = node;
    node -> right = node;
    node -> parent = NULL;
    node -> child = NULL;
    node -> degree = 0;
    node -> flag = false;
    return node;
}

Node* insert(Node* root, Node* newNode) {
    if (newNode -> val < minNode -> val) minNode = newNode;
    if (root == NULL) root = newNode;
    else {
        Node* leftNode = root -> left;
        leftNode -> right = newNode;
        root -> left = newNode;
        newNode -> left = leftNode;
        newNode -> right = root;
    }
    return root;
}

Node* sortRootByVal(Node* root) {
    if (root == NULL) return NULL; // ensure having node in root layer
    // place in array and use qsort
    Node** rootArr = (Node**) malloc (100 * sizeof(Node*));
    int size = 0;
    for(Node* node = root; node != NULL; ) {
        rootArr[size++] = node;
        node = node -> right;
        if (node == root) break;
    }
    qsort(rootArr, size, sizeof(Node*), compareVal);
    // combine them again
    for(int i=0; i<size-1; i++) {
        rootArr[i] -> right = rootArr[i+1];
        rootArr[i+1] -> left = rootArr[i];
        // printf("%d ", rootArr[i] -> val);
    }
    rootArr[0] -> left = rootArr[size-1];
    rootArr[size-1] -> right = rootArr[0];
    root = rootArr[0];
    free(rootArr);
    return root;
}

Node* sortRootByDegree(Node* root) {
    if (root == NULL) return NULL; // ensure having node in root layer
    // place in array and use qsort
    Node** rootArr = (Node**) malloc (rootLength * sizeof(Node*));
    int rootArrIndex = 0;
    for(Node* node = root; node != NULL; ) {
        rootArr[rootArrIndex++] = node;
        node = node -> right;
        if (node == root) break;
    }
    qsort(rootArr, rootArrIndex, sizeof(Node*), compareDegree);
    // combine them again
    for(int i=0; i<rootArrIndex-1; i++) {
        rootArr[i] -> right = rootArr[i+1];
        rootArr[i+1] -> left = rootArr[i];
        // printf("%d ", rootArr[i]);
    }
    // printf("\n\n");
    rootArr[0] -> left = rootArr[rootArrIndex-1];
    rootArr[rootArrIndex-1] -> right = rootArr[0];
    root = rootArr[0];
    free(rootArr);
    return root;
}

Node* extractMin(Node* root) {
    // printf("%d\n", minNode -> child -> val);
    if (root == NULL) return NULL;
    Node* minNodeChild = minNode -> child;
    // printf("minNodeChild: %d\n", minNodeChild -> val);
    if (minNodeChild != NULL) {
        for(Node* node = minNodeChild; node != NULL; ) {
            // printf("%d -> ", minNodeChild -> val);
            // clear
            Node *nodeRight = node -> right;
            node -> parent = NULL; node -> left = node; node -> right = node; node -> flag = false;
            root = insert(root, node); rootLength++;
            node = nodeRight;
            if (node == minNodeChild) break;
        }
    }
    // printf("---------\n");
    // free minNode
    rootLength--;
    Node* minNodeLeft = minNode -> left;
    Node* minNodeRight = minNode -> right;
    if (minNodeRight == minNode) { // only minNode in root layer
        free(minNode); minNode = INFNODE; return NULL;
    }
    if (root == minNode) root = minNodeRight;
    minNodeLeft -> right = minNodeRight;
    minNodeRight -> left = minNodeLeft;
    free(minNode);
    // printf("before %d\n", rootLength);
    // test(root);
    return consolidation(root);
}

Node* consolidation(Node* root) {
    root = sortRootByVal(root);
    // test(root);
    Node** table = (Node**) calloc (MAX_DEGREE, sizeof(Node*));
    // int maxRootNum = rootLength;
    // int count = 0;
    for (Node* node = root; node != NULL; ) {
        Node* tmpNode = node;
        // count ++;
        // printf("-------------------node: %d, next node: %d\n", node -> val, node -> right -> val);
        node = node -> right; 
        while(1) {
            if (table[tmpNode -> degree] == NULL) { // find place
                // printf("find place: %d, degree: %d\n", tmpNode -> val, tmpNode -> degree);
                table[tmpNode -> degree] = tmpNode;
                // if (tmpNode -> degree != 0) test(tmpNode -> child);
                break;
            }
            // not find place
            Node* sameDegreeNode = table[tmpNode -> degree]; table[tmpNode -> degree] = NULL;
            if (tmpNode -> val > sameDegreeNode -> val) { // let tmpNode < sameDegreeNode
                Node* tmp = tmpNode;
                tmpNode = sameDegreeNode;
                sameDegreeNode = tmp;
            }
            // printf("not find place: %d %d\n", tmpNode -> val, sameDegreeNode -> val);

            // clear sameDegreeNode
            sameDegreeNode -> parent = tmpNode; sameDegreeNode -> left = sameDegreeNode; sameDegreeNode -> right = sameDegreeNode;
            tmpNode -> child = insert(tmpNode -> child, sameDegreeNode);
            tmpNode -> degree++;
            rootLength--;
        }
        if (node == root) break;
    }
    // combine them again
    qsort(table, MAX_DEGREE, sizeof(Node*), compareVal);
    for(int i=0; i<MAX_DEGREE-1; i++) {
        if (table[i+1] == NULL) break;
        table[i] -> right = table[i+1];
        table[i+1] -> left = table[i];
    }
    table[0] -> left = table[rootLength-1];
    table[rootLength-1] -> right = table[0];
    minNode = table[0];
    root = table[0];
    free(table);
    return root;
}

int compareVal(const void *a, const void *b) {
    Node* h1 = *(Node **)a;
    Node* h2 = *(Node **)b;
    if (h1 == NULL && h2 == NULL) return 0;
    if (h1 == NULL) return 1; // NULL place at the last
    if (h2 == NULL) return -1;
    return h1 -> val - h2 -> val;
}

int compareDegree(const void *a, const void *b) {
    Node* h1 = *(Node **)a;
    Node* h2 = *(Node **)b;
    return h1 -> degree - h2 -> degree;
}

void printHeap(Node* root) {
    root = sortRootByDegree(root);
    // test(root);
    for(Node* rootNode = root; rootNode != NULL; ) {
        printf("%d ", rootNode -> val); 
        if(rootNode -> degree == 0) { 
            printf("\n"); 
            rootNode = rootNode -> right;
            if (rootNode == root) break;
            continue; 
        }

        // printf("test: %d\n", rootNode -> child -> val);
        Node** headArr = (Node**) malloc (100 * sizeof(Node*));
        int start = 0, stop = 0;
        headArr[stop++] = rootNode -> child;
        while(start < stop) {
            // test(headArr[start]);
            Node* node = sortRootByVal(headArr[start++]);
            // test(node);
            for(Node* i = node; i != NULL; ) {
                // printf("now: %d, next: %d", i -> val, i -> right -> val);
                if (i -> child != NULL) {
                    headArr[stop++] = i -> child;
                    // printf("child: %d", i -> child -> val);
                }
                // printf("\n");
                // printf("(%d | left: %d, right: %d, parent: %d) ", i -> val, i -> left -> val, i -> right -> val, i -> parent -> val);
                printf("%d ", i -> val);
                i = i -> right;
                if (i == node) break;
            }
        }
        printf("\n");
        rootNode = rootNode -> right;
        if (rootNode == root) break;
    }
    
}

void decrease(Node* root, int target, int decreaseValue) {
    // search targetNode
    Node* targetNode = NULL;
    Node** headArr = (Node**) malloc (100 * sizeof(Node*));
    int start = 0, stop = 0;
    headArr[stop++] = root;
    while(start < stop && targetNode == NULL) {
        Node* layerHead = headArr[start++];
        for(Node* i = layerHead; i != NULL; ) {
            if (i -> val == target) {
                targetNode = i;
                break;
            }
            if (i -> child != NULL) headArr[stop++] = i -> child;
            i = i -> right;
            if(i == layerHead) break;
        } 
    }
    free(headArr);
    if(targetNode == NULL) return; // not find

    targetNode -> val -= decreaseValue;
    if (targetNode -> parent == NULL) { // targetNode is in the root layer
        if(targetNode -> val < minNode -> val) minNode = targetNode;
        return; 
    }
        
    if (targetNode -> parent -> val < targetNode -> val) return; // don't need to cut
    Node* parent = removeFromLayerAndInsertRootAndReturnParent(root, targetNode);

    do {
        if (parent == NULL) break;
        if (parent != NULL && !parent -> flag) { // parent's flag is false
            parent -> flag = true; 
            break;
        }
        targetNode = parent;
        parent = removeFromLayerAndInsertRootAndReturnParent(root, targetNode);
    } while(1);
}

Node* removeFromLayerAndInsertRootAndReturnParent(Node* root, Node* targetNode) {
    Node* parent = targetNode -> parent;
    if (parent != NULL) {
        // remove targetNode from its layer and insert in root layer
        if (targetNode -> right == targetNode) { // only targetNode in its layer
            parent -> child = NULL;
        }
        else {
            if (parent -> child == targetNode) { // pointer is on the targetNode
                parent -> child = targetNode -> right;
            }
            Node* left = targetNode -> left;
            Node* right = targetNode -> right;
            left -> right = right;
            right -> left = left;
        } 
        // clear targetNode
        targetNode -> parent = NULL; targetNode -> left = targetNode; targetNode -> right = targetNode; targetNode -> flag = false;
        root = insert(root, targetNode); rootLength++;
        parent -> degree -= 1;
    }
    return parent;
}

Node* delete(Node* root, int targetVal) {
    decrease(root, targetVal, INF);
    // printHeap(root); printf("minNode: %d\n", minNode -> val);
    return extractMin(root);
}

void test(Node* root) {
    for (Node* i = root; i != NULL; ) {
        printf("%d -> ", i -> val);
        i = i -> right;
        if (i == root) break;
    } 
    printf("\n");
}

int main() {
    char* operation = (char*) malloc (sizeof(char) * 15);
    int target = -1;
    INFNODE = createNode(INF);
    minNode = INFNODE;
    Node* root = NULL;

    // root = insert(root, createNode(10)); rootLength++;
    // root = insert(root, createNode(2)); rootLength++;
    // root = insert(root, createNode(5)); rootLength++;
    // root = insert(root, createNode(3)); rootLength++;
    // root = extractMin(root);
    // root = extractMin(root);
    // root = extractMin(root);
    // printHeap(root);

    while (scanf("%s", operation) != EOF) {
        if (strcmp(operation, "exit") == 0) {
            printHeap(root);
            break;
        }
        else if (strcmp(operation, "insert") == 0) {
            scanf("%d", &target);
            root = insert(root, createNode(target)); rootLength++;
        }
        else if (strcmp(operation, "delete") == 0) {
            scanf("%d", &target);
            root = delete(root, target);
        }
        else if (strcmp(operation, "decrease") == 0) {
            int decreaseValue;
            scanf("%d %d", &target, &decreaseValue);
            decrease(root, target, decreaseValue);
        }
        else if (strcmp(operation, "extract-min") == 0) {
            root = extractMin(root);
        }
    }
    
    
}