#ifndef SUCCESS_H
#define SUCCESS_H

// Failure records the circumstances surrounding a test failure.  Using C++
// macros were are able to record the name of the file where the failure
// occurred, the line number, and the text of the condition which provoked
// the failure.

#include <iostream>
#include <string>

class Success {
public:
  std::string testName;

  Success(std::string testName) : testName(testName) {}
  std::string serialize() const;
};

#endif
