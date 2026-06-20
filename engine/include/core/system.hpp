#pragma once

namespace ChadEngine {

    class System {
    public:
        const char* Title = "Unnamed";
        virtual ~System() = default;

        virtual void Update(float dt) {}
        virtual void Render() {}
        virtual void OnPlay() {}
        virtual void OnStop() {}
    };

} // namespace engine