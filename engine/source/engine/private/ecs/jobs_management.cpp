#include <ecs/jobs.hpp>
#include <ecs/jobs_management.hpp>

#include <anton_stl/vector.hpp>

namespace anton_engine {
    static anton_stl::Vector<Job*> jobs;

    i64 schedule_job(Job* job) {
        jobs.push_back(job);
    }

    void execute_jobs() {
        for (Job* job: jobs) {
            job->execute();
        }

        jobs.clear();
    }
} // namespace anton_engine
