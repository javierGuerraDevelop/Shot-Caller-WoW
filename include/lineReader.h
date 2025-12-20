#ifndef SHOTCALLERCPP_LINEREADER_H
#define SHOTCALLERCPP_LINEREADER_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using std::string;
using std::vector;

void monitor_file(const string& filename, vector<string> lines);
vector<string> read_file(const string& filename);
string get_latest_combat_log(const string& logs_directory);

#endif  // SHOTCALLERCPP_LINEREADER_H
