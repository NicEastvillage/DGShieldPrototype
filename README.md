# DGShieldPrototype

This is a prototype of dependency-graph-based algorithm incorporating partition refinement to compute shields.
The algorithm can be found in `src/ShieldGeneratorDG.cpp`.

### Model

The prototype uses a hardcoded discrete model where an agent have to move to the right of the screen without entering any unsafe areas.
In safe areas the agent has three actions: up, down, or right, but its movement imperfect.

- An up action will move the agent one step up and one step right, `pos += (1, 1)`.
- A right action will move the agent one or two steps right, `pos += (1, 0) or (2, 0)`.
- A down action will move the agent one step right and either one or two steps down, `pos += (1, -1) or (1, -2)`.

The model is defined in `src/Model.cpp`.

The algorithm can be easily generalized to continuous models if a maximum refinement (minimum partition size) is defined. 

### Controls

- `LMB (left click)`: Change initial state
- `RMB`: Add/remove walls
- `UP`/`RIGHT`/`DOWN`: Take an up/right/down action
- `P`: Simulate trace (respecting shield if computed)
- `O` (hold): Take random action (respecting shield if computed)
- `I`: Reset trace
- `E`: Compute shield
- `Q`: Remove shield
- `S`: Change color of shield
  - Default: Red = all actions are unsafe; green = all outgoing traces are completely safe; yellow = explored but not completely safe; No color = unexplored.
  - Rainbow: Red channel = right action is safe; Blue channel = down action is safe; Green channel = up action is safe. No color = unexplored. (i.e. yellow implies both right and up actions are safe)

### Assumptions

The performance of the algorithm heavily relies on the following assumptions:

- It fast to check if a partition contains a unsafe state.
  - Partitions are refined or concluded unsafe whenever this is the case.
- Given a partition with any granularity, it is fast to compute all successor partitions of the finest granularity.
  - The successor partitions must be of finest granularity for the following reason: Whenever a partition is split, incoming edges must be reassigned to the new child partitions. This is easily done if the edge targets are defined with partitions of highest granularity - a simple inclusion test against the children partitions. (All alternatives involve recomputing the successors of all source configurations of all incoming edges)
  - Successors that fall in the source partition can be omitted for better performance as such successors result in useless self-loops in the dependency graph.
