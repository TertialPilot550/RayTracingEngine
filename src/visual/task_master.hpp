#pragma once
#include "../lead.hpp"
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <thread>

/**
 * A generic thread pool implementation that take tasks
 * in the form of functions with no arguments and void
 * return type
 */

class TaskMaster {

    int thread_count;
    int chunk_size;
    int total_tasks;

    std::vector<std::thread> workers;
    std::vector<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::atomic<int> tasks_completed;
    

    public:

    TaskMaster(int thread_count = 1, int chunk_size = 1) {
        this->thread_count = thread_count;
        this->chunk_size = chunk_size;
    }

    void dispatch(const std::vector<std::function<void()>>& task_list) {

        std::cout << "Entered Dispath for Thread Pool\n";


        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks = std::move(task_list);
            tasks_completed = 0;
            total_tasks = tasks.size();
        }
        workers.clear();

        std::cout << "Tasks established\n";

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
                }


            });
        }

        std::cout << "Display progress\n";
        

        // Display progress
        double percent_complete = (tasks_completed * 1.0) / (total_tasks * 1.0);
        while (percent_complete < 1) {
            // Print with carriage return
            std::cout << "\rTask Progress: " << percent_complete << " ";

            int boxes = percent_complete * 10;
            for (int i = 0; i < boxes; i++) {
                std::cout << "■";
            }
            for (int i = 0; i < 10-boxes; i++) {
                std::cout << "☐";
            }

            // wait for a sec
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // update
            percent_complete = (tasks_completed * 1.0) / (total_tasks * 1.0);
        }

        std::cout << "Workers Created\n";

        // join the workers
        for (auto& worker : workers) {
            if (worker.joinable()) { worker.join(); };
        }

        std::cout << "Workers Joined\n";
    }

};
