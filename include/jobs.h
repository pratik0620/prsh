#ifndef JOBS_H
#define JOBS_H

#include<iostream>
#include<vector>
#include<unistd.h>

enum class JobStatus {
    RUNNING,
    STOPPED,
    TERMINATED,
    DONE,
};

struct Job {
    int id;
    pid_t pid;
    std::string command;
    JobStatus status;
};

void addJob(pid_t, const std::string&, JobStatus);
void removeJob(pid_t);
const std::vector<Job>& getJobs();
Job* findJob(int);
Job* getLastStoppedJob();
Job* getLastJob();
const char* jobStatusToString(JobStatus);

#endif