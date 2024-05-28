#include <boost/process.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

const int kProcessesNumber = 4;
const int kBlockSize = 100;
const std::string kChars = "0123456789"
                          "abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int kFilenameLength = 10;

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

std::optional<std::filesystem::path> Map(const char* mapper,
                                         const std::filesystem::path& input_directory) {
  auto directory = CreateDirectory();
  if (!directory.has_value()) {
    std::cerr << "Unable to create directory\n";
    return std::nullopt;
  }

  std::vector<boost::process::child> children(kProcessesNumber);
  int current_child = 0;
  uint64_t file_number = 0;
  for (auto& file : std::filesystem::directory_iterator{input_directory}) {
    children[current_child].wait();
    std::string new_filename = std::to_string(file_number);
    ++file_number;
    children[current_child] = boost::process::child(mapper,
                                                    boost::process::std_in < file.path(),
                                                    boost::process::std_out > directory.value() / new_filename);
    current_child = (current_child + 1) % children.size();
  }

  for (auto& child : children) {
    child.wait();
  }
  return directory;
}

int main(int argc, char** argv) {
  // argv[1] - input file
  // argv[2] - path to partition_map script
  // argv[3] - path to map script
  // argv[4] - path external_sort script
  // argv[5] - path to reduce1 script
  // argv[6] - path to reduce2 script
  // argv[7] - output_file filename

  srand(time(nullptr));
  if (argc != 8) {
    std::cerr << "Invalid arguments number\n";
  }

  // partition map
  auto partitioned_files_dir = CreateDirectory();  // directory with partitioned files
  if (!partitioned_files_dir.has_value()) {
    return 1;
  }
  auto partition_map = boost::process::child(argv[2],
                                             std::filesystem::absolute(argv[1]).c_str(),
                                             absolute(partitioned_files_dir.value()).c_str());
  partition_map.wait();
  if (partition_map.exit_code() != 0) {
    std::filesystem::remove_all(partitioned_files_dir.value());
    return partition_map.exit_code();
  }

  // map
  auto mapped_files_dir = Map(argv[3], partitioned_files_dir.value());  // directory with mapped files
  std::filesystem::remove_all(partitioned_files_dir.value());
  if (!mapped_files_dir.has_value()) {
    return 2;
  }

  // external sort
  std::string sorted_file = RandomString(kFilenameLength);  // file with sorted mapped data
  while (std::filesystem::exists(sorted_file)) {
    sorted_file = RandomString(kFilenameLength);
  }
  auto external_sort = boost::process::child(argv[4],
                                             absolute(mapped_files_dir.value()).c_str(),
                                             (std::filesystem::current_path() / sorted_file).c_str());
  external_sort.wait();
  std::filesystem::remove_all(mapped_files_dir.value());
  if (external_sort.exit_code() != 0) {
    std::filesystem::remove(sorted_file);
    return external_sort.exit_code();
  }

  // reduce1
  std::string reduced1_file = RandomString(kFilenameLength);  // file with sorted mapped data
  while (std::filesystem::exists(reduced1_file)) {
    reduced1_file = RandomString(kFilenameLength);
  }
  auto reduce1 = boost::process::child(argv[5],
                                       boost::process::std_in < sorted_file,
                                       boost::process::std_out > reduced1_file);
  reduce1.wait();
  std::filesystem::remove(sorted_file);
  if (reduce1.exit_code() != 0) {
    return reduce1.exit_code();
  }

  // reduce2
  std::string output_file = argv[7];
  if (std::filesystem::exists(output_file)) {
    std::filesystem::resize_file(output_file, 0);
  }
  auto reduce2 = boost::process::child(argv[6],
                                       boost::process::std_in < reduced1_file,
                                       boost::process::std_out > output_file);
  reduce2.wait();
  std::filesystem::remove(reduced1_file);
  if (reduce2.exit_code() != 0) {
    return reduce2.exit_code();
  }
  return 0;
}
