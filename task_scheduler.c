#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

// Structure to represent a task
struct Task {
    int id;
    //char name[50];
    int priority;
    int wait_id;
    int task_state; // 0 for running, 1 for ready, 2 for wait
    struct Task* next;
};

// Structure to represent a queue
struct Queue {
    struct Task* front;
    struct Task* rear;
};

// Structure to represent a running task
struct RunningTask {
    struct Task* task;
};

// Function to initialize a queue
void initQueue(struct Queue* queue) {
    queue->front = queue->rear = NULL;
}

// Function to check if the queue is empty
int isQueueEmpty(struct Queue* queue) {
    return (queue->front == NULL);
}

// Function to enqueue a task into the queue
void enqueue(struct Queue* queue, struct Task* task) {
    if (isQueueEmpty(queue)) {
        queue->front = queue->rear = task;
    } else {
        queue->rear->next = task;
        queue->rear = task;
    }
    task->next = NULL;
}

// Function to enqueue a task into the ready queue while keeping it sorted by priority
void enqueueSortedByPriority(struct Queue* queue, struct Task* task) {
    struct Task* current = queue->front;
    struct Task* prev = NULL;

    // Find the correct position to insert the task based on priority
    while (current != NULL && current->priority <= task->priority) {
        prev = current;
        current = current->next;
    }

    // If the task is at the front
    if (prev == NULL) {
        task->next = queue->front;
        queue->front = task;
    } else {
        task->next = current;
        prev->next = task;
    }

    // If the task is at the rear
    if (current == NULL) {
        queue->rear = task;
    }
}

// Function to create a new task
struct Task* createTask(int id, int priority, int wait_id, int task_state) {
    struct Task* newTask = (struct Task*)malloc(sizeof(struct Task));
    newTask->id = id;
    newTask->priority = priority;
    newTask->wait_id = wait_id;
    newTask->task_state = task_state;
    newTask->next = NULL;

    return newTask;
}

// Function to display tasks in the queue
void displayQueue(struct Queue* queue, const char* queueName) {
    printf("\n%s Queue:\n", queueName);
    printf("ID\tPriority\tWait ID\t\tState\n");
    printf("---------------------------------------------------------\n");

    struct Task* current = queue->front;
    while (current != NULL) {
        printf("%d\t%d\t\t%d\t\t%s\n", current->id, current->priority,
               current->wait_id, (current->task_state == 0) ? "Running" : 
               (current->task_state == 1) ? "Ready" : "Wait");
        current = current->next;
    }
    printf("\n");
}


// Function to delete a task with a specific event_id from the queue
void deleteTask(struct Queue* queue, int event_id) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot delete.\n");
        return;
    }

    struct Task* current = queue->front;
    struct Task* prev = NULL;

    // Search for the task with the specified event_id
    while (current != NULL && current->id != event_id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    // If the task is at the front
    if (prev == NULL) {
        queue->front = current->next;
    } else {
        prev->next = current->next;
    }

    // If the task is at the rear
    if (current == queue->rear) {
        queue->rear = prev;
    }

    free(current);
    //printf("Task with event_id %d deleted from the queue.\n", event_id);
}

// Function to free the memory allocated for the task and queue
void destroyTaskAndQueue(struct Queue* queue) {
    while (queue->front != NULL) {
        struct Task* temp = queue->front;
        queue->front = temp->next;
        free(temp);
    }
    free(queue);
}

// Function to move a task from running to waiting state based on event_id
void moveRunningToWaiting(struct RunningTask* runningTask, struct Queue* waitQueue, int event_id) {
    if (runningTask->task == NULL) {
        printf("No task is currently in running state.\n");
        return;
    }

    // Move the task to waiting state
    runningTask->task->task_state = 2;
    runningTask->task->wait_id = event_id;
    enqueue(waitQueue, runningTask->task);
    runningTask->task = NULL;

    printf("Task with event_id %d moved from running to waiting state.\n", event_id);
}

// Function to move the first task in ready to running state
void moveFirstReadyToRunning(struct Queue* readyQueue, struct RunningTask* runningTask) {
    if (isQueueEmpty(readyQueue)) {
        printf("No tasks in the ready queue.\n");
        return;
    }
    struct Task* firstReadyTask = readyQueue->front;

    // Move the first task to running state
    runningTask->task = NULL;
    struct Task* newTask = createTask(readyQueue->front->id, readyQueue->front->priority, -1, 0);
    runningTask->task = newTask;
    //firstReadyTask->task_state = 0;


    // Remove the task from the ready queue
    deleteTask(readyQueue, firstReadyTask->id);

    printf("First task in the ready queue moved to running state.\n");
}

