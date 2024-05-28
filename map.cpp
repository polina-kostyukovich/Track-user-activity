#include <iostream>

int main() {
  std::string string;

  while (true) {
    std::getline(std::cin, string);
    if (string.empty()) break;

    uint64_t user_id_pos = string.find('\t') + 1;
    uint64_t time_pos = string.find('#', user_id_pos) + 1;

    std::string_view website_name = {string.c_str(), user_id_pos - 1};
    std::string_view user_id = {string.c_str() + user_id_pos, time_pos - user_id_pos - 1};
    std::string_view time = {string.c_str() + time_pos};

    std::cout << user_id << '\t' << website_name << '$' << time << '$'
              << time << '$' << time << "$1\n";
  }
  return 0;
}
