#include <string>
#include <unordered_map>
#include <mutex>
#include <filesystem>
#include "LibKraken.h"

extern int Kraken_Decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len);

static thread_local std::string _errorMessage;

void SetError(const std::string& errorMessage)
{
  _errorMessage = errorMessage;
}

const char *GetError()
{
  return _errorMessage.c_str();
}

bool Kraken_DecompressInMemory(
  _In_ const uint8_t *inCompressedBytes,
  _In_ std::size_t inCompressedSize,
  _Out_ uint8_t *outDecompressedBytes,
  _Inout_ std::size_t *inDecompressedSize)
{
  const auto num_bytes_written = Kraken_Decompress(inCompressedBytes, inCompressedSize, outDecompressedBytes, *inDecompressedSize);
  if (num_bytes_written < 0)
  {
    SetError("Failed to decompress bytes");
    return false;
  }

  *inDecompressedSize = num_bytes_written;

  return true;
}
