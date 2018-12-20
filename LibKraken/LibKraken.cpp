#include <string>
#include <unordered_map>
#include <mutex>
#include <filesystem>

extern int Kraken_Decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len);

enum class MemoryType {
  kMemoryType_Single,
  kMemoryType_Array
};

std::mutex memoryMapMutex;
std::unordered_map<const uint8_t*, MemoryType> memoryMap;
static thread_local std::string _errorMessage;

void FreeMemory(_In_ const uint8_t *data)
{
  std::lock_guard<std::mutex> lock(memoryMapMutex);

  auto memoryIter = memoryMap.find(data);
  if (memoryIter != memoryMap.end())
  {
    if (memoryIter->second == MemoryType::kMemoryType_Array)
    {
      delete[] memoryIter->first;
    }
    else
    {
      delete memoryIter->first;
    }
    memoryMap.erase(memoryIter);
  }
}

void SetError(const std::string& errorMessage)
{
  _errorMessage = errorMessage;
}

const char *GetError()
{
  return _errorMessage.c_str();
}

bool ConvertCrnInMemory(
  _In_ const uint8_t *inCompressedBytes,
  _In_ std::size_t inCompressedSize,
  _Out_opt_ uint8_t **outDecompressedBytes,
  _Out_opt_ std::size_t* outDecompressedSize)
{
  auto out_buff = new uint8_t[128];
  auto out_buff_size = 128;

  *outDecompressedSize = Kraken_Decompress(inCompressedBytes + 4, inCompressedSize - 4, *outDecompressedBytes, *outDecompressedSize);

  {
    std::lock_guard<std::mutex> lock(memoryMapMutex);
    memoryMap[out_buff] = MemoryType::kMemoryType_Array;
  }

  return true;
}

#ifdef CONSOLE_DEBUG
#include <filesystem>
#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
  static const std::string kInputFile = "test1.bin";

  if (!std::experimental::filesystem::exists(kInputFile))
  {
    printf("Missing file\n");
    return 1;
  }

  std::experimental::filesystem::path crn_path(kInputFile);
  const auto file_size = std::experimental::filesystem::file_size(crn_path);

  auto *crn_bytes = new uint8_t[file_size];
  std::ifstream in_file(crn_path.string().c_str(), std::ios::beg | std::ios::binary);
  in_file.read(reinterpret_cast<char *>(crn_bytes), file_size);
  in_file.close();

  uint8_t *out_buff;
  std::size_t out_buff_size = 0x560C;
  ConvertCrnInMemory(crn_bytes, file_size, &out_buff, &out_buff_size);
  delete[] crn_bytes;

  std::ofstream out_file("out.dds", std::ios::binary);
  out_file.write(reinterpret_cast<char *>(out_buff), out_buff_size);
  out_file.close();

  FreeMemory(out_buff);

  return 0;
}
#endif