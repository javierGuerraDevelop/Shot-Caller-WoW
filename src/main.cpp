#include <fstream>
#include <iostream>
#include <thread>

#include "engine.h"
#include "line_reader.h"
#include "line_writer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <logs_directory>" << std::endl;
        return 1;
    }

    // Open timestamped output file
    std::ofstream output_file;
    std::string output_path = open_output_file(output_file);
    if (!output_file.is_open()) {
        return 1;
    }
    std::cout << "Writing shotcalls to: " << output_path << std::endl;

    // Wire engine callback to write shotcalls to file
    ShotCallEngine engine{};
    engine.set_shotcall_callback(make_shotcall_writer(output_file));

    // Run process_shotcalls on a background thread
    std::thread shotcall_thread([&engine]() {
        engine.process_shotcalls();
    });

    // Find the log file
    std::string logs_directory = argv[1];
    std::string combat_log_file = get_latest_combat_log(logs_directory);
    if (combat_log_file.empty()) {
        std::cerr << "Failed to find a combat log file. Exiting." << std::endl;
        return 1;
    }

    // Seek to end so we only pick up lines written after launch
    std::ifstream log_file{ combat_log_file };
    if (!log_file.is_open()) {
        std::cerr << "Failed to open combat log: " << combat_log_file << std::endl;
        return 1;
    }
    log_file.seekg(0, std::ios::end);
    std::cout << "Monitoring: " << combat_log_file << std::endl;

    // Read new lines, parse, and feed events to the engine
    std::string line;
    while (true) {
        if (std::getline(log_file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            CombatEvent event{ parse_line(line) };
            if (!event.event_type.empty()) {
                engine.handle_event(event);
            }
        } else {
            log_file.clear();
            log_file.seekg(0, std::ios::cur);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }

    shotcall_thread.join();
    return 0;
}
