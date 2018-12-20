#pragma once

#include <memory>

#define DllExport   __declspec( dllexport ) 

extern "C"
{
  DllExport bool ConvertDdsInMemory(
    _In_ const uint8_t *inCompressedBytes,
    _In_ std::size_t inCompressedSize,
    _Out_opt_ uint8_t **outBuff,
    _Out_opt_ std::size_t *outBuffSize
  );

  DllExport const char *GetError();
  DllExport void FreeMemory(_In_ const uint8_t *data);
}
