#include "exceptions.h"

const char *CustomException::what() const throw() { return msg.c_str(); }
