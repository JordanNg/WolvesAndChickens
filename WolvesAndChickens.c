#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Riverbank {
    int chickens, wolves, boat;
};

struct State {
    struct Riverbank left;
    struct Riverbank right;
    int id;
};

struct Frontier {
    int front, rear, size;
    unsigned int capacity;
    struct State* array;  
};

struct Graph {
    struct State currentState;
    struct State goalState;

    struct Frontier* queue;
    struct State* visitedArray;
};

int genSuccs(struct State currentState, struct State *successors, int rear);
int BFS(struct Graph graph);
struct Graph initializeGraph(struct State startState, struct State goalState, unsigned int capacity);
struct Frontier* initializeFrontier(unsigned capacity);
int isGoalState(struct State currentState, struct State goalState);
void allocateMore(struct State *queue, int currCapacity);

int main(int argc, char* argv[]) {
    
    // Usage checking
    //if (argc != 4)
    if (argc != 3)
        printf("ERROR: Incorrect usage. Proper usage is ./WolvesAndChickens <StartingState> <GoalState> <SearchMode> <OutputFile>\n");

    FILE *fp;
    char buff[255];

    char startbuff[255];
    char goalbuff[255];

    int s;
    int g;

    // Initialize riverbanks for start and goal states
    int sLeftChicks, sLeftWolves, sLeftBoat;
    int sRightChicks, sRightWolves, sRightBoat;

    int gLeftChicks, gLeftWolves, gLeftBoat;
    int gRightChicks, gRightWolves, gRightBoat;

    // Using a variable to hold the start and goal file names
    // Start buffer should be 1 and goal buffer should be 2
    snprintf(startbuff, sizeof(startbuff),"%s", argv[1]);
    snprintf(goalbuff, sizeof(goalbuff),"%s", argv[2]);

    // Read the start file
    fp = fopen(startbuff, "r");
    // If unable to open the file
    if (fp == NULL) {
        printf("ERROR: Could not open file");
        return -1;
    }
    // Read the file into variables
    struct State startState;
    fscanf(fp, "%d,%d,%d", &startState.left.chickens, &startState.left.wolves, &startState.left.boat);
    fscanf(fp, "%d,%d,%d", &startState.right.chickens, &startState.right.wolves, &startState.right.boat);

    fclose(fp);

    // Read goal file
    fp = fopen(goalbuff, "r");
    // If unable to open the file
    if (fp == NULL) {
        printf("ERROR: Could not open file");
        return -1;
    }
    // Read the file into variables
    struct State goalState;
    fscanf(fp, "%d,%d,%d", &goalState.left.chickens, &goalState.left.wolves, &goalState.left.boat);
    fscanf(fp, "%d,%d,%d", &goalState.right.chickens, &goalState.right.wolves, &goalState.right.boat);

    fclose(fp);

    /* ******************************************* */

    int len = 100;
    //TODO: Call Breadth - First - Search
    // Do intialization of the starting state
    // Initialize Frontier 
    struct Graph graph = initializeGraph(startState, goalState, len);
    int i;
    printf("Initial State: \n");
    for (i = 0; i < graph.queue->size; i++) {
        printf("Left Bank: %d chickens, %d wolves, %d boats\n", graph.queue->array[i].left.chickens, graph.queue->array[i].left.wolves, graph.queue->array[i].left.boat);
        printf("Right Bank: %d chickens, %d wolves, %d boats\n", graph.queue->array[i].right.chickens, graph.queue->array[i].right.wolves, graph.queue->array[i].right.boat); 
    }
    printf("\n");
   
    int BFSResult = BFS(graph); 

    printf("BFS returned a: %d\n", BFSResult);

    return 0;
}


