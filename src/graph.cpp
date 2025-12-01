#include <SGC/graph.hpp>

Graph::Graph(bool isFunctional, std::string name, std::string body, float r,
             float g, float b, float thickness)
    : isFunctional(isFunctional),
      name(std::move(name)),
      body(std::move(body)),
      r(r),
      g(g),
      b(b),
      thickness(thickness) {}

std::string Graph::getGraphShaderPart() const {
  if (isFunctional)
    return "if (isEqualApprox(" + body + ", worldPos.y, pixelSize * " +
           std::to_string(thickness) +
           "))"
           "  FragColor = vec4(" +
           std::to_string(r) + "," + std::to_string(g) + "," +
           std::to_string(b) +
           ", 1.0);"
           "else ";
  else
    return "if (" + body + ") FragColor = vec4(" + std::to_string(r) + "," +
           std::to_string(g) + "," + std::to_string(b) +
           ", 1.0);"
           "else ";
}