#include "engine.h"
#include "lineReader.h"

using std::cerr;
using std::endl;
using std::string;
using std::vector;

int main()
{
        ShotCallEngine engine;
        string logs_directory = "C:/Program Files (x86)/World of Warcraft/_retail_/Logs";
        string combat_log_file = get_latest_combat_log(logs_directory);
        if (combat_log_file.empty()) {
                cerr << "Failed to find a combat log file. Exiting." << endl;
                return 1;
        }
        vector<string> lines;
        monitor_file(combat_log_file, lines);

        return 0;
}
