#include <iostream>

struct Info {
  std::string id_site;
  uint64_t min_time;
  uint64_t max_time;
  uint64_t total_time;
  uint64_t visits_number;

  [[nodiscard]] std::string ToString() const {
    return id_site + '$' + std::to_string(min_time) + '$'
        + std::to_string(max_time) + '$' + std::to_string(total_time) + '$'
        + std::to_string(visits_number);
  }
};

int main() {
  std::string string;
  Info result;

  while (true) {
    std::getline(std::cin, string);
    if (string.empty()) break;

    uint64_t id_site_length = string.find('$');
    uint64_t min_time_end = string.find('$', id_site_length + 1);
    uint64_t max_time_end = string.find('$', min_time_end + 1);
    uint64_t total_time_end = string.find('$', max_time_end + 1);

    Info string_info = {
        string.substr(0, id_site_length),
        std::stoull(string.substr(id_site_length + 1, min_time_end - id_site_length - 1)),
        std::stoull(string.substr(min_time_end + 1, max_time_end - min_time_end - 1)),
        std::stoull(string.substr(max_time_end + 1, total_time_end - max_time_end - 1)),
        std::stoull(string.substr(total_time_end + 1, string.size() - total_time_end - 1))
    };

    if (string_info.id_site == result.id_site) {
      if (string_info.min_time < result.min_time) {
        result.min_time = string_info.min_time;
      }

      if (string_info.max_time > result.max_time) {
        result.max_time = string_info.max_time;
      }

      result.total_time += string_info.total_time;
      result.visits_number += string_info.visits_number;
    } else {
      if (!result.id_site.empty()) {
        std::cout << result.ToString() << '\n';
      }
      result = std::move(string_info);
    }
  }

  if (!result.id_site.empty()) {
    std::cout << result.ToString() << '\n';
  }
  return 0;
}