//TODO: Breadth-First-Search
// Returns a 0 on a success and a 1 on a failure
// Implemented using a FIFO queue
int BFS(struct Graph graph) {
    // Check if in a goal state
    int checkGoal = isGoalState(graph.currentState, graph.goalState);
    if (!checkGoal) {
        return 0;
    }
    // If the frontier is empty the return failure
    if (graph.queue->size == 0) {
        return 1;
    }
    while (graph.queue->size != 0) {
    //int j;
    //for (j = 0; j < 10; j++) { 
        // Choose the first node in the frontier
        graph.currentState = graph.queue->array[graph.queue->front];
        graph.queue->front = (graph.queue->front + 1) % graph.queue->capacity;
        graph.queue->size = graph.queue->size - 1;

        printf("FRONT: %d\n", graph.queue->front);
        printf("REAR: %d\n", graph.queue->rear);
        printf("SIZE: %d\n", graph.queue->size);
        printf("CAPACITY: %d\n", graph.queue->capacity);
        
        // If the node is a goal state return solution
        checkGoal = isGoalState(graph.currentState, graph.goalState);
        if (!checkGoal) {
            return 0;
        }

        // Add node to the explored set
        // TODO:
        // only if not in the frontier or explored set
        if (graph.queue->size >= graph.queue->capacity / 2) {
            graph.queue->capacity = graph.queue->capacity * 2;
            allocateMore(graph.queue->array, graph.queue->capacity);
            //graph.queue->array = (struct State*) realloc(graph.queue->array, graph.queue->capacity * sizeof(struct State));
        }

        int numSuccs = genSuccs(graph.currentState, graph.queue->array, graph.queue->rear);
        printf("NUM SUCCESSORS ADDED: %d\n", numSuccs);
        graph.queue->size = graph.queue->size + numSuccs;
        graph.queue->rear = graph.queue->rear + numSuccs;

        int i;
        for (i = graph.queue->front; i < graph.queue->size + graph.queue->front; i++) {
            printf("Left Bank: %d chickens, %d wolves, %d boats\n", graph.queue->array[i].left.chickens, graph.queue->array[i].left.wolves, graph.queue->array[i].left.boat);
            printf("Right Bank: %d chickens, %d wolves, %d boats\n", graph.queue->array[i].right.chickens, graph.queue->array[i].right.wolves, graph.queue->array[i].right.boat);
        }
        printf("\n");
    }
}


//TODO: Depth-First-Search
void DFS() {
}


//TODO: Iterative-Deepening Depth-First-Search
void IDDFS() {
}

// Initiailize the graph and the frontier for the first time based on the initial state
struct Graph initializeGraph(struct State startState, struct State goalState, unsigned int capacity) {
    struct Graph graph;
    graph.currentState = startState;
    graph.goalState = goalState;
    
    // Initialize the Frontier
    graph.queue = initializeFrontier(capacity);

    // Initialize the visitedArray
    graph.visitedArray = (struct State*) malloc(10 * sizeof(struct State));

    // Generate Successors, if there are no successors then check if goal state if not failure
    graph.queue->rear = (graph.queue->rear + 1) % graph.queue->capacity;
    int numSuccs = genSuccs(graph.currentState, graph.queue->array, graph.queue->rear);

    graph.queue->rear = (graph.queue->rear + numSuccs) % graph.queue->capacity;
    graph.queue->size = graph.queue->size + numSuccs;

    return graph;
}

// Helper function to initialize the frontier array
struct Frontier* initializeFrontier(unsigned capacity) {
    struct Frontier* frontier = (struct Frontier*) malloc(sizeof(struct Frontier));
    frontier->capacity = capacity;
    frontier->front = frontier->size = 0;
    frontier->rear = capacity - 1;
    frontier->array = (struct State*) malloc(frontier->capacity * sizeof(struct State));
    return frontier;
}

void allocateMore(struct State *queue, int currCapacity) {
    queue = (struct State*) realloc(queue, currCapacity * sizeof(struct State));
}

// Helper function to check if the two states are equivalent
// returns 0 on success and 1 on a failure
int isGoalState(struct State currentState, struct State goalState) {
    if (currentState.left.chickens == goalState.left.chickens && currentState.left.wolves == goalState.left.wolves 
    && currentState.left.boat == goalState.left.boat) {
        if (currentState.right.chickens == goalState.right.chickens && currentState.right.wolves == goalState.right.wolves 
        && currentState.right.boat == goalState.right.boat) {    
            return 0;
        }
    } else {
        return 1;
    }
}

