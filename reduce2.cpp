#include <iostream>

int main() {
  std::string string;
  std::string current_user_id;

  while (true) {
    std::getline(std::cin, string);
    if (string.empty()) break;
    uint64_t user_id_length = string.find('\t');
    uint64_t site_length = string.find('$', user_id_length + 1);
    uint64_t min_time_end = string.find('$', site_length + 1);
    uint64_t max_time_end = string.find('$', min_time_end + 1);
    uint64_t total_time_end = string.find('$', max_time_end + 1);

    uint64_t total_time = std::stoull(string.substr(max_time_end + 1, total_time_end - max_time_end - 1));
    uint64_t visits_number = std::stoull(string.substr(total_time_end + 1, string.size() - total_time_end - 1));
    double average_time = static_cast<double>(total_time) / visits_number;

    std::string_view user_id = {string.c_str(), user_id_length};
    std::string_view other_info = {string.c_str() + user_id_length + 1, max_time_end - user_id_length - 1};

    if (user_id == current_user_id) {
      std::cout << '#' << other_info << '$' << average_time;
    } else {
      if (!current_user_id.empty()) {
        std::cout << '\n';
      }
      std::cout << user_id << '\t' << other_info << '$' << average_time;
      current_user_id = user_id;
    }
  }
  std::cout << '\n';
  return 0;
}
