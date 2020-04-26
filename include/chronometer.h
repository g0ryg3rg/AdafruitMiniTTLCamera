#pragma once

#include <chrono>

// Basic chronometer based on std::chrono::steady_clock
class Chronometer
{

public:
  void StartTimer()
  {
    start_time_ = std::chrono::steady_clock::now();
  }

  void StopTimer()
  {
    stop_time_= std::chrono::steady_clock::now();
  }

  double GetDurationMillisec()
  {
     std::chrono::duration<double, std::milli> duration_ms = stop_time_ - start_time_;
     return duration_ms.count();
  }
private:
  std::chrono::time_point<std::chrono::steady_clock> start_time_;
  std::chrono::time_point<std::chrono::steady_clock> stop_time_;
};