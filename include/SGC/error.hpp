#pragma once

#include <exception>
#include <string>

enum class SGCErrorType : int {
  UNKNOWN_ERROR,
  ENGINE_ERROR,
  GLFW_ERROR,
  GLAD_ERROR,
  OPENGL_ERROR,
};

class SGCError : std::exception {
 public:
  int errorCode;
  std::string msg;

  SGCError(SGCErrorType errorCode, std::string&& msg);

  const char* what() const noexcept override;
};