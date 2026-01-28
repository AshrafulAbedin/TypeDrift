// hashing.h
#ifndef HASHING_H
#define HASHING_H

#include <string>

bool isValidUsername(const std::string& username);
bool isValidPassword(const std::string& password);

 std::string padString(const std::string& str, int targetLen = 23);
 std::string encryptString(const std::string& str);
 std::string decryptString(const std::string& str);

void initCharMap();

#endif