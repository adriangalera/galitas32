#include <cstdio>
#include <Logger.h>

class ConsoleLogger final : public Logger {
public:
    void debug(const char* message) override {
        std::printf("%s\n", message);
    }
};