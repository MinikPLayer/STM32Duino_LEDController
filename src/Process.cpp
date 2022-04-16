#include "Process.hpp"

std::vector<Process*> Process::processes;

void Process::_RunTicks()
{
    for(int i = 0;i<processes.size();i++)
        if(processes[i] != nullptr && processes[i]->IsActive())
            processes[i]->Tick();
}