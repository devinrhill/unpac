#ifndef GIGA_APP_H
#define GIGA_APP_H

namespace giga {

class App {
public:
    App(int argc, char** argv);

    // Main-loop start/stop and run
    virtual int run() = 0;

    // Getters
    int getArgc() const noexcept;
    const char** getArgv() const noexcept;
    bool isRunning() const noexcept;
    int getExitValue() const noexcept;
private:
    // Main-loop core operations
    virtual void input() = 0; // Handle user-input
    virtual void update() = 0; // Update variables, behavior, etc.
    virtual void draw() = 0; // Draw / render

    int _argc;
    char** _argv;
    bool _isRunning = false;
    int _exitValue = 0;
};

} // namespace giga

#endif // GIGA_APP_H
