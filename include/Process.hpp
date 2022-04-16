#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include <vector>

class Process
{
    static std::vector<Process*> processes;

protected:
    bool active = true;

    Process() 
    {
        processes.push_back(this);
    }

    ~Process()
    {
        for(size_t i = 0;i<processes.size();i++) {
            if(processes[i] == this) {
                processes.erase(processes.begin() + i);
                break;
            }
        }
    }
public:
    bool IsActive() { return active; }

    virtual void Tick() {}

    static void _RunTicks();
};

#endif