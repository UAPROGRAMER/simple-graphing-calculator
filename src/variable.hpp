#pragma once

#include <string>

class Variable {
public:
  std::string name;
  float maxValue;
  float minValue;
  float step;
  float value;

  Variable(std::string name, float maxValue, float minValue, float step,
      float value);

  std::string getVariableShaderPart() const;
};

bool isValidId(std::string id);