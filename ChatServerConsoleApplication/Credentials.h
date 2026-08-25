#pragma once
#include <unordered_map>
#include <fstream>
#include <string>

class Credentials
{
private:
	static inline const std::string saveFileName = "logins.data";
	static inline std::unordered_map<std::string, std::string> userCredentials;
	static inline bool initialized = false;

	static bool saveLogins();
	static std::unordered_map<std::string, std::string> loadLogins();

	static std::string encryptPassword(std::string password);

public:
	static void init();
	static bool addNew(std::string username, std::string password);
	static bool verifyLogin(std::string username, std::string password);
	static bool usernameTaken(std::string username);
};

