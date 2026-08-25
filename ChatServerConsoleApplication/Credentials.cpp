#include "Credentials.h"

void Credentials::init()
{
	if (Credentials::initialized) return;
    Credentials::initialized = true;
	Credentials::userCredentials = Credentials::loadLogins();
}


std::unordered_map<std::string, std::string> Credentials::loadLogins()
{

    std::unordered_map<std::string, std::string> map;
    if (!Credentials::initialized) return map;
    std::ifstream file(Credentials::saveFileName);

    if (!file.is_open()) return map;
    std::string key, value;
    while (file >> key >> value) {
        map[key] = value;
    }
    return map;
}

bool Credentials::saveLogins()
{
    if (!Credentials::initialized) return false;
    std::ofstream file(Credentials::saveFileName);
    if (!file.is_open()) return false;

    for (const auto& [key, value] : Credentials::userCredentials)
    {
        file << key << " " << value << "\n";
    }

    return true;
}

bool Credentials::addNew(std::string username, std::string password)
{ 
    if (!Credentials::initialized) return false;
    //check to make sure username is not taken
    if (Credentials::usernameTaken(username)) return false;

    //encrypt the password
    std::string encryptedPassword = Credentials::encryptPassword(password);

    //append the new login to the save file
    std::ofstream file(Credentials::saveFileName, std::ios_base::app);
    if (!file.is_open()) return false;
    file << username << " " << encryptedPassword << "\n";

    //add the new login to the map
    Credentials::userCredentials[username] = encryptedPassword;
    return true;
}

bool Credentials::usernameTaken(std::string username)
{
    if (!Credentials::initialized) return false;
    return Credentials::userCredentials.contains(username);
}

std::string Credentials::encryptPassword(std::string password)
{
    std::string encryptedPassoword = password;
    return encryptedPassoword;
}

bool Credentials::verifyLogin(std::string username, std::string password)
{
    if (!Credentials::initialized) return false;
    std::string encryptedPassword = Credentials::encryptPassword(password);
    auto iter = Credentials::userCredentials.find(username);
    if (iter == Credentials::userCredentials.end()) return false;
    return iter->second == encryptedPassword;
}