// Generate successors and add it to the queue in FIFO order
// Returns the new size of the passed in array
int genSuccs(struct State currentState, struct State *successors, int rear) {
    
    int boatOnLeft = 0;
    // Determine where the boat is 
    if (currentState.left.boat == 1)
        boatOnLeft = 1;
    else 
        boatOnLeft = 0;

    int numSuccs = 0;
    // Required order of generation
    if (boatOnLeft) {
        // Put one chicken in the boat
        if (currentState.left.chickens != 0 ) {
            if (currentState.left.chickens - 1 >= currentState.left.wolves || currentState.left.wolves == 0 || currentState.left.chickens - 1 == 0) {
                if (currentState.right.chickens + 1 >= currentState.right.wolves || currentState.right.wolves == 0) {
                    // Add new state to the array
                    struct State new = currentState;
                    new.left.chickens -= 1;
                    new.right.chickens += 1;
                    new.left.boat = 0;
                    new.right.boat = 1;
                
                    successors[rear] = new;
                    rear++;
                    numSuccs++;
                }
            }
        }
        // Put two chickens in the boat
        if (currentState.left.chickens >= 2) {
            if (currentState.left.chickens - 2 >= currentState.left.wolves || currentState.left.wolves == 0 || currentState.left.chickens - 2 == 0) {
                if (currentState.right.chickens + 2 >= currentState.right.wolves || currentState.right.wolves == 0) {
                    // Add to the array
                    struct State new = currentState;
                    new.left.chickens -= 2;
                    new.right.chickens += 2;
                    new.left.boat = 0;
                    new.right.boat = 1;
                
                    successors[rear] = new;
                    rear++;
                    numSuccs++;
                }
            }
        }
        // Put one wolf in the boat
        if (currentState.left.wolves != 0) {
            if (currentState.right.chickens >= currentState.right.wolves + 1 || currentState.right.chickens == 0) {
                // Add to the array
                struct State new = currentState;
                new.left.wolves -= 1;
                new.right.wolves += 1;
                new.left.boat = 0;
                new.right.boat = 1;
         
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }
        // Put one wolf and one chicken in the boat
        if (currentState.left.chickens != 0 && currentState.left.wolves != 0) {
            if (!(currentState.right.wolves > currentState.right.chickens)) {
                // Add to the array
                struct State new = currentState;
                new.left.chickens -= 1;
                new.right.chickens += 1;
                new.left.wolves -= 1;
                new.right.wolves += 1;
                new.left.boat = 0;
                new.right.boat = 1;
                
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }
        // Put two wolves in the boat
        if (currentState.left.wolves >= 2) {
            if (currentState.right.chickens >= currentState.right.wolves + 2 || currentState.right.chickens == 0) {
                // Add to the array
                struct State new = currentState;
                new.left.wolves -= 2;
                new.right.wolves += 2;
                new.left.boat = 0;
                new.right.boat = 1;
               
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }

        return numSuccs;
    } else { 
        // Put one chicken in the boat
        if (currentState.right.chickens != 0 ) {
            if (currentState.right.chickens - 1 >= currentState.right.wolves || currentState.right.wolves == 0 || currentState.right.chickens - 1 == 0) {
                if (currentState.left.chickens + 1 >= currentState.left.wolves || currentState.left.wolves == 0) {
                    // Add new state to the array
                    struct State new = currentState;
                    new.right.chickens -= 1;
                    new.left.chickens += 1;
                    new.right.boat = 0;
                    new.left.boat = 1;
                
                    successors[rear] = new;
                    rear++;
                    numSuccs++;
                }
            }
        }
        // Put two chickens in the boat
        if (currentState.right.chickens >= 2) {
            if (currentState.right.chickens - 2 >= currentState.right.wolves || currentState.right.wolves == 0 || currentState.right.chickens - 2 == 0) {
                if (currentState.left.chickens + 2 >= currentState.left.wolves || currentState.left.wolves == 0) {
                    // Add to the array
                    struct State new = currentState;
                    new.right.chickens -= 2;
                    new.left.chickens += 2;
                    new.right.boat = 0;
                    new.left.boat = 1;
                
                    successors[rear] = new;
                    rear++;
                    numSuccs++;
                }
            }
        }
        // Put one wolf in the boat
        if (currentState.right.wolves != 0) {
            if (currentState.left.chickens >= currentState.left.wolves + 1 || currentState.left.chickens == 0) {
                // Add to the array
                struct State new = currentState;
                new.right.wolves -= 1;
                new.left.wolves += 1;
                new.right.boat = 0;
                new.left.boat = 1;
                
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }
        // Put one wolf and one chicken in the boat
        if (currentState.right.chickens != 0 && currentState.right.wolves != 0) {
            if (!(currentState.left.wolves > currentState.left.chickens)) {
                // Add to the array
                struct State new = currentState;
                new.right.chickens -= 1;
                new.left.chickens += 1;
                new.right.wolves -= 1;
                new.left.wolves += 1;
                new.right.boat = 0;
                new.left.boat = 1;
                
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }
        // Put two wolves in the boat
        if (currentState.right.wolves >= 2) {
            if (currentState.left.chickens >= currentState.left.wolves + 2 || currentState.left.chickens == 0) {
                // Add to the array
                struct State new = currentState;
                new.right.wolves -= 2;
                new.left.wolves += 2;
                new.right.boat = 0;
                new.left.boat = 1;
                
                successors[rear] = new;
                rear++;
                numSuccs++;
            }
        }

        return numSuccs;
    }
}
