#include <SGC/sgc.hpp>
#include <iostream>

int main() {
  try {
    SGCEngine().run();
  } catch (const SGCError& e) {
    std::cerr << e.msg;
    return e.errorCode;
  } catch (const std::exception& e) {
    std::cerr << "[SGCEngine]: Unexpected error: " << e.what() << "\n";
    return static_cast<int>(SGCErrorType::UNKNOWN_ERROR);
  } catch (...) {
    std::cerr << "[SGCEngine]: Unknown error.\n";
    return static_cast<int>(SGCErrorType::UNKNOWN_ERROR);
  }

  return 0;
}