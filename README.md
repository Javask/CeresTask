# CeresTask
Simple library so I stop rewriting my tasking code
<hr>


## CMake options
To disable automatic build of test executable use CMake option CERESTASK_BUILD_TESTS.

##Usage

###Task
To create a task inherit from the Task-class. 
The custom task has to define a run function. 
The run function has to accept a boolean and return a boolean. 
The return value signifies if the task failed in execution. 
A value of false means failure, true success.
The boolean argument accepted by the function signifies 
whether or not a direct predecessor of this task failed execution.
A value of true meaning at least one predecessor failed.

###TaskGroup
To organize tasks they have to be added to a TaskGroup, which contains the dependencies between tasks.
A dependency can be added via addWaitOnEdge.
Such a group can then be submitted to an Executor.

###Executor
The executor contains the threads that execute the tasks.
It will try to execute as many tasks in parallel as possible, as long as the dependencies allow it.
The number of threads the Executor should spawn can be set in the constructor.
On destruction it will join all created threads, waiting on task completion for all of them.


