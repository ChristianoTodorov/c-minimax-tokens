# Minimax Token Game Solver (C)

Optimal two-player zero-sum game solver written in pure C, implementing the **Minimax** decision algorithm with **Alpha-Beta pruning** to compute optimal moves over a sequence of weighted tokens.

## Game Rules & State Space
Two players (`A` and `B`) alternate turns picking tokens from either end of a linear sequence (`head` and `tail` pointers):
- **Single-token moves:** Take 1 token from the left (`head`) or 1 token from the right (`tail`).
- **Double-token moves (Constrained):** Take 2 tokens from the left, 2 from the right, or 1 from each end. A player cannot execute a double-token move on two consecutive turns (`takenTwo` state flag).
- **Objective:** Maximize the score differential (`playerScore - opponentScore`) assuming optimal play from both sides.

## Technical Highlights
- **Algorithm:** Depth-first game tree search using Minimax with Alpha-Beta pruning (`INT_MIN` / `INT_MAX` bounds) to prune suboptimal branches.
- **Zero-Allocation Tree Traversal:** Game board and player states (`GameBoardT`, `PlayerT`) are passed and copied by value on the call stack during recursion, avoiding dynamic allocation overhead inside search nodes.
- **Input Validation:** Safe line-buffered parsing (`fgets` + `strtol`) checking integer bounds (`INT_MIN`–`INT_MAX`), trailing characters, and maximum token capacity.

## Build & Run

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic main.cpp -o tokens_solver
./tokens_solver
