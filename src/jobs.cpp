#include<algorithm>

#include "jobs.h"

std::vector<Job> jobs;
static int nextJobId = 1;

void addJob(pid_t pid, const std::string& command) {
    Job job;
    job.id = nextJobId++;
    job.pid = pid;
    job.command = command;
    job.status = JobStatus::STOPPED;

    jobs.push_back(job);
}

void removeJob(pid_t pid) {
    jobs.erase(
        std::remove_if(jobs.begin(), jobs.end(), [pid](const Job& job) {
            return job.pid == pid;
        }), 
        jobs.end()
    );
}

const std::vector<Job>& getJobs() {
    return jobs;
}

Job* findJob(int id) {
    for(size_t i=0; i<jobs.size(); i++) {
        if(jobs[i].id == id) return &jobs[i];
    }
    return nullptr;
}

Job* getLastStoppedJob() {
    int n = jobs.size();
    for(int i=n-1; i>=0; i--) {
        if(jobs[i].status == JobStatus::STOPPED) {
            return &jobs[i];
        }
    }
    return nullptr;
}

const char* jobStatusToString(JobStatus status) {
    switch (status) {
        case JobStatus::RUNNING:    return "Running";
        case JobStatus::STOPPED:    return "Stopped";
        case JobStatus::TERMINATED: return "Terminated";
        case JobStatus::DONE:       return "Done";
    }

    return "Unknown";
}