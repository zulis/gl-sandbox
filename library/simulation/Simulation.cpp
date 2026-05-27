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

};

Simulation::Simulation()
    : impl{std::make_unique<Impl>()}
{
    if (impl->max_inactive_fps == 0) {
        impl->max_inactive_fps = std::max(impl->max_inactive_fps, impl->max_fps);
    }
}

Simulation::~Simulation()
{}

void Simulation::runOneFrame()
{
    duration_t elapsed = clock_t::now() - impl->last_frame_timepoint;
    if(impl->max_fps > 0)
    {
        duration_t target_duration = 1000ms / impl->max_fps;

        for(;;)
        {
            elapsed = clock_t::now() - impl->last_frame_timepoint;
            if(elapsed >= target_duration)
            {
                break;
            }

            if(elapsed < duration_t(0))
            {
                break;
            }
            duration_t sleep_time = (target_duration - elapsed);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(sleep_time);

            if(sleep_time > std::chrono::microseconds(1000))
            {
                if(ms.count() > 0)
                {
                    sleep_time /= ms.count();
                }

                std::this_thread::sleep_for(sleep_time);
            }
        }
    }

    if(elapsed < duration_t(0))
    {
        elapsed = duration_t(0);
    }
    impl->last_frame_timepoint = clock_t::now();

    // if fps lower than minimum, clamp eplased time
    if(impl->min_fps > 0)
    {
        duration_t target_duration = 1000ms / impl->min_fps;
        if(elapsed > target_duration)
        {
            elapsed = target_duration;
        }
    }

    // perform time step smoothing
    if(impl->smoothing_step > 0)
    {
        impl->timestep = duration_t::zero();
        impl->previous_timesteps.push_back(elapsed);
        if(impl->previous_timesteps.size() > impl->smoothing_step)
        {
            auto begin = impl->previous_timesteps.begin();
            impl->previous_timesteps.erase(begin, begin + int(impl->previous_timesteps.size() - impl->smoothing_step));
            for(auto step : impl->previous_timesteps)
            {
                impl->timestep += step;
            }
            impl->timestep /= static_cast<duration_t::rep>(impl->previous_timesteps.size());
        }
        else
        {
            impl->timestep = impl->previous_timesteps.back();
        }
    }
    else
    {
        impl->timestep = elapsed;
    }

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