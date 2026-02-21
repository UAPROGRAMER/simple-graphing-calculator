#pragma once

#include <imgui.h>

#include <vector>

#include "graph.hpp"
#include "opengl.hpp"
#include "variable.hpp"

class SGCEngine {
private:
  GLFWwindow* window = nullptr;
  GLuint displayVAO = 0;
  GLuint shaderProgram = 0;

  int windowWidth = 800;
  int windowHeight = 800;

  GLfloat positionX = 0.000001;
  GLfloat positionY = 0.000001;

  GLfloat zoom = 200.0;

  GLint windowSizeUniformLocation = 0;
  GLint positionUniformLocation = 0;
  GLint zoomUniformLocation = 0;
  GLint sublinePeriodUniformLocation = 0;
  GLint microlinePeriodUniformLocation = 0;
  GLint timeUniformLocation = 0;

  std::vector<Graph> graphs;
  std::vector<Variable> variables;

  bool makeShaderProgram();

  void process();

  void processGUI();

  void draw();

public:
  SGCEngine(const SGCEngine&) = delete;
  SGCEngine& operator=(const SGCEngine&) = delete;
  SGCEngine(SGCEngine&&) = delete;
  SGCEngine& operator=(SGCEngine&&) = delete;

  SGCEngine();

  ~SGCEngine();

  void run();

  void windowSizeCallback(int width, int height);

  void scrollCallback(double offsetX, double offsetY);

  void keyCallback(int key, int scancode, int action, int mods);
};