#include <variable.hpp>

Variable::Variable(std::string name, float maxValue, float minValue, float step,
    float value)
    : name(std::move(name)), maxValue(maxValue), minValue(minValue),
    step(step), value(value) {}

std::string Variable::getVariableShaderPart() const {
  return "uniform float " + std::string(name) + ";";
}

bool isValidId(std::string id) {
  for (char c : id)
    if (!std::isalnum(c) && c != '_')
      return false;
  return true;
}