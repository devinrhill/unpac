#ifndef GIGA_PLATFORM_H
#define GIGA_PLATFORM_H

#ifdef __LP64__
#define IS_64_BIT
#else
#define IS_32_BIT
#endif

#if (defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER))
#define IS_WINDOWS
#endif

namespace giga {

enum class Platform {
	Unknown = 0,
	Unix = 1,
	Posix = 2,
	Linux = 3,
	Windows = 4,
	Mac = 5,
	iPhone = 6
};

#if defined(__linux__)
constexpr Platform NATIVE_PLATFORM = Platform::Linux;
#elif defined(__unix__)
constexpr Platform NATIVE_PLATFORM = Platform::Unix;
#elif defined(_POSIX_VERSION)
constexpr Platform NATIVE_PLATFORM = Platform::Posix;
#elif IS_WINDOWS
constexpr Platform NATIVE_PLATFORM = Platform::Windows;
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
constexpr Platform NATIVE_PLATFORM = Platform::Mac;
#elif TARGET_OS_IPHONE
constexpr Platform NATIVE_PLATFORM = Platform::iPhone;
#endif
#else
constexpr Platform NATIVE_PLATFORM = Platform::Unknown;
#endif

const char* getPlatformName(Platform _platform = Platform::Unknown);

#if IS_WINDOWS
constexpr const char* DIRECTORY_SEPARATOR = "\\";
#else
constexpr const char* DIRECTORY_SEPARATOR = "/";
#endif

} // namespace giga

#endif // GIGA_PLATFORM_H
