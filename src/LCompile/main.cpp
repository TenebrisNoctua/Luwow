#include <iostream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "LuauCompiler.h"

using json = nlohmann::json;

// Obtains every file in a given directory path, recursively.
void recursiveReadDirectory(std::filesystem::path filePath, std::vector<std::string> &validFiles) {
    for (const auto& entry: std::filesystem::directory_iterator(filePath)) {
        const std::filesystem::path newPath = entry.path();
        if (std::filesystem::is_regular_file(newPath)) {
            std::string extensionString = newPath.extension().string();
            if (!(extensionString.find(".luau") != std::string::npos)) {
                std::cerr << "Invalid file type found: " << newPath.string() << std::endl;
            }
            validFiles.push_back(newPath.string());
        } else if (std::filesystem::is_directory(newPath)) {
            recursiveReadDirectory(newPath, validFiles);
        } else {
            std::cerr << "Invalid file type found: " << newPath.string() << std::endl;
        }
    }
};

// Reads the given paths from a configuration JSON file.
std::vector<std::string> readPathsFromJSON(std::string path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file) {
        std::cerr << "There was an error opening the file." << std::endl;
    }

    json j;
    file >> j;
    file.close();

    if (!j.contains("tree")) {
        std::cerr << "Provided configuration JSON does not have a file tree." << std::endl;
    }

    std::vector<std::string> validFiles;
    const json tree = j["tree"];

    for (auto& element: tree.items()) {
        std::string key = element.key();
        std::string value = element.value();

        // TODO: If we ever implement a Project class, the keys or alias fields in the tree could be used as aliases.

        std::filesystem::path sourcePath = std::filesystem::current_path().parent_path();
        std::filesystem::path finalPath = (sourcePath / value).lexically_normal();

        // Check if the path is valid and whether it points to a file or directory
        if (std::filesystem::exists(finalPath)) {
            if (std::filesystem::is_regular_file(finalPath)) {
                std::string extensionString = finalPath.extension().string();
                if (!(extensionString.find(".luau") != std::string::npos)) {
                    std::cerr << "Invalid file type found: " << finalPath.string() << std::endl;
                }
                // This is a hack to make LRun run a given main file first.
                // This should be removed in favor of a LRun re-write for a Project class.
                if (key.find("main") != std::string::npos) {
                    validFiles.insert(validFiles.begin() + 0, finalPath.string());
                } else {
                    validFiles.push_back(finalPath.string());
                }
            } else if (std::filesystem::is_directory(finalPath)) {
                recursiveReadDirectory(finalPath, validFiles);
            } else {
                std::cerr << "Invalid file type found: " << finalPath.string() << std::endl;
            }
        } else {
            std::cerr << finalPath << " does not exist." << std::endl;
        }
    }

    return validFiles;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // TODO: Make arguments so people don't accidentally write over their last luau script
        std::cerr << "Usage: lcompile <script.luau> <script2.luau> <script3.luau> ... <output.package>" << std::endl;
        std::cerr << "Or: lcompile --c <config.json> <output.package>" << std::endl;
        return 1;
    }
    
    std::cout << "Luau Compiler - Compile Luau Scripts" << std::endl;

    LuauCompiler compiler;

    if (strcmp(argv[1], "--c") == 0) {
        std::vector<std::string> validPaths = readPathsFromJSON(argv[2]);
        for (const auto& scriptPath: validPaths) {
            if (!compiler.compileScript(scriptPath)) {
                std::cerr << "Failed to compile script: " << scriptPath << std::endl;
                return 1;
            }
        }
        compiler.savePackage(argv[argc - 1]);
    } else {
        // Compile all files passed in on the command line through the luau compiler
        // and append them into the package in the order specified on the command line.
        
        for (int i = 1; i < argc - 1; i++) {
            std::string scriptPath = argv[i];
            if (!compiler.compileScript(scriptPath)) {
                std::cerr << "Failed to compile script: " << scriptPath << std::endl;
                return 1;
            }
        }
        compiler.savePackage(argv[argc - 1]);
    };

    std::cout << "Luau Compiler - Successfully compiled the scripts to a package." << std::endl;

    return 0;
}
