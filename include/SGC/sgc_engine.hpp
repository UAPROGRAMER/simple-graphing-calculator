#pragma once

#include <SGC/opengl.hpp>
#include <imgui.h>
#include <vector>
#include <SGC/graph.hpp>

class SGCEngine {
 private:
  GLFWwindow* window = nullptr;
  GLuint displayVAO = 0;
  GLuint shaderProgram = 0;

  int windowWidth = 800;
  int windowHeight = 800;

  GLfloat positionX = 0.0;
  GLfloat positionY = 0.0;

  GLfloat zoom = 200.0;

  GLint windowSizeUniformLocation = 0;
  GLint positionUniformLocation = 0;
  GLint zoomUniformLocation = 0;
  GLint sublinePeriodUniformLocation = 0;
  GLint microlinePeriodUniformLocation = 0;
  GLint timeUniformLocation = 0;

  std::vector<Graph> graphs;

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

  void debugCallback(const GLchar* msg);

  void scrollCallback(double offsetX, double offsetY);

  void keyCallback(int key, int scancode, int action, int mods);
};