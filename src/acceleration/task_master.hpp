#pragma once

#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <thread>

/**
 * @class
 * @brief Generic Thread Pool Implementation
 * @details A generic thread pool implementation that take tasks
 * in the form of functions with no arguments and void
 * return type
 */
class TaskMaster {

    int thread_count;
    int total_tasks;

    std::vector<std::thread> workers;
    std::vector<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::atomic<int> tasks_completed;
    

    public:

    /**
     * @brief The default construction uses a thread count of 1
     */
    TaskMaster(int thread_count = 1) {
        this->thread_count = thread_count;
    }

    /**
     * @brief Builds a thread pool, and executed the task list based on object parameters.
     * 
     * @param task_list: a list of impure functions to be run on worker threads
     * @details Uses a thread pool structure to parralelize an arbitrary list of impure functions
     * 
     * @note Since the functions are not required or assumed to be pure, remember to make sure they
     * are thread safe, i.e. that they do not modify any shared state.
     */
    void dispatch(std::vector<std::function<void()>>& task_list) {



        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks = std::move(task_list);
            tasks_completed = 0;
            total_tasks = tasks.size();
        }
        if (total_tasks == 0) {
            std::cout << "\rTask Progress: 100% (0/0)\n";
            return;
        }
        workers.clear();


        // Define n worker threads
        for (int i = 0; i < thread_count; i++) {
            workers.emplace_back([this]() {
                
                // While there are tasks remaining...
                while (true) {
                    std::function<void()> task = nullptr;

                    {
                        std::lock_guard<std::mutex> lock(queue_mutex);

                        if (tasks.empty())
                            return;

                        task = std::move(tasks.back());
                        tasks.pop_back();
                    }

                    task();
                    tasks_completed++;

        //             std::cout << "START " << i
        //           << " thread " << std::this_thread::get_id()
        //           << '\n';

        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // std::cout << "END " << i
        //           << " thread " << std::this_thread::get_id()
        //           << '\n';
                }
                

            });
        }

        

        // Display progress
        if (total_tasks == 0) total_tasks = 1;
        double percent_complete = (tasks_completed * 1.0) / (total_tasks * 1.0);
        while (percent_complete < 1) {
            // Print with carriage return and flush so the progress bar is visible live.
            std::cout << "\rTask Progress: " << percent_complete << " (" << tasks_completed << ")";
            int boxes = static_cast<int>(percent_complete * 10.0);
            for (int i = 0; i < boxes; i++) {
                std::cout << "■";
            }
            for (int i = 0; i < 10-boxes; i++) {
                std::cout << "☐";
            }
            std::cout << std::flush;

            // wait for a sec
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // update
            percent_complete = (tasks_completed * 1.0) / (total_tasks * 1.0);
        }
        std::cout << "\n" << std::flush;


        // join the workers
        for (auto& worker : workers) {
            if (worker.joinable()) { worker.join(); };
        }

    }

};
