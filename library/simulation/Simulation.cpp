#include "Simulation.h"
#include <algorithm>
#include <thread>

namespace library
{
using namespace std::literals;

class Simulation::Impl
{
public:
    /// minimum frames per second
    unsigned int min_fps = 0;
    /// maximum frames per second
    unsigned int max_fps = 200;
    /// maximum inactive fps
    unsigned int max_inactive_fps = 20;
    /// previous time steps for smoothing in seconds
    std::vector<duration_t> previous_timesteps;
    /// next frame time step in seconds
    duration_t timestep = duration_t::zero();
    /// current frame
    std::uint64_t frame = 0;
    /// how many frames to average for the smoothed time step
    unsigned int smoothing_step = 11;
    /// frame update timer
    timepoint_t last_frame_timepoint = clock_t::now();
    /// time point when we launched
    timepoint_t launch_timepoint = clock_t::now();

    static duration_t getFrameDuration(unsigned int fps)
    {
        if (fps == 0) {
            return duration_t::zero();
        }

        return std::chrono::duration_cast<duration_t>(std::chrono::duration<double>(1.0 / static_cast<double>(fps)));
    }

    void updateTimestep(duration_t elapsed)
    {
        if (smoothing_step == 0) {
            timestep = elapsed;
            return;
        }

        previous_timesteps.push_back(elapsed);

        if (previous_timesteps.size() > smoothing_step) {
            auto begin = previous_timesteps.begin();
            previous_timesteps.erase(begin, begin + int(previous_timesteps.size() - smoothing_step));
        }

        timestep = duration_t::zero();

        for (const auto step : previous_timesteps) {
            timestep += step;
        }

        timestep /= static_cast<duration_t::rep>(previous_timesteps.size());
    }

};

Simulation::Simulation()
    : impl{std::make_unique<Impl>()}
{
    impl->last_frame_timepoint = impl->launch_timepoint;

    if (impl->max_inactive_fps == 0) {
        impl->max_inactive_fps = std::max(impl->max_inactive_fps, impl->max_fps);
    }
}

Simulation::~Simulation()
{}

void Simulation::runOneFrame()
{
    const auto previous_frame_timepoint = impl->last_frame_timepoint;
    auto frame_timepoint = clock_t::now();

    if (impl->max_fps > 0) {
        const duration_t target_duration = Impl::getFrameDuration(impl->max_fps);
        const auto target_timepoint = previous_frame_timepoint + target_duration;
        const auto sleep_threshold = std::chrono::duration_cast<duration_t>(1500us);

        if (frame_timepoint + sleep_threshold < target_timepoint) {
            std::this_thread::sleep_until(target_timepoint - sleep_threshold);
            frame_timepoint = clock_t::now();
        }

        while (frame_timepoint < target_timepoint) {
            std::this_thread::yield();
            frame_timepoint = clock_t::now();
        }
    }

    duration_t elapsed = frame_timepoint - previous_frame_timepoint;

    if (elapsed < duration_t::zero()) {
        elapsed = duration_t::zero();
    }

    impl->last_frame_timepoint = frame_timepoint;

    // if fps lower than minimum, clamp eplased time
    if (impl->min_fps > 0) {
        duration_t target_duration = Impl::getFrameDuration(impl->min_fps);

        if (elapsed > target_duration) {
            elapsed = target_duration;
        }
    }

    impl->updateTimestep(elapsed);

    ++impl->frame;
}

void Simulation::setMinFps(unsigned int fps)
{
    impl->min_fps = std::max<unsigned int>(fps, 0);
}

void Simulation::setMaxFps(unsigned int fps)
{
    impl->max_fps = std::max<unsigned int>(fps, 0);
}

void Simulation::setMaxInactiveFps(unsigned int fps)
{
    impl->max_inactive_fps = std::max<unsigned int>(fps, 0);
}

void Simulation::setTimeSmoothingStep(unsigned int step)
{
    impl->smoothing_step = step;

    if (step == 0) {
        impl->previous_timesteps.clear();
        return;
    }

    if (impl->previous_timesteps.size() > step) {
        auto begin = impl->previous_timesteps.begin();
        impl->previous_timesteps.erase(begin, begin + int(impl->previous_timesteps.size() - step));
    }
}

Simulation::duration_t Simulation::getTimeSinceLaunch() const
{
    return clock_t::now() - impl->launch_timepoint;
}

unsigned Simulation::getFps() const
{
    auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(impl->timestep).count();
    return static_cast<unsigned>(dt == 0.0f ? 0 : 1000.0f / dt);
}

std::chrono::duration<float> Simulation::getDeltaTime() const
{
    auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(impl->timestep);
    return dt;
}

}