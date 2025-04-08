# 🔢 Prime Number Search in Matrix Using Pthreads

This project demonstrates the use of **parallel processing with threads** using the **Pthreads** library. It focuses on comparing the performance between **serial** and **multithreaded** execution for finding prime numbers in a large matrix.

## 🧠 Description

- Generates a **10,000 x 10,000** matrix filled with random integers between 0 and 31,999.
- The matrix is divided into **10x10 macroblocks** to allow for distributed work among threads.
- Each thread searches for **prime numbers** within the macroblocks. A `mutex` is used to ensure that only one thread processes a block at a time.
- At the end, the program prints the execution time of the serial and multithreaded versions, as well as the total number of primes found.

## 🧾 Features

- 💡 `isPrime(n)` function to check if a number is prime.
- 🧮 **Serial execution**: one thread traverses the entire matrix.
- 🚀 **Parallel execution**: threads divide and conquer macroblocks concurrently (6 threads by default).
- 🔐 Use of `pthread_mutex` to ensure safe thread synchronization.
- 🧼 Matrix memory is freed at the end of execution.

## 📁 Files

- `program.c`: main implementation containing both serial and parallel logic.
- `pthreadVC2.lib`: library required for compilation with pthreads on Windows.
- `.vcxproj` and `.sln`: project files for opening with Visual Studio.

## 🛠️ Compilation and Execution

### ✅ Linux

```sh
gcc program.c -o program -lpthread -lm
./program
```
### 🪟 Windows (Visual Studio)

1. Open the `.sln` file.

2. Build and run using Visual Studio.

3. Make sure `pthreadVC2.lib` is properly linked in the project settings.
   
## 🧪 Experiments You Can Try

- Change `QTD_THREADS` to experiment with different levels of parallelism.
- Increase or decrease the matrix size to test performance scalability.
- Modify `MACRO_LINHA` and `MACRO_COLUNA` to change how the matrix is partitioned.

## 💡 Key Concepts

- **Multithreading with pthreads**: Threads are used to concurrently process macroblocks in the matrix.
- **Synchronization**: `pthread_mutex_t` is used to avoid race conditions when updating shared state.
- **Cache Locality Optimization**: The matrix is processed in macroblocks to take advantage of CPU cache behavior.
- **Prime Number Detection**: The function `ehPrimo` checks whether each element in the matrix is a prime.
- **Performance Comparison**: The execution time for serial and parallel approaches is printed for comparison.
---
🧑‍💻 Project developed for the Operating Systems course – 2024/2.
