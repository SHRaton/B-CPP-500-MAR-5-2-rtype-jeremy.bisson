#include <chrono>
#include <string>

class Command {
public:
    std::string message;
    std::chrono::steady_clock::duration relative_time; // Durée relative à launch_time_

    // Méthodes utilitaires pour sérialisation
    long long getRelativeTimeMillis() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(relative_time).count();
    }

    void setRelativeTimeFromMillis(long long millis) {
        relative_time = std::chrono::milliseconds(millis);
    }
};