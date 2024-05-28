#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

const double kUsersMultiplier = 1.5;
const double kSitesMultiplier = 2;

const int kMaxSiteLength = 10;
const uint64_t kMaxTime = 100;

const std::string kChars = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string RandomString(int length) {
  std::string string;
  string.reserve(length);

  while(length > 0) {
    string += kChars[rand() % kChars.size()];
    --length;
  }
  return string;
}

std::vector<std::string> GetSites(uint64_t sites_number) {
  std::vector<std::string> sites(sites_number);
  for (auto& site : sites) {
    int site_length = rand() % (kMaxSiteLength) + 1;
    site = RandomString(site_length);
  }
  return sites;
}

int main(int argc, char** argv) {
  // argv[1] - "-n" / "-d"
  // argv[2] - strings number / filename
  // argv[3] - "-d" / "-n"
  // argv[4] - filename / strings number
  srand(time(nullptr));

  if (argc != 5) {
    std::cerr << "Invalid arguments number\n";
    return 1;
  }
  if (!(std::strcmp(argv[1], "-n") == 0 && std::strcmp(argv[3], "-d") == 0) &&
      !(std::strcmp(argv[1], "-d") == 0 && std::strcmp(argv[3], "-n") == 0)) {
    std::cerr << "Invalid format\n";
    return 2;
  }

  uint64_t strings_number;
  std::string filename;

  if (std::strcmp(argv[1], "-n") == 0) {
    strings_number = std::stoull(argv[2]);
    filename = argv[4];
  } else {
    strings_number = std::stoull(argv[4]);
    filename = argv[2];
  }

  uint64_t users_number = std::ceil(kUsersMultiplier * std::log(strings_number));
  uint64_t sites_number = std::ceil(kSitesMultiplier * std::log(strings_number));
  auto sites = GetSites(sites_number);

  std::ofstream write(filename);
  for (uint64_t i = 0; i < strings_number; ++i) {
    uint64_t site_number = rand() % sites_number;
    uint64_t user_id = rand() % users_number + 1;
    uint64_t time = rand() % kMaxTime + 1;
    write << sites[site_number] << '\t' << user_id << '#' << time << '\n';
  }
  return 0;
}
