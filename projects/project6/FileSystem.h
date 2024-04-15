#include <iostream>
#include <map>
#include <ctime>
#include <cstdlib>  // For size_t

using namespace std;

enum FileType {
    FileType_FILE,
    FileType_DIRECTORY
};

class File {
public:
    string name;
    FileType type;
    time_t created_at;
    time_t modified_at;
    bool isOpen;
    string permissions;
    unsigned long size;  // For file size in bytes
    map<string, File*> children;  // Only used if FileType is DIRECTORY

    File(const string& n, FileType t, const string& perms, unsigned long sz = 0)
        : name(n), type(t), permissions(perms), isOpen(false), size(sz) {
        time(&created_at);
        modified_at = created_at;
    }

    ~File() {
        // Manually delete children to prevent memory leaks
        for (map<string, File*>::iterator it = children.begin(); it != children.end(); ++it) {
            delete it->second;
        }
    }

    void displayInfo() const {
        cout << "Name: " << name << ", Type: " << (type == FileType_DIRECTORY ? "Directory" : "File")
             << ", Permissions: " << permissions << ", Size: " << size << " bytes" << endl;
    }
};

class FileSystem {
private:
    File* root;

    // Recursive helper function to display the directory tree
    void displayDirectoryTreeHelper(const File* directory, int depth) const {
        if (!directory) return;  // Safety check to prevent dereferencing nullptr

        // Print the current directory or file with indentation based on depth
        cout << string(depth * 2, ' ') << (directory->type == FileType_DIRECTORY ? "Directory: " : "File: ") << directory->name << endl;

        // If the current node is a directory, recursively display its contents
        if (directory->type == FileType_DIRECTORY) {
            for (map<string, File*>::const_iterator it = directory->children.begin(); it != directory->children.end(); ++it) {
                displayDirectoryTreeHelper(it->second, depth + 1);
            }
        }
    }

    // Recursive helper function to search for the file
    bool searchFileHelper(const string& fileName, const File* directory, const string& currentPath) const {
        if (!directory) return false;  // Safety check

        // Check each file in the current directory
        for (map<string, File*>::const_iterator it = directory->children.begin(); it != directory->children.end(); ++it) {
            string fullPath = currentPath + "/" + it->first;
            if (it->second->name == fileName) {
                cout << "File found: " << fullPath << endl;
                return true;
            }
            // If it's a directory, recurse into it
            if (it->second->type == FileType_DIRECTORY) {
                if (searchFileHelper(fileName, it->second, fullPath)) {
                    return true;  // Return true if file is found in the subtree
                }
            }
        }
        return false;  // File not found in this branch
    }

public:
    FileSystem() {
        root = new File("/", FileType_DIRECTORY, "rwx");
    }

    ~FileSystem() {
        delete root;
    }

    bool createFile(const string& path, FileType type, const string& permissions, unsigned long size = 0) {
        size_t lastSlash = path.rfind('/');
        if (lastSlash == string::npos || lastSlash + 1 >= path.length()) {
            cout << "Invalid path or directory name." << endl;
            return false;
        }
        
        string parentPath = path.substr(0, lastSlash);
        string name = path.substr(lastSlash + 1);

        File* parent = findFile(parentPath);
        if (!parent || parent->type != FileType_DIRECTORY) {
            cout << "Invalid directory path." << endl;
            return false;
        }

        if (parent->children.find(name) != parent->children.end()) {
            cout << "File or directory already exists." << endl;
            return false;
        }

        File* newFile = new File(name, type, permissions, size);
        parent->children[name] = newFile;
        cout << "Created " << (type == FileType_DIRECTORY ? "directory" : "file") << ": " << path << endl;
        return true;
    }

