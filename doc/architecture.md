# JeroRobot Core Architecture (Task / Node / Action)

> Version: 0.2  
> Last updated: TODO

---

## 1. Project Goal

JeroRobot Core is a C++ framework for robot task control.

The main goals are:

- Receive high-level commands from **TCP / HTTP** interfaces.
- Convert external commands into a unified **Task → Node → Action** execution model.
- Manage task lifecycle, state, and error handling in a safe and predictable way.
- Integrate with the concrete robot platform, including chassis, actuators, sensors, IO, etc.

In one sentence:

> The upper system sends a *Task*; the Core decomposes it into concrete actions and drives the robot to execute it safely and controllably.

---

## 2. Core Concepts

### 2.1 Task

A **Task** represents one complete mission, for example:

- Clean a specific area.
- Pick goods at point A and deliver them to point B.
- Go to the docking station and start charging.

A Task is composed of **an ordered list of Nodes**, which are executed sequentially.

#### 2.1.1 Global constraints

- At any time, **only one Task can be running** in the system  
  (single active task model).
- The Task is considered **successful** only if **all Nodes complete successfully**.
- The Task is considered **failed** if:
  - Any Node ends in a failure state (by default), or
  - The Task is explicitly canceled/aborted by an external command, or
  - A global timeout / fatal internal error occurs.

> Note: the exact mapping between Node results and Task result can be customized later (e.g., allow some Nodes to be “skipped without failing the Task”).

#### 2.1.2 Suggested Task fields (draft)

A Task definition typically contains (not final, to be refined during implementation):

- `id` (string)  
  - Unique identifier of the Task.
- `description` (string)  
  - Human-readable description for logging and debugging.
- `priority` (integer / enum)  
  - Future extension: used for scheduling if multiple task requests arrive.
- `timeout_ms` (integer)  
  - Maximum allowed execution time for the whole Task.
- `nodes` (array of Node definitions)  
  - The ordered list of Nodes which compose this Task.
- `params` (key/value map or JSON)  
  - Dynamic parameters, e.g. target station ID, order ID, etc.

The concrete structure is represented in code by something like `TaskDef`.

---

### 2.2 Node

A **Node** is a phase/stage inside a Task. Examples:

- “Move to pickup position”.
- “Load cargo”.
- “Move to drop-off position”.
- “Return to charger”.

Nodes define the **high-level sequence** of a Task.

#### 2.2.1 Execution constraints

- Nodes in a Task are executed **strictly in sequence**.
- At any given time, **at most one Node is in the `Running` state**.
- A Node only starts after the previous Node has finished (either success or failure).

#### 2.2.2 Node completion

A Node contains a set of **Actions**. Within one Node:

- All its Actions are started according to the Node’s internal policy  
  (current version: all Actions start in parallel).
- The Node is considered **finished** when *all* Actions have reached a terminal state  
  (`Success / Failed / Timeout / Canceled`).
- Node result:
  - If **any Action** is `Failed / Timeout / Canceled`, the Node result becomes `Failed`.
  - If **all Actions** are `Success`, the Node result becomes `Success`.

This default policy can be extended later (e.g., some Actions marked as “optional”).

#### 2.2.3 Node behavior extensions (future work)

The following behaviors are planned but not yet finalized:

- **Retry policy**  
  - Each Node can optionally specify:
    - `max_retry_count`
    - `retry_interval_ms`
  - When a Node fails, the Task engine may automatically retry the Node according to its policy before marking the whole Task as failed.

- **Skip / conditional execution**  
  - Nodes may have a condition expression based on:
    - Task parameters,
    - Robot state,
    - External environment.
  - If the condition is not satisfied, the Node can be skipped without failing the Task.

- **Branching / sub-flows**  
  - In the future, Nodes may be allowed to jump to different next Nodes based on conditions, forming a DAG or simple flow graph instead of a pure linear list.

These advanced behaviors will be documented once the implementation is stable.

---

### 2.3 Action

An **Action** is the smallest execution unit (atomic operation). Examples:

- **MoveToAction**: navigate the chassis to a target pose / station.
- **LiftToAction**: move a lift/arm to a specific height or joint angle.
- **WaitSensorAction**: wait until a certain sensor reaches the expected state.
- **IOControlAction**: set a digital IO on/off, or write to an analog output.
- **SleepAction**: simple time delay.

Actions are where concrete robot behaviors happen. A Node combines one or more Actions to form a meaningful stage.

#### 2.3.1 Parallelism inside a Node

- A Node may contain multiple Actions.
- All Actions in a Node **can be executed in parallel**, for example:
  - Move the chassis while adjusting a camera gimbal.
  - Move and read sensor data simultaneously.
- The actual parallelism is determined by the Action implementation:
  - some Actions might run synchronously in the tick loop,
  - some Actions might spawn their own threads or use an internal thread pool.

#### 2.3.2 Action state machine

Each Action has its own internal state machine, with the following basic states:

- `Idle`  
  - Initial state, Action has not started yet.
- `Running`  
  - Action logic is actively executing.
- `Success`  
  - Action completed as expected, reaching its goal.
- `Failed`  
  - Action completed with an error that should be treated as failure.
- `Timeout`  
  - Action took longer than allowed; treated as a form of failure.
- `Canceled`  
  - Action was explicitly canceled (e.g., Task was aborted).

A simplified lifecycle is:

```text
Idle → Running → (Success | Failed | Timeout | Canceled)
