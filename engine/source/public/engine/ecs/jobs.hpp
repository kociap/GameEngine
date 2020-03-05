#ifndef ENGINE_ECS_JOBS_HPP_INCLUDE
#define ENGINE_ECS_JOBS_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
    class Job {
    public:
        virtual ~Job() {}

        virtual void execute() = 0;
    };

    i64 schedule_job(Job*);
} // namespace anton_engine
#endif // !ENGINE_ECS_JOBS_HPP_INCLUDE
