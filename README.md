# Feedback Allocator

Implementation of a feedback allocator in C++, utilizing the MinCostMaxFlow algorithm provided by the Boost Graph Library (BGL).

## Overview

This program allocates feedback providers (trainers) to feedback recipients (participants) based on their preferences while considering any "no-go" relationships. It utilizes the MinCostMaxFlow algorithm to optimize the allocation process.

## Input

Input for the program can be provided in a file named `user_test.in`. The input format consists of several test cases, each containing the following:

- `t`: The number of test cases.
- For each test case:
  - `trainer_count`: The number of trainers.
  - `participant_count`: The number of participants.
  - `number_of_preferences`: The maximum number of preferences a participant can have.
  - `number_of_nogos`: The number of trainer-participant pairs with a strict "no-go" relationship.
  - For each trainer:
    - `trainer_name`: Name of the trainer.
    - `cap`: Maximum number of feedbacks the trainer can give.
  - For each participant:
    - `participant_name`: Name of the participant.
    - `preference_name`: Names of trainers, ordered by preference. Use "x" to indicate no preference.
  - For each "no-go" relationship:
    - `trainer_name`: Name of the trainer.
    - `participant_name`: Name of the participant.

Example input format:
```
1
3 3 2 1
Salz 3
Paprika 2
Chips 1 
Nutella Chips Paprika 
Honig Paprika Salz 
Coke Salz Chips 
Chips Nutella
```
## Setup

### Linux

Download the most recent versions of `g++`, `boost`, and `cgal` packages from your favorite package manager. On Ubuntu, for instance, run the following command:

```bash
sudo apt-get install build-essential libboost-all-dev cmake cmake-data
```

### Mac OS X / macOS
First, install Xcode via the App Store, which includes a recent version of the LLVM C++ compiler. To install Boost use either MacPorts or Homebrew.

### Compilation
To compile the code, use the provided Makefile:
```bash
make
```

## Execution
After compilation, execute the program and provide the input through the user_test.in file. Use the following command:

```bash
./out/feedback-allocator < public/user_test.in
```

## Output
The program outputs the assigned feedback providers for each participant. If an optimal solution is found, it prints "Allocation found!". Otherwise, it prints "XXX - NO SOLUTION!".