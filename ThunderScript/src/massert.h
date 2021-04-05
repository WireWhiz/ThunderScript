#pragma once
#include <iostream>
#include <exception>
#include <string>

#ifdef NDEBUG

#define assert(expression) ((void)0)

#else
class assertException : public std::exception
{
    virtual const char* what() const throw()
    {
        return "An assert has fired";
    }
} assertExcept;
void _assert(bool expression, const char* file, int line)
{
    if (!expression)
    {
        std::cout << "Assert failed!" << std::endl;
        std::cout << "On line: " << line << std::endl;
        std::cout << "In file: " << file;
    }
}
void _assert(bool expression, const char* file, int line, std::string message)
{
    if (!expression)
    {
        std::cout << "Assert failed with message: " << message << std::endl;
        std::cout << "On line: " << line << std::endl;
        std::cout << "In file: " << file;
    }
}
void _assert(bool expression, const char* file, int line, char* message)
{
    if (!expression)
    {
        std::cout << "Assert failed with message: " << message << std::endl;
        std::cout << "On line: " << line << std::endl;
        std::cout << "In file: " << file;
    }
}
void _assert(bool expression, const char* file, int line, const char* message)
{
    if (!expression)
    {
        std::cout << "Assert failed with message: " << message << std::endl;
        std::cout << "On line: " << line << std::endl;
        std::cout << "In file: " << file;
    }
}

#define assert(expression) _assert(expression, __FILE__, __LINE__); if(!(expression)) throw assertExcept
#define massert(expression, message) _assert(expression, __FILE__, __LINE__, message); if(!(expression)) throw assertExcept
#endif

