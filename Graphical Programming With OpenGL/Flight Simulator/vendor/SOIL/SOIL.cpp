#include "SOIL/SOIL.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <objbase.h>
#include <wincodec.h>
#endif

namespace {
std::string g_lastResult = "SOIL compatibility loader not used yet.";

#if defined(_WIN32)
std::wstring ToWideString(const char* filename)
{
    if (filename == nullptr)
    {
        return {};
    }

    const int length = MultiByteToWideChar(CP_ACP, 0, filename, -1, nullptr, 0);
    if (length <= 0)
    {
        return {};
    }

    std::vector<wchar_t> buffer(static_cast<std::size_t>(length));
    if (MultiByteToWideChar(CP_ACP, 0, filename, -1, buffer.data(), length) <= 0)
    {
        return {};
    }

    return std::wstring(buffer.data());
}

bool TryLoadImageWithWic(
    const char* filename,
    int* width,
    int* height,
    int* channels,
    int force_channels,
    unsigned char** outputData
)
{
    if (outputData == nullptr)
    {
        return false;
    }

    const std::wstring wideFilename = ToWideString(filename);
    if (wideFilename.empty())
    {
        return false;
    }

    const HRESULT initResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool shouldUninitialize = initResult == S_OK || initResult == S_FALSE;
    const bool comAvailable = shouldUninitialize || initResult == RPC_E_CHANGED_MODE;
    if (!comAvailable)
    {
        return false;
    }

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    bool success = false;

    if (SUCCEEDED(CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            reinterpret_cast<void**>(&factory))) &&
        factory != nullptr &&
        SUCCEEDED(factory->CreateDecoderFromFilename(
            wideFilename.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder)) &&
        decoder != nullptr &&
        SUCCEEDED(decoder->GetFrame(0, &frame)) &&
        frame != nullptr &&
        SUCCEEDED(factory->CreateFormatConverter(&converter)) &&
        converter != nullptr &&
        SUCCEEDED(converter->Initialize(
            frame,
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom)))
    {
        UINT imageWidth = 0;
        UINT imageHeight = 0;

        if (SUCCEEDED(frame->GetSize(&imageWidth, &imageHeight)) && imageWidth > 0 && imageHeight > 0)
        {
            const std::size_t pixelCount = static_cast<std::size_t>(imageWidth) * static_cast<std::size_t>(imageHeight);
            std::vector<unsigned char> rgbaData(pixelCount * 4);
            const UINT stride = imageWidth * 4;

            if (SUCCEEDED(converter->CopyPixels(nullptr, stride, static_cast<UINT>(rgbaData.size()), rgbaData.data())))
            {
                const int outputChannels = force_channels == 3 ? 3 : 4;
                auto* loadedData = static_cast<unsigned char*>(std::malloc(pixelCount * static_cast<std::size_t>(outputChannels)));

                if (loadedData != nullptr)
                {
                    for (std::size_t index = 0; index < pixelCount; ++index)
                    {
                        loadedData[index * outputChannels + 0] = rgbaData[index * 4 + 0];
                        loadedData[index * outputChannels + 1] = rgbaData[index * 4 + 1];
                        loadedData[index * outputChannels + 2] = rgbaData[index * 4 + 2];

                        if (outputChannels == 4)
                        {
                            loadedData[index * outputChannels + 3] = rgbaData[index * 4 + 3];
                        }
                    }

                    if (width != nullptr)
                    {
                        *width = static_cast<int>(imageWidth);
                    }

                    if (height != nullptr)
                    {
                        *height = static_cast<int>(imageHeight);
                    }

                    if (channels != nullptr)
                    {
                        *channels = outputChannels;
                    }

                    *outputData = loadedData;
                    success = true;
                }
                else
                {
                    g_lastResult = "SOIL compatibility loader failed to allocate WIC output buffer.";
                }
            }
        }
    }

    if (converter != nullptr)
    {
        converter->Release();
    }

    if (frame != nullptr)
    {
        frame->Release();
    }

    if (decoder != nullptr)
    {
        decoder->Release();
    }

    if (factory != nullptr)
    {
        factory->Release();
    }

    if (shouldUninitialize)
    {
        CoUninitialize();
    }

    if (success)
    {
        g_lastResult = "Loaded image successfully via WIC.";
    }

    return success;
}
#endif

