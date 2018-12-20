#ifdef CONSOLE_DEBUG
#include <filesystem>
#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>

#include "LibKraken.h"

namespace filesystem = std::experimental::filesystem;

std::vector<std::pair<filesystem::path, filesystem::path>> GetTests(const filesystem::path& test_path)
{
  auto test_files = std::vector<std::pair<filesystem::path, filesystem::path>>();
  auto test_data_iterator = filesystem::directory_iterator(test_path);

  for (auto& test_file : test_data_iterator)
  {
    const auto& test_file_path = test_file.path();

    if (test_file_path.extension() == ".test") {
      auto solution_path = test_file_path.parent_path() / test_file_path.stem().u8string().append(".expected");

      if (filesystem::exists(solution_path)) {
        test_files.emplace_back(std::make_pair(
          test_file_path,
          solution_path
        ));
      }
    }
  }

  return test_files;
}

bool RunTest(const filesystem::path& test_path, const filesystem::path& expected_path)
{
  if (!filesystem::exists(test_path))
  {
    std::cout << "Missing test file: " << test_path;
    return false;
  }

  if (!filesystem::exists(expected_path))
  {
    std::cout << "Missing expected file: " << expected_path;
    return false;
  }

  const auto test_file_size = filesystem::file_size(test_path);
  auto test_bytes = std::make_unique<uint8_t[]>(test_file_size);
  std::ifstream test_file(test_path.string().c_str(), std::ios::beg | std::ios::binary);
  test_file.read(reinterpret_cast<char *>(test_bytes.get()), test_file_size);
  test_file.close();

  const auto expected_file_size = filesystem::file_size(expected_path);
  auto expected_bytes = std::make_unique<uint8_t[]>(expected_file_size);
  std::ifstream expected_file(expected_path.string().c_str(), std::ios::beg | std::ios::binary);
  expected_file.read(reinterpret_cast<char *>(expected_bytes.get()), expected_file_size);
  expected_file.close();

  auto actual_bytes_size = expected_file_size;
  auto actual_bytes = std::make_unique<uint8_t[]>(actual_bytes_size);

  const auto is_successful = Kraken_DecompressInMemory(test_bytes.get(), test_file_size, actual_bytes.get(), &actual_bytes_size);

  if (is_successful) {
    if (actual_bytes_size != expected_file_size) {
      std::cout << "Failure: Expected " << expected_file_size << " bytes, but got " << actual_bytes_size << std::endl;
      return false;
    }

    for (uintmax_t i = 0; i < actual_bytes_size; ++i) {
      if (actual_bytes[i] != expected_bytes[i]) {
        printf("Failure on byte %llu: Expected %02X Got %02X\n", i, expected_bytes[i], actual_bytes[i]);
        return false;
      }
    }
  }
  else {
    std::cout << "Failure" << std::endl;
    return false;
  }

  std::cout << "Success: decompressed " << test_file_size << " bytes to " << actual_bytes_size << std::endl;
  return true;
}

int main()
{
  auto success = true;

  auto test_files = GetTests("TestData");
  for (const auto& test : test_files) {
    std::cout << test.first.stem() << " - ";

    if (!RunTest(test.first, test.second)) {
      std::cout << "*** Failed ***" << std::endl;
      success = false;
    }
    else {
      std::cout << "Passed" << std::endl;
    }
  }

  if (!success) {
    std::cout << "Failed to pass tests" << std::endl;
  }

  return 0;
}
#endif
