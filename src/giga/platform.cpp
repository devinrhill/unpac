#include "platform.hpp"

namespace giga {

const char* getPlatformName(Platform _platform) {
	if(_platform == Platform::Unknown) {
		_platform = NATIVE_PLATFORM;
	}

	switch(_platform) {
        case Platform::Unix: {
            return "Unix";
        } case Platform::Posix: {
            return "POSIX";
        } case Platform::Linux: {
            return "Linux";
        } case Platform::Windows: {
            return "Microsoft Windows";
        } case Platform::Mac: {
            return "Apple iMac";
        } case Platform::iPhone: {
            return "Apple iPhone";
        } case Platform::Unknown: {} default: {
            goto unknown;
        }
	}

unknown:
	return "Unknown";
}

} // namespace giga
