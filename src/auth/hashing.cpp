#include "hashing.h"
#include <unordered_map>
#include <cctype>

// FOR DEBUGGING PURPOSE
//  #include <iostream>

const std::string default_str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_+{}[]";
static std::unordered_map<char, int> char_index;
const int defstrlen = 79;
const int maxstrlen = 23;
const int k1 = 11, k2 = 27, k2_inv = 41; // 27 * 41 ≡ 1 (mod 79)

void initCharMap()
{
    char_index.clear();
    for (int i = 0; i < defstrlen; ++i)
        char_index[default_str[i]] = i;
}

// Checking ======================================================================================================================
bool isValidUsername(const std::string &s)
{
    if (s.size() < 3 || s.size() > 20)
        return false;
    for (char c : s)
        if (!(std::isalnum(c) || c == '@' || c == '!' || c == '$' || c == '#' || c == '%' || c == '&' || c == '*' || c == '_'))
            return false;
    return true;
}

bool isValidPassword(const std::string &s)
{
    if (s.size() < 6 || s.size() > 20)
        return false;
    for (char c : s)
        if (!(std::isalnum(c) || c == '@' || c == '!' || c == '$' || c == '#' || c == '%' || c == '&' || c == '*' || c == '_'))
            return false;
    return true;
}
//======================================================================================================================================


// Padding the character from max 20 to 23 chars
std::string padString(const std::string &str, int len)
{
    if ((int)str.size() >= len - 2)
        return str.substr(0, len - 2) + std::string(1, '0' + str.size() / 10) + std::string(1, '0' + str.size() % 10);
    std::string t = str;
    for (int i = (int)t.size(); i < len - 2; ++i)
        t += default_str[(i * i) % defstrlen];
    t += ('0' + str.size() / 10);
    t += ('0' + str.size() % 10);
    return t;
}


//================ ENCYRPTION==================
std::string encryptString(const std::string &s)
{
    std::string r = s;
    for (size_t i = 0; i < s.size(); ++i)
    {
        auto it = char_index.find(s[i]);
        if (it != char_index.end())
        {
            int idx = (it->second * k2 + k1) % defstrlen;
            r[i] = default_str[idx];
        }
    }
    return r;
}
//=============== DECRYPTION==================
std::string decryptString(const std::string &s)
{
    if (s.size() < 2)
        return "";
    std::string r = s;

    // First decrypt all characters INCLUDING the last 2 (length suffix)
    for (size_t i = 0; i < s.size(); ++i)
    {
        auto it = char_index.find(s[i]);
        if (it != char_index.end())
        {
            int y = it->second - k1;
            if (y < 0)
                y += defstrlen;
            int idx = (k2_inv * y) % defstrlen;
            r[i] = default_str[idx];
        }
    }

    // extract length from DECRYPTED string
    int orig_len = (r[r.size() - 2] - '0') * 10 + (r[r.size() - 1] - '0');
    if (orig_len < 0 || orig_len > (int)r.size() - 2)
        return "";
    return r.substr(0, orig_len);
}

// FOR DEBUGGING PURPOSE
//  int main(){
//      std::string s;
//      std::cin >> s;
//      std::cout<< encryptString(padString(s, 23)) << "\n";
//      std::cout<< decryptString(encryptString(padString(s, 23)))<<"\n";
//  }