    bool deleteFile(const string& path) {
        if (path == "/") {
            cout << "Cannot delete the root directory." << endl;
            return false;
        }

        size_t lastSlash = path.rfind('/');
        if (lastSlash == string::npos) {
            cout << "Invalid path." << endl;
            return false;
        }

        string parentPath = path.substr(0, lastSlash);
        string name = path.substr(lastSlash + 1);
        File* parent = findFile(parentPath);

        if (!parent || parent->type != FileType_DIRECTORY) {
            cout << "Invalid parent directory path." << endl;
            return false;
        }

        map<string, File*>::iterator it = parent->children.find(name);
        if (it == parent->children.end()) {
            cout << "File or directory not found." << endl;
            return false;
        }

        File* fileToDelete = it->second;
        if (fileToDelete->type == FileType_DIRECTORY && !fileToDelete->children.empty()) {
            cout << "Directory is not empty." << endl;
            return false;
        }

        // Delete the file or directory and remove from the parent's children map
        delete fileToDelete;  // Deallocate the memory
        parent->children.erase(it);  // Remove the entry from the map

        cout << "Deleted " << name << " successfully." << endl;
        return true;
    }

    bool renameFile(const string& oldPath, const string& newName) {
        File* file = findFile(oldPath);
        if (!file) {
            cout << "File not found." << endl;
            return false;
        }
        // Check if new name exists in the parent directory
        size_t lastSlash = oldPath.rfind('/');
        string parentPath = oldPath.substr(0, lastSlash);
        File* parent = findFile(parentPath);
        if (parent->children.find(newName) != parent->children.end()) {
            cout << "A file or directory with the new name already exists." << endl;
            return false;
        }
        // Rename the file or directory
        parent->children[newName] = file;
        parent->children.erase(file->name);
        file->name = newName;
        return true;
    }

    // Display directory tree starting from a specific directory given by path
    void displayDirectoryTree(const string& startPath = "/") const {
        File* startNode = findFile(startPath);  // Find the starting node based on the path
        if (!startNode) {
            cout << "Directory not found: " << startPath << endl;
            return;
        }
        displayDirectoryTreeHelper(startNode, 0);  // Start from the found directory
    }

    // Utility to find a file given a path
    File* findFile(const string& path) const {
        // Implementation of path traversal to find the file
        if (path == "/") return root;
        File* current = root;
        size_t pos = 1; // Start after the initial '/'
        while (pos != string::npos && pos < path.size()) {
            size_t nextPos = path.find('/', pos);
            string part = path.substr(pos, nextPos - pos);
            map<string, File*>::const_iterator it = current->children.find(part);
            if (it == current->children.end()) return NULL; // No such path part exists
            current = it->second; // Move to the next part of the path
            pos = (nextPos != string::npos) ? nextPos + 1 : string::npos; // Move past the '/' if not at the end
        }
        return current;
    }

    void listFiles(const string& directoryPath) {
        File* directory = findFile(directoryPath);
        if (!directory || directory->type != FileType_DIRECTORY) {
            cout << "Invalid directory path." << endl;
            return;
        }

        cout << "Contents of directory " << directoryPath << ":" << endl;
        for (map<string, File*>::iterator it = directory->children.begin(); it != directory->children.end(); ++it) {
            it->second->displayInfo();
        }
    }

    bool moveFile(const string& oldPath, const string& newPath) {
        // Extract file name and new directory path
        size_t oldFileNameIndex = oldPath.rfind('/');
        if (oldFileNameIndex == string::npos) return false;
        string fileName = oldPath.substr(oldFileNameIndex + 1);

        size_t newDirIndex = newPath.rfind('/');
        if (newDirIndex == string::npos) return false;
        string newDirPath = newPath.substr(0, newDirIndex);
        string newFileName = newPath.substr(newDirIndex + 1);

        // Locate old and new directories
        File* oldDir = findFile(oldPath.substr(0, oldFileNameIndex));
        File* newDir = findFile(newDirPath);
        if (!oldDir || !newDir || oldDir->type != FileType_DIRECTORY || newDir->type != FileType_DIRECTORY) {
            return false;
        }

        // Check if the file exists in the old directory and if the target name is free in the new directory
        if (oldDir->children.find(fileName) == oldDir->children.end() || newDir->children.find(newFileName) != newDir->children.end()) {
            return false;
        }

        // Move the file
        File* file = oldDir->children[fileName];
        oldDir->children.erase(fileName);
        newDir->children[newFileName] = file;
        file->name = newFileName; // Update the file name if it has changed
        return true;
    }

