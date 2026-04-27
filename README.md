# Snake 2.0 - System-Level Implementation in C

A real-time, terminal-based Snake game developed entirely in C. This project demonstrates deep system-level programming by explicitly minimizing dependence on the C Standard Library, implementing custom mathematical operations, string manipulations, and memory management subsystems from scratch.

## Features

- **Custom Memory Management:** Implements a First-Fit Heap Allocator over a statically allocated 64KB Virtual RAM pool (`my_alloc`, `my_dealloc`).
- **Raw Terminal I/O:** Utilizes POSIX `termios` for non-blocking raw input, eliminating the need to press Enter for commands.
- **Tear-Free Rendering:** Leverages ANSI escape sequences and the Alternate Screen Buffer (`ESC[?1049h`) to prevent terminal scrolling and frame stacking.
- **Three Difficulty Modes:**
  - **Easy:** Wrap-around borders (no wall collision).
  - **Medium:** Standard boundary collision.
  - **Hard:** Introduces an internal maze structure with increased speed.
- **Dynamic Speed Normalization:** Automatically adjusts horizontal/vertical tick rates to compensate for terminal font aspect ratios (making vertical and horizontal movement visually consistent).
- **Persistent High Scores:** Local file I/O to maintain and display top 3 leaderboards for each difficulty mode.

## Architecture

The project is heavily modularized, mimicking a bare-metal implementation:
- `game.c/h`: Core state machine, collision logic, maze layouts.
- `screen.c/h`: ANSI color definitions, coordinate rendering, alternate buffer switching.
- `scores.c/h`: Persistent disk I/O and top-K sorting.
- `memory.c/h`: Custom heap allocation.
- `math.c/h` & `string.c/h`: Custom fundamental math/string operations.
- `keyboard.c/h`: POSIX input control.

## Compilation & Execution

This project requires a POSIX-compliant system (macOS/Linux). No external dependencies or libraries are required.

```bash
# Compile the project
make

# Run the game
./snake
```

## Controls
- **W, A, S, D** or **Arrow Keys**: Move the snake
- **Enter**: Confirm selections in menus
- **Q**: Quit game / Return to menu
- **R**: Restart game (from game over screen)

## Academic Report
An IEEE-formatted project report detailing the methodology, architecture, and optimizations (File Systems, Security, Process Management, I/O Management, and Memory Management) is included as `Report.pdf`.
