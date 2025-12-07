#include "engine.h"
#include "lineReader.h"
#include "parser.h"

int main() {
    ShotCallEngine engine;
    std::string write_filename = "../text_files/combat_log_write.txt";
    std::string read_filename = "../text_files/combat_log_large.txt";

    std::vector<std::string> lines;
    monitor_file(write_filename, lines);

    // for (size_t i = 0; i < lines.size(); ++i)
    // {
    //     combat_event event = parse_line(lines[i]);
    //     std::cout << lines[i] << std::endl;
    // }
}