    bool duplicateFile(const string& sourcePath, const string& targetPath) {
        size_t sourceFileNameIndex = sourcePath.rfind('/');
        size_t targetFileNameIndex = targetPath.rfind('/');
        if (sourceFileNameIndex == string::npos || targetFileNameIndex == string::npos) return false;

        string sourceDirPath = sourcePath.substr(0, sourceFileNameIndex);
        string targetDirPath = targetPath.substr(0, targetFileNameIndex);
        string sourceFileName = sourcePath.substr(sourceFileNameIndex + 1);
        string targetFileName = targetPath.substr(targetFileNameIndex + 1);

        File* sourceDir = findFile(sourceDirPath);
        File* targetDir = findFile(targetDirPath);
        if (!sourceDir || !targetDir || sourceDir->type != FileType_DIRECTORY || targetDir->type != FileType_DIRECTORY) {
            return false;
        }

        if (sourceDir->children.find(sourceFileName) == sourceDir->children.end() || targetDir->children.find(targetFileName) != targetDir->children.end()) {
            return false;
        }

        File* originalFile = sourceDir->children[sourceFileName];
        File* newFile = new File(*originalFile); // Assuming File has a copy constructor
        newFile->name = targetFileName;
        targetDir->children[targetFileName] = newFile;
        return true;
    }

    bool duplicateDirectory(const string& sourcePath, const string& targetPath) {
        File* sourceDir = findFile(sourcePath);
        File* targetDir = findFile(targetPath);
        if (!sourceDir || !targetDir || sourceDir->type != FileType_DIRECTORY || targetDir->type != FileType_DIRECTORY) {
            return false;
        }

        // Recursively copy all files and directories
        for (map<string, File*>::iterator it = sourceDir->children.begin(); it != sourceDir->children.end(); ++it) {
            string newPath = targetPath + "/" + it->first;
            if (it->second->type == FileType_DIRECTORY) {
                File* newDir = new File(it->second->name, FileType_DIRECTORY, it->second->permissions);
                targetDir->children[it->second->name] = newDir;
                duplicateDirectory(sourcePath + "/" + it->second->name, newPath);
            } else {
                duplicateFile(sourcePath + "/" + it->second->name, newPath);
            }
        }
        return true;
    }

    // Search for a file by name starting from the specified directory
    bool searchFile(const string& fileName, const string& startPath = "/") const {
        File* startNode = findFile(startPath);  // Find the starting node based on the path
        if (!startNode) {
            cout << "Starting directory not found: " << startPath << endl;
            return false;
        }
        return searchFileHelper(fileName, startNode, startPath);
    }

    // Method to display basic information
    void displayBasicInfo(const std::string& path) {
        File* file = findFile(path);
        if (!file) {
            std::cout << "File or directory not found." << std::endl;
            return;
        }
        std::cout << "Name: " << file->name << std::endl;
        std::cout << "Type: " << (file->type == FileType_DIRECTORY ? "Directory" : "File") << std::endl;
        if (file->type == FileType_FILE) {
            std::cout << "Size: " << file->size << " bytes" << std::endl;
        }
    }

    // Method to display detailed information
    void displayDetailedInfo(const std::string& path) {
        File* file = findFile(path);
        if (!file) {
            std::cout << "File or directory not found." << std::endl;
            return;
        }
        displayBasicInfo(path); // Display basic info
        std::cout << "Permissions: " << file->permissions << std::endl;
        std::cout << "Created At: " << ctime(&file->created_at);
        std::cout << "Modified At: " << ctime(&file->modified_at);
        if (file->type == FileType_DIRECTORY) {
            std::cout << "Contains: " << file->children.size() << " items" << std::endl;
        }
    }

};
