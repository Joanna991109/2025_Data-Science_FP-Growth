# 2025 Data Science HW1 - Frequent Pattern Mining

## Overview
- This project implements a frequent pattern mining algorithm in C++ to discover frequent itemsets from transactional databases based on a given minimum support threshold.
- The program utilizes the FP-Growth (Frequent Pattern Growth) algorithm and is built entirely from scratch without the use of any external frequent pattern mining libraries. 

## Implementation Details
The algorithm efficiently mines frequent patterns by compressing the transaction database into a memory-efficient data structure and dividing the search space. The C++ implementation (`114065514_hw1.cpp`) primarily consists of:
- **Node Structure**: Represents an item in the FP-Tree, storing its item ID, frequency count, and pointers to its parent and children.
- **Tree Structure**: Manages the root of the FP-Tree, the header table, and tracks item frequencies. It includes functions to build conditional FP-Trees and recursively mine frequent itemsets (`FP_Growth`).

## Compilation
- To compile the source code, use `g++` with the `c++2a` standard. Execute the following command in your terminal:
```bash
g++ -std=c++2a -pthread -fopenmp -O2 -o 114065514_hw1 114065514_hw1.cpp
```

## Usage 
- Run the compiled executable by providing exactly three arguments in the following order:
```bash
./114065514_hw1 [min_support] [input_file] [output_file]
```
- **[min_support]**: A floating-point value representing the frequency threshold (e.g., `0.2`).
- **[input_file]**: The path to the text file containing the input transactions.
- **[output_file]**: The path to the text file where the resulting patterns will be saved.

**Example:**
```bash
./114065514_hw1 0.2 input1.txt output1.txt
```

## Data Formats
- **Input Format**: A text file where each line represents a single transaction. Items within a transaction are integers ranging from 0 to 999 and are separated by commas (`,`) without any spaces. Newlines are formatted using `\n` (LF).
- **Output Format**: A text file where each line displays a frequent pattern followed by a colon (`:`) and its support rate. The support value is strictly rounded to 4 decimal places. 
    - *Example Output Line*: `1,2,3:0.2500`
