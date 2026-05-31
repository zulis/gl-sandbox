#pragma once

#include <memory>
#include <chrono>
#include <vector>

namespace library
{
class Simulation
{
    using clock_t = std::chrono::steady_clock;
    using timepoint_t = clock_t::time_point;
    using duration_t = clock_t::duration;
public:
    Simulation();
    virtual ~Simulation();

    void runOneFrame();
    void setMinFps(unsigned int);
    void setMaxFps(unsigned int);
    void setMaxInactiveFps(unsigned int);
    void setTimeSmoothingStep(unsigned int);
    duration_t getTimeSinceLaunch() const;
    unsigned getFps() const;
    std::chrono::duration<float> getDeltaTime() const;
    std::chrono::duration<float> getSmoothedDeltaTime() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
}