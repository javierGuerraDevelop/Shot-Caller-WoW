#include "lineReader.h"

namespace ch = std::chrono;
using std::cerr;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::ios;
using std::string;
using std::vector;

string get_latest_combat_log(const string& logs_directory)
{
        namespace fs = std::filesystem;

        string latest_file;
        fs::file_time_type latest_time;
        bool found = false;

        if (!fs::exists(logs_directory) || !fs::is_directory(logs_directory)) {
                cerr << "Logs directory does not exist: " << logs_directory << endl;
                return "";
        }

        for (const auto& entry : fs::directory_iterator(logs_directory)) {
                if (!entry.is_regular_file())
                        continue;

                string filename = entry.path().filename().string();
                if (filename.find("WoWCombatLog") == string::npos)
                        continue;

                auto file_time = entry.last_write_time();
                if (!found || file_time > latest_time) {
                        latest_time = file_time;
                        latest_file = entry.path().string();
                        found = true;
                }
        }

        if (!found) {
                cerr << "No combat log files found in: " << logs_directory << endl;
                return "";
        }

        cout << "Found latest combat log: " << latest_file << endl;
        return latest_file;
}

void trim_whitespace(string& str)
{
        const string WHITESPACE = " \n\r\t\f\v";
        size_t end = str.find_last_not_of(WHITESPACE);
        if (end == string::npos) {
                str.clear();
                return;
        }
        str.erase(end + 1);

        size_t start = str.find_first_not_of(WHITESPACE);
        str.erase(0, start);
}

vector<string> read_file(const string& filename)
{
        vector<string> lines;
        ifstream input_file(filename);
        if (!input_file.is_open()) {
                cerr << "Error opening file" << filename << endl;
                return vector<string>{};
        }

        string line;
        while (getline(input_file, line)) {
                trim_whitespace(line);
                lines.push_back(line);
        }
        return lines;
}

void monitor_file(const string& filename, vector<string> lines)
{
        ifstream input_file(filename);
        if (!input_file.is_open()) {
                cerr << "Error opening file: " << filename << "\n";
                return;
        }

        input_file.seekg(0, ios::end);
        cout << "Monitoring for new lines in: " << filename << "...\n";
        string line{};
        while (true) {
                if (getline(input_file, line)) {
                        trim_whitespace(line);
                        if (!line.empty()) {
                                cout << line << endl;
                                lines.push_back(line);
                        }
                } else {
                        input_file.clear();
                        input_file.seekg(0, ios::cur);
                        std::this_thread::sleep_for(ch::milliseconds(250));
                }
        }
}
