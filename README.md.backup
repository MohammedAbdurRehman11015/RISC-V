# README

**AI23BTECH11013, AI23BTECH11015**

## 1 Introduction

The Lab7 assignment allows you to execute RISC-V instructions, manage register values, perform memory addressing, implement stack operations, and simulate cache behavior.

## 2 Contents of this Folder

The compressed folder `Lab7_AI23BTECH11013_AI23BTECH11015.zip` contains the following files:

1. **Main.c**: Contains the C code for executing functions and providing the interface for commands such as load, run, cache_sim status, etc.

2. **lab7_AI_13_15.c**: Contains the C code for all the functions defined in lab7_AI_13_15.h, used to implement RISC-V instructions and cache simulation.

3. **lab7_AI_13_15.h**: Defines all functions used in lab7_AI_13_15.c.

4. **Makefile**: A Makefile that compiles the C code and generates an executable named riscv_sim.

5. **README**: Provides information about the folder and usage instructions.

6. **report.pdf**: Contains a summary of the project and describes the approach used for implementation.

## 3 Deployment

To deploy this project, follow the steps below:

1. Download the `Lab7_AI23BTECH11013_AI23BTECH11015.zip` file.

2. Extract the contents into a directory of your choice.

3. Prepare an input file with proper RISC-V instructions, ensuring correct syntax.

4. Open a terminal and navigate to the extracted folder:
   ```bash
   cd Lab7_AI23BTECH11013_AI23BTECH11015
   ```

5. Compile the code using the Makefile:
   ```bash
   make
   ```

6. Run the executable:
   ```bash
   ./riscv_sim
   ```

7. Load the input file:
   ```bash
   load <filename>
   ```
   (Replace `<filename>` with the name of your input file.)

8. Execute the full code:
   ```bash
   run
   ```

9. View the register values:
   ```bash
   regs
   ```

10. View memory and stored values:
    ```bash
    mem <address> <count>
    ```

11. Execute one instruction step-by-step:
    ```bash
    step
    ```

12. Display the current stack:
    ```bash
    show-stack
    ```

13. Add a breakpoint:
    ```bash
    break <line>
    ```

14. Delete an existing breakpoint:
    ```bash
    del break <line>
    ```

15. Enable cache and provide config.txt containing cache attributes:
    ```bash
    cache_sim enable config.txt
    ```

16. Disable cache:
    ```bash
    cache_sim disable
    ```

17. View cache status and attributes:
    ```bash
    cache_sim status
    ```

18. Invalidate all entries in the cache:
    ```bash
    cache_sim invalidate
    ```

19. Display all entries in the cache:
    ```bash
    cache_sim dump filename.txt
    ```

20. Print cache statistics for the executing code:
    ```bash
    cache_sim stats
    ```