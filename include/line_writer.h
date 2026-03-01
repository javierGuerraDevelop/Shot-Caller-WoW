#ifndef SHOTCALLERCPP_LINE_WRITER_H
#define SHOTCALLERCPP_LINE_WRITER_H

#include <fstream>
#include <functional>
#include <string>

// Opens a timestamped file under output/ and returns a callback suitable for
// ShotCallEngine::set_shotcall_callback. The ofstream is managed by the caller.
std::string open_output_file(std::ofstream& file);

std::function<void(const std::string&, const std::string&)> make_shotcall_writer(
    std::ofstream& file);

#endif  // SHOTCALLERCPP_LINE_WRITER_H