bool ReadNextToken(std::istream& stream, std::string& token)
{
    token.clear();

    char character = '\0';
    while (stream.get(character))
    {
        if (character == '#')
        {
            std::string discarded;
            std::getline(stream, discarded);
            continue;
        }

        if (!std::isspace(static_cast<unsigned char>(character)))
        {
            token.push_back(character);
            break;
        }
    }

    if (token.empty())
    {
        return false;
    }

    while (stream.get(character))
    {
        if (character == '#')
        {
            std::string discarded;
            std::getline(stream, discarded);
            break;
        }

        if (std::isspace(static_cast<unsigned char>(character)))
        {
            break;
        }

        token.push_back(character);
    }

    return true;
}

int ParseInt(const std::string& value)
{
    return std::stoi(value);
}

unsigned char* ExpandChannels(const std::vector<unsigned char>& data, int width, int height, int targetChannels)
{
    const int pixelCount = width * height;
    auto* output = static_cast<unsigned char*>(std::malloc(static_cast<std::size_t>(pixelCount * targetChannels)));

    if (output == nullptr)
    {
        g_lastResult = "SOIL compatibility loader failed to allocate output buffer.";
        return nullptr;
    }

    for (int index = 0; index < pixelCount; ++index)
    {
        output[index * targetChannels + 0] = data[index * 3 + 0];
        output[index * targetChannels + 1] = data[index * 3 + 1];
        output[index * targetChannels + 2] = data[index * 3 + 2];

        if (targetChannels == 4)
        {
            output[index * targetChannels + 3] = 255;
        }
    }

    return output;
}
}

extern "C" unsigned char* SOIL_load_image(
    const char* filename,
    int* width,
    int* height,
    int* channels,
    int force_channels
)
{
    if (filename == nullptr)
    {
        g_lastResult = "SOIL compatibility loader received a null filename.";
        return nullptr;
    }

#if defined(_WIN32)
    unsigned char* wicLoadedData = nullptr;
    if (TryLoadImageWithWic(filename, width, height, channels, force_channels, &wicLoadedData))
    {
        return wicLoadedData;
    }
#endif

    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open())
    {
        g_lastResult = "SOIL compatibility loader could not open image file.";
        return nullptr;
    }

    std::string magic;
    std::string widthToken;
    std::string heightToken;
    std::string maxValueToken;

    if (!ReadNextToken(stream, magic) || !ReadNextToken(stream, widthToken) || !ReadNextToken(stream, heightToken) || !ReadNextToken(stream, maxValueToken))
    {
        g_lastResult = "SOIL compatibility loader could not parse the PPM header.";
        return nullptr;
    }

    const int parsedWidth = ParseInt(widthToken);
    const int parsedHeight = ParseInt(heightToken);
    const int maxValue = ParseInt(maxValueToken);

    if (parsedWidth <= 0 || parsedHeight <= 0 || maxValue <= 0 || maxValue > 255)
    {
        g_lastResult = "SOIL compatibility loader only supports 8-bit PPM images.";
        return nullptr;
    }

    std::vector<unsigned char> rgbData(static_cast<std::size_t>(parsedWidth * parsedHeight * 3));

    if (magic == "P6")
    {
        while (std::isspace(static_cast<unsigned char>(stream.peek())))
        {
            stream.get();
        }

        stream.read(reinterpret_cast<char*>(rgbData.data()), static_cast<std::streamsize>(rgbData.size()));
        if (stream.gcount() != static_cast<std::streamsize>(rgbData.size()))
        {
            g_lastResult = "SOIL compatibility loader could not read all P6 pixel data.";
            return nullptr;
        }
    }
    else if (magic == "P3")
    {
        for (std::size_t index = 0; index < rgbData.size(); ++index)
        {
            std::string componentToken;
            if (!ReadNextToken(stream, componentToken))
            {
                g_lastResult = "SOIL compatibility loader could not read all P3 pixel data.";
                return nullptr;
            }

            rgbData[index] = static_cast<unsigned char>(ParseInt(componentToken));
        }
    }
    else
    {
        g_lastResult = "SOIL compatibility loader currently supports only P3/P6 PPM images.";
        return nullptr;
    }

    const int outputChannels = force_channels == 4 ? 4 : 3;
    unsigned char* output = ExpandChannels(rgbData, parsedWidth, parsedHeight, outputChannels);
    if (output == nullptr)
    {
        return nullptr;
    }

    if (width != nullptr)
    {
        *width = parsedWidth;
    }

    if (height != nullptr)
    {
        *height = parsedHeight;
    }

    if (channels != nullptr)
    {
        *channels = outputChannels;
    }

    g_lastResult = "Loaded image successfully.";
    return output;
}

extern "C" void SOIL_free_image_data(unsigned char* img_data)
{
    std::free(img_data);
}

extern "C" const char* SOIL_last_result(void)
{
    return g_lastResult.c_str();
}