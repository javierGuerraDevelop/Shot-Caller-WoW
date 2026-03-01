#ifndef SHOTCALLERCPP_LINE_READER_H
#define SHOTCALLERCPP_LINE_READER_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void monitor_file(const std::string& filename, std::vector<std::string> lines);
std::vector<std::string> read_file(const std::string& filename);
std::string get_latest_combat_log(const std::string& logs_directory);

#endif  // SHOTCALLERCPP_LINE_READER_H
