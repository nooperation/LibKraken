#pragma once

#define DllExport   __declspec( dllexport ) 

extern "C"
{
  DllExport bool Kraken_DecompressInMemory(
    _In_ const uint8_t *inCompressedBytes,
    _In_ std::size_t inCompressedSize,
    _Out_ uint8_t *outDecompressedBytes,
    _Inout_ std::size_t *inDecompressedSize
  );

  DllExport const char *GetError();
}
