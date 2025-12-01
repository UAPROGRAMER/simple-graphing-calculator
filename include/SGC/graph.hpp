#pragma once

#include <string>

class Graph {
 public:
  bool isFunctional;
  std::string name;
  std::string body;
  bool isValid = false;
  float r;
  float g;
  float b;
  float thickness;
  bool isVisible = true;

  Graph(bool isFunctional, std::string name, std::string body, float r, float g, float b,
        float thickness);
  
  std::string getGraphShaderPart() const;
};