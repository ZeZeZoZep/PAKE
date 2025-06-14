#ifndef TIMERSET_H
#define TIMERSET_H

#include <chrono>
#include <string>
#include <map>

class TimerSet {
public:
    TimerSet(const std::string& className, const std::string& functionName);

    void start(const std::string& label);
    void stop(const std::string& label);
    void commit(int d = -1);

private:
    std::string className_, functionName_;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> starts_;
    std::map<std::string, long long> durations_;
};

#endif // TIMERSET_H