// Function to move all tasks with the specified wait_id from ready to waiting state
void moveReadyToWaiting(struct Queue* waitQueue, struct Queue* readyQueue, int id, int event_id){
    if (isQueueEmpty(readyQueue)) {
        printf("No tasks in the ready queue.\n");
        return;
    }

    struct Task* current = readyQueue->front;
    struct Task* prev = NULL;

    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Task with task_id %d not found in the queue.\n", id);
        return;
    }

    if (prev == NULL) {
        readyQueue->front = current->next;
    } else {
        prev->next = current->next;
    }

    // If the task is at the rear
    if (current == readyQueue->rear) {
        readyQueue->rear = prev;
    }

    // Move the task to waiting state
    struct Task* newTask = createTask(id, current->priority, event_id, 3);
    enqueue(waitQueue, newTask);

    free(current);
}

// Function to move all tasks with the specified wait_id from wait to ready state
void moveWaitToReady(struct Queue* waitQueue, struct Queue* readyQueue, int wait_id) {
    struct Task* current = waitQueue->front;
    struct Task* prev = NULL;

    while (current != NULL) {
        if (current->wait_id == wait_id) {
            // Move the task from wait to ready state
            current->task_state = 1;

            // Enqueue the task in the ready queue
            struct Task* newTask = createTask(current->id, current->priority, -1, 1);
            //enqueue(readyQueue, newTask);
            enqueueSortedByPriority(readyQueue, newTask);

            // Move the task to waiting state
            //

            // Remove the task from the wait queue
            if (prev == NULL) {
                waitQueue->front = current->next;
            } else {
                prev->next = current->next;
            }

            // If the task is at the rear
            if (current == waitQueue->rear) {
                waitQueue->rear = prev;
            }

            struct Task* temp = current;
            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }

    printf("All tasks with wait_id %d moved from wait to ready state.\n", wait_id);
}

void checkRunningTask(struct RunningTask* runningTask, struct Queue* readyQueue){
    int p1=runningTask->task->priority;
    int p2=readyQueue->front->priority;
        if((runningTask->task->priority) > (readyQueue->front->priority)){
            runningTask->task->task_state = 1; 
            struct Task* newTask = runningTask->task;
            //createTask(runningTask->task->id, runningTask->task->priority, runningTask->task->name, runningTask->task->wait_id, 1);
            enqueueSortedByPriority(readyQueue, newTask);
            int id1=newTask->id;
            moveFirstReadyToRunning(readyQueue, runningTask);
        }
}

//function to check duplicate task id
int checkDuplicateId(struct RunningTask* runningTAsk, struct Queue* readyQueue, struct Queue* waitQueue, int task_id)
{
    //check in running list
    if((runningTAsk->task->id)==task_id){
        return 1;
    }
    else{
        //check in ready queue
        struct Task* current = readyQueue->front;
        struct Task* prev = NULL;
        while(current!=NULL){
            if((current->id)==task_id){
                return 1;
            }
            prev=current;
            current=current->next;
        }
    }

    //check in wait queue
    struct Task* current = waitQueue->front;
    struct Task* prev = NULL;
    while(current!=NULL){
        if((current->id)==task_id){
            return 1;
        }
        prev=current;
        current=current->next;
    }
    return 0;

}

int main() {
    struct Queue* readyQueue = (struct Queue*)malloc(sizeof(struct Queue));
    struct Queue* waitQueue = (struct Queue*)malloc(sizeof(struct Queue));
    struct RunningTask* runningTask = (struct RunningTask*)malloc(sizeof(struct RunningTask));
    initQueue(readyQueue);
    initQueue(waitQueue);
    runningTask->task = NULL;

    char command;
    int event_id, id, priority, wait_id, task_state;
    char name[50];

    //load tasks from text file
    FILE *fptr;
    fptr = fopen("task_scheduler.txt", "r");
    char myString[100];
    //fgets(myString, 100, fptr);
    int count=0;
    while(fgets(myString, MAX_LINE_LENGTH, fptr)){
        char *line = strtok(myString, ";");
        int t[4];
        int i=0;
        while(line != NULL)
        {
            
            t[i]=atoi(line);
            line = strtok(NULL, ";");
            i++;
        }
        //skip first line in file
        if(count !=0){
            struct Task* newTask = createTask(t[0], t[1], t[3], t[2]);
            if (t[2] == 0) {
                runningTask->task = newTask;
            } else if (t[2] == 1) {
                enqueueSortedByPriority(readyQueue, newTask);
            } else if (t[2] == 2) {
                enqueue(waitQueue, newTask);
            }
        }
        count++;
    }
    fclose(fptr);

    displayQueue(readyQueue, "Ready");
    displayQueue(waitQueue, "Wait");

    if (runningTask->task != NULL) {
        printf("\nRunning Task:\n");
        printf("ID\tPriority\tWait ID\t\tState\n");
        printf("---------------------------------------------------------\n");
        printf("%d\t%d\t\t%d\t\tRunning\n", runningTask->task->id,
                runningTask->task->priority,
                runningTask->task->wait_id);
        printf("\n");
    }

    do {
        printf("\nEnter command (n for new task, d for delete task, w to move from ready to wait state, s to suspend, e to trigger an event, x to exit): ");
        scanf(" %c", &command);
        //getchar();
        //printf("%s", command);

        switch (command) {
            case 'n':
                int check;
                //printf("Enter Task ID: ");
                scanf("%d", &id);
                check=checkDuplicateId(runningTask, readyQueue, waitQueue, id);
                if(check==1)
                {
                    printf("Duplicate id \n");
                    break;
                }
                /*while(check!=0){
                    printf("Duplicate id \n");
                    printf("Enter Task ID: ");
                    scanf("%d", &id);
                    check=checkDuplicateId(runningTask, readyQueue, waitQueue, id);
                }
                */
                scanf("%d", &priority);

                scanf("%d", &wait_id);

                task_state = 2;

                struct Task* newTask = createTask(id, priority, wait_id, task_state);
                enqueue(waitQueue, newTask);
                /*if (task_state == 0) {

                    runningTask->task = newTask;
                } else */
                /*if (task_state == 1) {
                    enqueueSortedByPriority(readyQueue, newTask);
                } else if (task_state == 2) {
                    enqueue(waitQueue, newTask);
                }*/
                break;

            case 'd':
                //printf("Enter Task ID to delete: ");
                scanf("%d", &event_id);
                deleteTask(readyQueue, event_id);
                deleteTask(waitQueue, event_id);
                if ((runningTask->task != NULL && runningTask->task->id == event_id) || runningTask->task->id == 0) {
                    //free(runningTask->task);
                    //moveFirstReadyToRunning(readyQueue, runningTask);
                    //printf("Task with event_id %d deleted from running state.\n", event_id);
                    printf("Cannot delete running task");
                }
                break;
            
            case 's':
                //printf("Enter Twait id to move from running to waiting state: ");
                scanf("%d", &event_id);
                if(runningTask->task->id == 0){
                    printf("cannot move default task");
                    break;
                }
                moveRunningToWaiting(runningTask, waitQueue, event_id);
                moveFirstReadyToRunning(readyQueue, runningTask);
                break;

            case 'w':
                //printf("Enter Task ID to move from ready to waiting state: ");
                scanf("%d", &id);
                //printf("Enter event id: ");
                scanf("%d", &event_id);
                if(id == 0)
                {
                    printf("cannot move default task");
                    break;
                }
                moveReadyToWaiting(waitQueue, readyQueue, id, event_id);
                break;

            case 'e':
                printf("Enter wait_id to move tasks from wait to ready state: ");
                scanf("%d", &wait_id);
                moveWaitToReady(waitQueue, readyQueue, wait_id);
                checkRunningTask(runningTask, readyQueue);
                break;

            case 'x':
                break;

            default:
                printf("Invalid command. Please enter n, d, or x.\n");
                break;
        }

        // Display the ready and wait queues

        displayQueue(readyQueue, "Ready");
        displayQueue(waitQueue, "Wait");

        if (runningTask->task != NULL) {
            printf("\nRunning Task:\n");
            printf("ID\tPriority\tWait ID\t\tState\n");
            printf("---------------------------------------------------------\n");
            printf("%d\t%d\t\t%d\t\tRunning\n", runningTask->task->id,
                   runningTask->task->priority,
                   runningTask->task->wait_id);
            printf("\n");
        }

    } while (command != 'x');

    // Free the memory allocated for tasks and queues
    destroyTaskAndQueue(readyQueue);
    destroyTaskAndQueue(waitQueue);

    return 0;
}
