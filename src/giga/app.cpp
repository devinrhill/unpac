#include "app.hpp"

namespace giga {

giga::App::App(int argc, char** argv): _argc{argc}, _argv{argv} {

}

int App::run() {
    return _exitValue;
}

// Getters
int App::getArgc() const noexcept {
    return _argc;
}

const char** App::getArgv() const noexcept {
    return const_cast<const char**>(_argv);
}

bool App::isRunning() const noexcept {
    return _isRunning;
}

int App::getExitValue() const noexcept {
    return _exitValue;
}

} // namespace giga
