#include <filesystem>
#include <fstream>
#include <iostream>

const int kBlockSize = 100;

int main(int argc, char** argv) {
  // argv[1] - filename with data
  // argv[2] - path to directory to store partitioned files

  if (argc != 3) {
    std::cerr << "Invalid arguments number\n";
    return 1;
  }

  std::ifstream read_data(argv[1]);
  std::filesystem::path directory = argv[2];

  std::string string;
  int written_strings_number = kBlockSize;
  std::ofstream write;
  uint64_t file_number = 0;
  while (true) {
    std::getline(read_data, string);
    if (string.empty()) break;

    if (written_strings_number == kBlockSize) {
      written_strings_number = 0;
      std::string new_filename = std::to_string(file_number);
      ++file_number;
      write.close();
      write = std::ofstream(directory / new_filename);
    }

    write << string << '\n';
    ++written_strings_number;
  }
  return 0;
}
