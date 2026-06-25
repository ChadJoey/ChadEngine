#pragma once
#include <string>

namespace ChadEngine {

    class System {
    public:
        virtual ~System() = default;
        virtual void Update(float dt) {}
        virtual void Render() {}
        virtual void OnPlay() {}
        virtual void OnStop() {}

        int Priority = 0;
        std::string Title;
    };

} // namespace engine