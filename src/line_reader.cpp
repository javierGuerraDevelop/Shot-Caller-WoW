#include "lineReader.h"

void trim_whitespace(std::string& string) {
    const std::string WHITESPACE = " \n\r\t\f\v";

    size_t end = string.find_last_not_of(WHITESPACE);
    if (end == std::string::npos) {
        string.clear();
        return;
    }
    string.erase(end + 1);

    size_t start = string.find_first_not_of(WHITESPACE);
    string.erase(0, start);
}

std::vector<std::string> read_file(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << filename << std::endl;
        return std::vector<std::string>{};
    }

    std::string line;
    while (std::getline(input_file, line)) {
        trim_whitespace(line);
        lines.push_back(line);
    }

    return lines;
}

void monitor_file(const std::string& filename, std::vector<std::string> lines) {
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    input_file.seekg(0, std::ios::end);
    std::cout << "Monitoring for new lines in: " << filename << "...\n";

    std::string line;
    while (true) {
        if (std::getline(input_file, line)) {
            trim_whitespace(line);
            if (!line.empty()) {
                std::cout << line << std::endl;
                lines.push_back(line);
            }
        } else {
            input_file.clear();
            input_file.seekg(0, std::ios::cur);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
}
