#ifndef ENGINE_ECS_SYSTEM_HPP_INCLUDE
#define ENGINE_ECS_SYSTEM_HPP_INCLUDE

namespace anton_engine {
    class System {
    public:
        virtual ~System() {}

        virtual void start() {}
        virtual void update() = 0;
    };
} // namespace anton_engine

#endif // !ENGINE_ECS_SYSTEM_HPP_INCLUDE
