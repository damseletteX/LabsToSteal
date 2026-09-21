#include "../include/Dir.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>

namespace fs = std::filesystem;

DirectorySynchronizer::DirectorySynchronizer(
    const std::string& src, 
    const std::string& tgt, 
    int maxProc
) : sourceDir(src), targetDir(tgt), maxProcesses(maxProc), activeProcesses(0) {}

bool DirectorySynchronizer::validateDirectories() {
    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        std::cerr << "Source directory does not exist or is not a directory: " << sourceDir << std::endl;
        return false;
    }
    if (!fs::exists(targetDir)) {
        if (!fs::create_directories(targetDir)) {
            std::cerr << "Failed to create target directory: " << targetDir << std::endl;
            return false;
        }
    }
    return true;
}

void DirectorySynchronizer::printSyncInfo() {
    std::cout << "Starting synchronization..." << std::endl;
    std::cout << "Source: " << sourceDir << std::endl;
    std::cout << "Target: " << targetDir << std::endl;
    std::cout << "Max concurrent processes: " << maxProcesses << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

bool DirectorySynchronizer::copyFileWithPermissions(
    const std::string& source, 
    const std::string& destination
) {
    int source_fd = open(source.c_str(), O_RDONLY);
    if (source_fd == -1) {
        return false;
    }
    struct stat source_stat;
    if (fstat(source_fd, &source_stat) == -1) {
        close(source_fd);
        return false;
    }
    int dest_fd = open(destination.c_str(), O_WRONLY | O_CREAT | O_TRUNC, source_stat.st_mode);
    if (dest_fd == -1) {
        close(source_fd);
        return false;
    }
    bool copy_success = copyFileContent(source_fd, dest_fd);
    close(source_fd);
    close(dest_fd);
    if (copy_success) {
        copy_success = (chmod(destination.c_str(), source_stat.st_mode) == 0);
    }
    return copy_success;
}

bool DirectorySynchronizer::copyFileContent(int source_fd, int dest_fd) {
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            return false;
        }
    }
    return (bytes_read == 0);
}

std::vector<std::string> DirectorySynchronizer::getFilesToCopy() {
    std::vector<std::string> filesToCopy;
    try {
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                processFileEntry(entry, filesToCopy);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Directory access error: " << e.what() << std::endl;
    }
    return filesToCopy;
}

void DirectorySynchronizer::processFileEntry(
    const fs::directory_entry& entry,
    std::vector<std::string>& filesToCopy
) {
    std::string filename = entry.path().filename().string();
    std::string targetPath = targetDir + "/" + filename;
    if (!fs::exists(targetPath)) {
        filesToCopy.push_back(filename);
    }
}

void DirectorySynchronizer::startCopyProcess(const std::string& filename) {
    pid_t pid = fork();
    if (pid == 0) {
        executeFileCopy(filename);
    } else if (pid > 0) {
        activeProcesses++;
    } else {
        handleForkError(filename);
    }
}

void DirectorySynchronizer::executeFileCopy(const std::string& filename) {
    std::string sourcePath = sourceDir + "/" + filename;
    std::string targetPath = targetDir + "/" + filename;
    ssize_t file_size = getFileSize(sourcePath);
    if (copyFileWithPermissions(sourcePath, targetPath)) {
        printCopySuccess(filename, file_size);
    } else {
        printCopyError(filename);
    }
    exit(0);
}

ssize_t DirectorySynchronizer::getFileSize(const std::string& filepath) {
    struct stat stat_buf;
    if (stat(filepath.c_str(), &stat_buf) == 0) {
        return stat_buf.st_size;
    }
    return 0;
}

void DirectorySynchronizer::printCopySuccess(const std::string& filename, ssize_t bytes_copied) {
    std::cout << "PID: " << getpid() 
              << " | File: " << filename 
              << " | Bytes copied: " << bytes_copied << std::endl;
}

void DirectorySynchronizer::printCopyError(const std::string& filename) {
    std::cout << "PID: " << getpid() 
              << " | File: " << filename 
              << " | COPY ERROR" << std::endl;
}

void DirectorySynchronizer::handleForkError(const std::string& filename) {
    std::cerr << "Failed to create process for file: " << filename << std::endl;
}

void DirectorySynchronizer::waitForProcesses() {
    while (activeProcesses > 0) {
        int status;
        pid_t finished_pid = wait(&status);
        if (finished_pid > 0) {
            activeProcesses--;
        }
    }
}

void DirectorySynchronizer::processFilesBatch(const std::vector<std::string>& filesToCopy) {
    for (const auto& filename : filesToCopy) {
        waitForAvailableProcessSlot();
        startCopyProcess(filename);
    }
}

void DirectorySynchronizer::waitForAvailableProcessSlot() {
    while (activeProcesses >= maxProcesses) {
        waitForProcesses();
    }
}

void DirectorySynchronizer::printCompletionMessage() {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Synchronization completed successfully." << std::endl;
}

void DirectorySynchronizer::printNoFilesMessage() {
    std::cout << "All files are already synchronized." << std::endl;
}

void DirectorySynchronizer::synchronize() {
    // 1. Проверяем директории
    if (!validateDirectories()) return;
    // 2. Выводим информацию
    printSyncInfo();
    // 3. Получаем список файлов для копирования
    std::vector<std::string> filesToCopy = getFilesToCopy();
    // 4. Если нечего копировать - выходим
    if (filesToCopy.empty()) {
        printNoFilesMessage();
        return;
    }
    // 5. Копируем файлы с контролем процессов
    processFilesBatch(filesToCopy);
    // 6. Ждем завершения всех процессов
    waitForProcesses();
    // 7. Выводим финальное сообщение
    printCompletionMessage();
}