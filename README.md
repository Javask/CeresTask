# CeresTask
Simple library so I stop rewriting my tasking code
<hr>

## Notes

### TaskID 
intmax_t wrapper that cannot be set by hand </br>
Will create sequential IDs starting at zero each run </br>
ID exhaustion can occur, this needs to be fixed! </br>
### Tasks
Tasks are created by extending the Task virtual class.</br>
Tasks need to define a run function, which can return either true or false to indicate failure. </br>
Tasks can be named, if not they will have a random name for logging purposes. </br>
Each task has an id which is runtime unique to them.</br>
Tasks are arranged by grouping them in Task Groups
### Task Groups
Task Groups define a dependency graph between the tasks contained.</br>
Task Groups can be executed by submitting them to an executor</br>
A shared future to each task's result will be exposed </br>
A shared future to the task groups result will be exposed, representing a logical-and of all task results </br>

### Executors
An executor executes as many tasks of a given task group simultaneously as possible. </br>
Task groups will be executed sequentially if submitted to the same executor. </br>
The thread count of the executor can be defined on creation. </br>
On destruction the executor waits until all threads finish their current task and joins them. </br>
If a single task is submitted it will count as a task group containing only it.</br>

### Task System
The Task system defines a static interface to the library</br>
This interface exposes one static executor to which task groups can be submitted. </br>
