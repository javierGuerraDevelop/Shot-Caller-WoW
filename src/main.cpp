#include "engine.h"
#include "lineReader.h"

int main() {
    ShotCallEngine engine;
    std::string logs_directory = "C:/Program Files (x86)/World of Warcraft/_retail_/Logs";
    std::string combat_log_file = get_latest_combat_log(logs_directory);
    if (combat_log_file.empty()) {
        std::cerr << "Failed to find a combat log file. Exiting." << std::endl;
        return 1;
    }
    std::vector<std::string> lines;
    monitor_file(combat_log_file, lines);

    return 0;
}
