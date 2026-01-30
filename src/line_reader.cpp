#include "lineReader.h"

namespace ch = std::chrono;

std::string get_latest_combat_log(const std::string& logs_directory)
{
    namespace fs = std::filesystem;

    std::string latest_file;
    fs::file_time_type latest_time;
    bool found = false;

    if (!fs::exists(logs_directory) || !fs::is_directory(logs_directory)) {
        std::cerr << "Logs directory does not exist: " << logs_directory << std::endl;
        return "";
    }

    for (const auto& entry : fs::directory_iterator(logs_directory)) {
        if (!entry.is_regular_file())
            continue;

        std::string filename = entry.path().filename().string();
        if (filename.find("WoWCombatLog") == std::string::npos)
            continue;

        auto file_time = entry.last_write_time();
        if (!found || file_time > latest_time) {
            latest_time = file_time;
            latest_file = entry.path().string();
            found = true;
        }
    }

    if (!found) {
        std::cerr << "No combat log files found in: " << logs_directory << std::endl;
        return "";
    }

    std::cout << "Found latest combat log: " << latest_file << std::endl;
    return latest_file;
}

void trim_whitespace(std::string& str)
{
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t end = str.find_last_not_of(WHITESPACE);
    if (end == std::string::npos) {
        str.clear();
        return;
    }
    str.erase(end + 1);

    size_t start = str.find_first_not_of(WHITESPACE);
    str.erase(0, start);
}

std::vector<std::string> read_file(const std::string& filename)
{
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

void monitor_file(const std::string& filename, std::vector<std::string> lines)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    input_file.seekg(0, std::ios::end);
    std::cout << "Monitoring for new lines in: " << filename << "...\n";
    std::string line{};
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
            std::this_thread::sleep_for(ch::milliseconds(250));
        }
    }
}
