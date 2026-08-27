#include "Credentials.h"
#include "ClientHandler.h"

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

    //make sure the username and password are valid (do not contain invalid characters)
    if (!Credentials::validUsername(username) || !Credentials::validPassword(password)) return false;

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

//TOOD: actually encrypt passwords
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

//usernames cannot start with a number or special character
//usernames cannot contain special characters OTHER THAN underscore
//usernames cannot be longer than 24 characters
bool Credentials::validUsername(std::string username)
{
    if (username == "") return false;
    if (username.size() > 24) return false;
    char firstChar = username[0];
    if (!std::isalpha(firstChar)) return false;
    for (int i = 1; i < username.size(); i++)
    {
        if (!(std::isalnum(username[i]) || username[i] == '_')) return false;
    }

    return !Credentials::usernameTaken(username);
}

//passwords can contain any normal character other than space
//passwords must be at least 8 characters and at most 64
//all password chars must be between '!' and '~' (inclusive) on the ascii chart, but may not contain spaces
bool Credentials::validPassword(std::string password)
{
    if (password.size() < 4 || password.size() > 64) return false;
    for (char c : password)
    {
        if (c < '!' || c > '~' || c == ' ') return false;
    }
    return true;
}