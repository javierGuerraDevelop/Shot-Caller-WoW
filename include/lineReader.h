#ifndef SHOTCALLERCPP_LINEREADER_H
#define SHOTCALLERCPP_LINEREADER_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void monitor_file(const std::string& filename, std::vector<std::string> lines);
std::vector<std::string> read_file(const std::string& filename);

#endif  // SHOTCALLERCPP_LINEREADER_H
