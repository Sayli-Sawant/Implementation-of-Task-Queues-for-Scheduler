
# TASK SCHEDULER

This program simulates a task scheduler that manages tasks in different states, including running, ready, and waiting states. 
Tasks are represented by a structure with attributes like ID, priority, wait ID, and task state. 
The program utilizes queues to organize tasks in the ready and wait states, along with a running task.

## Commands

- **n**: Create a new task. Enter Task ID, Priority, and Wait ID.
- **d**: Delete a task. Enter the Task ID to delete.
- **w**: Move a task from the ready to the wait state. Enter Task ID and wait ID.
- **s**: Suspend the running task and move it to the waiting state. Enter wait ID.
- **e**: Trigger an event to move tasks from the wait to ready state. Enter Wait ID.
- **x**: Exit the program.

## File Input

Tasks are loaded from a text file named **"task_scheduler.txt"** during program initialization. The file format is as follows:

\`\`\`
taskid;priority;state;waitid
121;2;2;124
122;1;2;121
\`\`\`

## Program Details

- The program uses a simple command-line interface to interact with the user.
- Tasks are displayed in ready and wait queues, along with the running task, after each command.
- Duplicate Task IDs are checked during task creation.
- Default tasks are not allowed to be moved or deleted.
- The ready queue is sorted by priority.
