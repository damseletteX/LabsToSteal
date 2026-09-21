#pragma once
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

class DirectorySynchronizer
{
private:
    std::string sourceDir;
    std::string targetDir;
    int maxProcesses;
    int activeProcesses;
    bool validateDirectories();
    void printSyncInfo();
    bool copyFileWithPermissions(
        const std::string &source,
        const std::string &destination);
    bool copyFileContent(int source_fd, int dest_fd);
    std::vector<std::string> getFilesToCopy();
    void processFileEntry(
        const fs::directory_entry &entry,
        std::vector<std::string> &filesToCopy);
    void startCopyProcess(const std::string &filename);
    void executeFileCopy(const std::string &filename);
    ssize_t getFileSize(const std::string &filepath);
    void printCopySuccess(
        const std::string &filename,
        ssize_t bytes_copied);
    void printCopyError(const std::string &filename);
    void handleForkError(const std::string &filename);
    void waitForProcesses();
    void processFilesBatch(const std::vector<std::string> &filesToCopy);
    void waitForAvailableProcessSlot();
    void printCompletionMessage();
    void printNoFilesMessage();

public:
    DirectorySynchronizer(
        const std::string &src,
        const std::string &tgt,
        int maxProc);
    void synchronize();
};