#include <iostream>
#include <string>
#include <vector>

#if defined(__APPLE__) || defined(__linux__)
    #include <fstream>
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace env_var {
// if apple or linux
#if defined(__APPLE__) || defined(__linux__)
    inline bool addToPath(const std::string &newPath) {
        std::string homePath = getenv("HOME");
        if (homePath.empty()) {
            std::cerr << "Failed to get HOME directory." << '\n';
            return false;
        }

        std::string profilePath = homePath + "/.bash_profile";
        std::ofstream file(profilePath, std::ios_base::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open .bash_profile." << '\n';
            return false;
        }

        file << "export PATH=\"$PATH:" << newPath << "\"" << '\n';
        if (!file.good()) {
            std::cerr << "Failed to write to .bash_profile." << '\n';
            return false;
        }

        file.close();
        std::cout << "PATH updated successfully in .bash_profile" << '\n';
        return true;
    }
#elif defined(_WIN32) || defined(_WIN64)
    bool addToUserPath(const std::string &newPath) {
        HKEY hKey;
        const char *userEnv = "Environment";

        // Open the key with write access
        if (RegOpenKeyEx(HKEY_CURRENT_USER, userEnv, 0, KEY_READ | KEY_WRITE,
                         &hKey) != ERROR_SUCCESS) {
            std::cerr << "Failed to open registry key." << '\n';
            return false;
        }

        // Determine the size of the current PATH
        DWORD size = 0;
        if (RegQueryValueEx(hKey, "Path", NULL, NULL, NULL, &size) !=
            ERROR_SUCCESS) {
            std::cerr << "Failed to query size of PATH." << '\n';
            RegCloseKey(hKey);
            return false;
        }

        // Allocate buffer for the current PATH
        std::vector<char> value(size);

        // Read the current PATH
        if (RegQueryValueEx(hKey, "Path", NULL, NULL,
                            reinterpret_cast<BYTE *>(value.data()),
                            &size) != ERROR_SUCCESS) {
            std::cerr << "Failed to read current PATH." << '\n';
            RegCloseKey(hKey);
            return false;
        }

        // Check if the new path is already in the PATH
        std::string currentPath(value.begin(), value.end());
        if (currentPath.find(newPath) != std::string::npos) {
            std::cout << "The new path is already in the PATH." << '\n';
        } else {
            // Append the new path
            currentPath += (currentPath.back() == ';' ? "" : ";") + newPath;

            // Write the new PATH back to the registry
            if (RegSetValueEx(
                    hKey, "Path", 0, REG_EXPAND_SZ,
                    reinterpret_cast<const BYTE *>(currentPath.c_str()),
                    currentPath.length() + 1) != ERROR_SUCCESS) {
                std::cerr << "Failed to write new PATH to the registry."
                          << '\n';
                RegCloseKey(hKey);
                return false;
            }

            std::cout << "PATH updated successfully." << '\n';
        }

        // Close the registry key
        RegCloseKey(hKey);

        // Broadcast a message to all windows to inform them of the environment
        // change
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                           reinterpret_cast<LPARAM>("Environment"),
                           SMTO_ABORTIFHUNG, 5000, NULL);

        return true;
    }
#endif
}  // namespace env_var
