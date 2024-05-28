#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

const std::string kChars = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int kFilenameLength = 10;
const int kBlockSize = 100;

std::string RandomString(int length) {
  std::string string;
  string.reserve(length);

  while(length > 0) {
    string += kChars[rand() % kChars.size()];
    --length;
  }
  return string;
}

std::optional<std::filesystem::path> CreateDirectory() {
  std::string dir_name = RandomString(kFilenameLength);
  while (std::filesystem::exists(dir_name)) {
    dir_name = RandomString(kFilenameLength);
  }

  if (!std::filesystem::create_directory(dir_name)) {
    std::cerr << "Error while creating directory\n";
    return std::nullopt;
  }
  return dir_name;
}

bool IsLess(const std::string& str1, const std::string& str2) {
  std::string_view view1 = {str1.c_str(), str1.find('$')};
  std::string_view view2 = {str2.c_str(), str2.find('$')};
  return view1 < view2;
}

void Merge(const std::filesystem::directory_entry& file1,
           const std::filesystem::directory_entry& file2,
           const std::filesystem::path& output_file) {
  std::ifstream read_file1(file1.path());
  std::ifstream read_file2(file2.path());
  std::ofstream write(output_file);

  std::string str1;
  std::string str2;
  std::getline(read_file1, str1);
  std::getline(read_file2, str2);
  while (true) {
    if (IsLess(str1, str2)) {
      write << str1 << '\n';
      std::getline(read_file1, str1);
      if (str1.empty()) break;
    } else {
      write << str2 << '\n';
      std::getline(read_file2, str2);
      if (str2.empty()) break;
    }
  }

  while (!str1.empty()) {
      write << str1 << '\n';
      std::getline(read_file1, str1);
  }
  while (!str2.empty()) {
    write << str2 << '\n';
    std::getline(read_file2, str2);
  }
}

std::optional<std::filesystem::path> MergeDirectory(
    const std::filesystem::path& directory) {
  auto new_dir = CreateDirectory();
  if (!new_dir.has_value()) {
    return std::nullopt;
  }

  std::filesystem::directory_entry prev_file;
  uint64_t file_number = 0;
  for (auto& file : std::filesystem::directory_iterator{directory}) {
    if (file_number % 2 == 1) {
      Merge(prev_file, file, new_dir.value() / std::to_string(file_number / 2));
    }
    prev_file = file;
    ++file_number;
  }

  if (file_number % 2 == 1) {
    if (!std::filesystem::copy_file(prev_file, new_dir.value() / std::to_string(file_number / 2 + 1))) {
      std::cerr << "Error while copying file\n";
      std::filesystem::remove_all(new_dir.value());
      return std::nullopt;
    }
  }
  return new_dir;
}

int main(int argc, char** argv) {
  // argv[1] - directory with input files
  // argv[2] - output file

  if (argc != 3) {
    std::cerr << "Invalid arguments number\n";
    return 1;
  }

  auto start_dir = CreateDirectory();
  if (!start_dir.has_value()) {
    return 1;
  }

  uint64_t files_number = 0;
  std::filesystem::path input_directory = argv[1];
  std::vector<std::string> contents;

  for (const auto& file : std::filesystem::directory_iterator{input_directory}) {
    std::string string;
    std::ifstream read(file.path());
    while (true) {
      std::getline(read, string);
      if (string.empty()) break;
      contents.push_back(string);

      if (contents.size() == kBlockSize) {
        std::string new_filename = std::to_string(files_number);
        ++files_number;

        std::sort(contents.begin(), contents.end(), IsLess);
        std::ofstream write(start_dir.value() / new_filename);
        for (const auto& str : contents) {
          write << str << '\n';
        }
        contents.clear();
      }
    }
    read.close();
  }

  if (!contents.empty()) {
    std::string new_filename = std::to_string(files_number);
    ++files_number;

    std::sort(contents.begin(), contents.end(), IsLess);
    std::ofstream write(start_dir.value() / new_filename);
    for (const auto& str : contents) {
      write << str << '\n';
    }
  }

  if (files_number == 1) {
    for (const auto& file : std::filesystem::directory_iterator{start_dir.value()}) {
      std::filesystem::copy_file(file, argv[2]);
    }
    std::filesystem::remove_all(start_dir.value());
    return 0;
  }

  uint iterations_number = std::bit_width(std::bit_ceil(files_number)) - 1;
  std::filesystem::path prev_dir = start_dir.value();
  for (uint i = 0; i + 1 < iterations_number; ++i) {
    auto new_dir = MergeDirectory(prev_dir);
    if (!new_dir.has_value()) {
      if (i != 0) {
        std::filesystem::remove_all(prev_dir);
      }
      return 1;
    }

    std::filesystem::remove_all(prev_dir);
    prev_dir = new_dir.value();
  }

  std::filesystem::directory_entry file1;
  int file_number = 0;
  for (auto& file : std::filesystem::directory_iterator{prev_dir}) {
    if (file_number == 0) {
      file1 = file;
    } else {
      Merge(file1, file, argv[2]);
    }
    ++file_number;
  }
  std::filesystem::remove_all(prev_dir);
  return 0;
}
