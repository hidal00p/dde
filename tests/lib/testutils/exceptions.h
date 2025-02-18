#ifndef TU_EXCEPTIONS_H
#define TU_EXCEPTIONS_H

#include <exception>
#include <string>

class CustomException : public std::exception {
private:
  std::string msg;

public:
  CustomException(std::string msg) : msg(msg) { std::exception(); }
  const char *what() const throw();
};

class StackMisuseException : CustomException {
  using CustomException::CustomException;
};

class NodeExpectedException : CustomException {
  using CustomException::CustomException;
};

#endif
