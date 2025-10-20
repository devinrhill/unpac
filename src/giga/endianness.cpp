#include "endianness.hpp"

namespace giga {

const char* getEndiannessName(Endianness endianness) {
	if(endianness == Endianness::Unknown) {
		endianness = NATIVE_ENDIANNESS;
	}

	switch(endianness) {
	    case Endianness::Little: {
		    return "Little";
        } case Endianness::Big: {
		    return "Big";
        } default: {
		    goto unknown_endianness;
        }
	}

unknown_endianness:
	return "Unknown";
}

} // namespace giga
