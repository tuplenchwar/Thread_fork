#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <atomic>
#include <sys/wait.h>
#include <limits>

void createChildProcesses(std::atomic<int>* atomic_process_cntr, int child_cnt)
{
    pid_t child_pids[child_cnt];
    int i, j;

    for (i = 0; i < child_cnt; ++i) {
        atomic_process_cntr->fetch_sub(1, std::memory_order_relaxed);

        if (atomic_process_cntr->load(std::memory_order_relaxed) >= 0) {
            pid_t childPid = fork();

            if (childPid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (childPid == 0) {
                sleep(3);
                createChildProcesses(atomic_process_cntr, child_cnt);
                exit(EXIT_SUCCESS);
            } else {
                // Parent process
                std::cout << "Child " << childPid << " created by Parent: " << getpid()  << std::endl;
                child_pids[i] = childPid;
            }
        } else {
            exit(EXIT_SUCCESS);
        }
    }

    // Move the wait function outside the loop
    for (j = 0; j < i; j++) {
        waitpid(child_pids[j], NULL, 0); // Wait for each child to finish
    }

    return;
}

// Validation function
int getInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) {
            break;
        } else {
            std::cerr << "Invalid input. Please enter a non-negative integer." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return value;
}

int main(void)
{
    // Size of the shared atomic variable
    size_t size = sizeof(std::atomic<int>);
    int child_cnt;
    int process_cnt;

    // Create a shared memory region
    std::atomic<int>* atomic_process_cntr = static_cast<std::atomic<int>*>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    if (atomic_process_cntr == MAP_FAILED) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }

    // take inputs
    process_cnt = getInput("Enter number of processes: ");

    // Initialize the shared atomic variable
    atomic_process_cntr->store(process_cnt, std::memory_order_relaxed);

    child_cnt = getInput("Enter the number of child processes to be created: ");


    createChildProcesses(atomic_process_cntr, child_cnt);

    // Cleanup shared memory region
    if (munmap(atomic_process_cntr, size) == -1) {
        perror("Error unmapping shared memory");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
