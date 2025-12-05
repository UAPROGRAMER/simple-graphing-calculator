#include <stb_image.h>

#include <SGC/error.hpp>
#include <SGC/imgui.hpp>
#include <SGC/mINI.hpp>
#include <SGC/opengl.hpp>
#include <SGC/sgc_engine.hpp>
#include <SGC/utils.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>

const GLchar* vertexShaderSource =                   //
    "#version 430 core\n"                            //
    "layout (location = 0) in vec2 attribPos;"       //
    "out vec2 fragPos;"                              //
    "void main() {"                                  //
    "  gl_Position = vec4(attribPos.xy, 0.0, 1.0);"  //
    "  fragPos = attribPos;"                         //
    "}";

const std::string fragmentShaderSourceStart =                               //
    "#version 430 core\n"                                                   //
    "#define pi 3.1415927410125732\n"                                       //
    "in vec2 fragPos;"                                                      //
    "out vec4 FragColor;"                                                   //
    "uniform vec2 windowSize;"                                              //
    "uniform vec2 position;"                                                //
    "uniform float zoom;"                                                   //
    "uniform float sublinePeriod;"                                          //
    "uniform float microlinePeriod;"                                        //
    "uniform float t;"                                                      //
    "bool isEqualApprox(float a, float b, float c) {"                       //
    "  return abs(a - b) <= c * 0.5;"                                       //
    "}"                                                                     //
    "void main() {"                                                         //
    "  float pixelSize = 1.0 / zoom;"                                       //
    "  vec2 worldPos = (windowSize * 0.5 * fragPos)"                        //
    "    * pixelSize + position;"                                           //
    "  vec2 pixelSublinePeriod ="                                           //
    "    (worldPos / sublinePeriod - round(worldPos / sublinePeriod))"      //
    "    * sublinePeriod;"                                                  //
    "  vec2 pixelMicrolinePeriod ="                                         //
    "    (worldPos / microlinePeriod - round(worldPos / microlinePeriod))"  //
    "    * microlinePeriod;"                                                //
    "  float x = worldPos.x;"                                               //
    "  float y = worldPos.y;"                                               //
    "  float ps = pixelSize;";

const std::string fragmentShaderSourceEnd =                                //
    "  if (isEqualApprox(worldPos.x, 0.0, pixelSize) ||"                   //
    "    isEqualApprox(worldPos.y, 0.0, pixelSize))"                       //
    "    FragColor = vec4(0.0, 0.0, 0.0, 1.0);"                            //
    "  else if (isEqualApprox(pixelSublinePeriod.x, 0.0, pixelSize) ||"    //
    "    isEqualApprox(pixelSublinePeriod.y, 0.0, pixelSize))"             //
    "    FragColor = vec4(0.65, 0.65, 0.65, 1.0);"                         //
    "  else if (isEqualApprox(pixelMicrolinePeriod.x, 0.0, pixelSize) ||"  //
    "    isEqualApprox(pixelMicrolinePeriod.y, 0.0, pixelSize))"           //
    "    FragColor = vec4(0.85, 0.85, 0.85, 1.0);"                         //
    "  else"                                                               //
    "    FragColor = vec4(1.0, 1.0, 1.0, 1.0);"                            //
    "}";

SGCEngine* activeEngine = nullptr;

void glfwWindowSizeCallback(GLFWwindow*, int width, int height) {
  if (activeEngine) activeEngine->windowSizeCallback(width, height);
}

void glfwScrollCallback(GLFWwindow*, double xoffset, double yoffset) {
  activeEngine->scrollCallback(xoffset, yoffset);
}

void glfwKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
  activeEngine->keyCallback(key, scancode, action, mods);
}

SGCEngine::SGCEngine() {
  activeEngine = this;

  // GLFW Initialization
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

#if defined(__APPLE__) && defined(__MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(800, 800, "Simple Graphing Calculator", nullptr,
                            nullptr);

  if (!window)
    throw SGCError(SGCErrorType::GLFW_ERROR,
                   "[GLFW]: Failed to create window.\n");

  // GLFW Setup
  GLFWimage windowIcon[4];
  int nrChannels;
  windowIcon[0].pixels =
      stbi_load("./data/icons/sgc256.png", &windowIcon[0].width,
                &windowIcon[0].height, &nrChannels, 0);
  windowIcon[1].pixels =
      stbi_load("./data/icons/sgc128.png", &windowIcon[1].width,
                &windowIcon[1].height, &nrChannels, 0);
  windowIcon[2].pixels =
      stbi_load("./data/icons/sgc64.png", &windowIcon[2].width,
                &windowIcon[2].height, &nrChannels, 0);
  windowIcon[3].pixels =
      stbi_load("./data/icons/sgc32.png", &windowIcon[3].width,
                &windowIcon[3].height, &nrChannels, 0);

  glfwSetWindowIcon(window, 4, windowIcon);

  stbi_image_free(windowIcon[0].pixels);
  stbi_image_free(windowIcon[1].pixels);
  stbi_image_free(windowIcon[2].pixels);
  stbi_image_free(windowIcon[3].pixels);

  glfwSetFramebufferSizeCallback(window, glfwWindowSizeCallback);
  glfwSetScrollCallback(window, glfwScrollCallback);
  glfwSetKeyCallback(window, glfwKeyCallback);

  glfwMakeContextCurrent(window);

  this->window = window;

  // Loading OpenGL with GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw SGCError(SGCErrorType::GLAD_ERROR,
                   "[GLAD]: Failed to load OpenGL loader.\n");

  // OpenGL Setup
  glViewport(0, 0, 800, 800);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Display VAO setup
  GLfloat displayVertices[] = {
      // positions
      -1.0f, -1.0f,  // Bottom Left
      -1.0f, 1.0f,   // Top Left
      1.0f,  1.0f,   // Top Right
      1.0f,  -1.0f,  // Bottom Right
  };

  GLuint displayIndices[] = {
      2, 0, 3,  // Bottom Right
      2, 1, 0   // Top Left
  };

  GLuint displayVAO;
  glGenVertexArrays(1, &displayVAO);

  GLuint displayVBO;
  glGenBuffers(1, &displayVBO);

  GLuint displayEBO;
  glGenBuffers(1, &displayEBO);

  glBindVertexArray(displayVAO);

  glBindBuffer(GL_ARRAY_BUFFER, displayVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(displayVertices), displayVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, displayEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(displayIndices), displayIndices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void*)(0));
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  this->displayVAO = displayVAO;

  // Shaders setup
  if (!makeShaderProgram())
    throw SGCError(SGCErrorType::OPENGL_ERROR,
                   "[OpenGL]: Failed to compile shader program.\n");

  // ImGui Setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::GetIO().IniFilename = "./data/imgui.ini";
  // ImGui::StyleColorsLight();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 430 core");
}

SGCEngine::~SGCEngine() {
  ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

  glfwSetFramebufferSizeCallback(window, nullptr);
  glfwSetScrollCallback(window, nullptr);
  glfwSetKeyCallback(window, nullptr);

  glfwTerminate();

  activeEngine = nullptr;
}

bool SGCEngine::makeShaderProgram() {
  GLint shaderSetupSuccess;
  GLchar shaderSetupInfoLog[512];

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderSetupSuccess);

  if (!shaderSetupSuccess) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, shaderSetupInfoLog);
    return false;
  }

  std::string fragmentShaderSourceStr = fragmentShaderSourceStart;

  for (const auto& graph : graphs)
    if (graph.isVisible)
      fragmentShaderSourceStr += graph.getGraphShaderPart();

  fragmentShaderSourceStr += fragmentShaderSourceEnd;

  const GLchar* fragmentShaderSource = fragmentShaderSourceStr.c_str();

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderSetupSuccess);

  if (!shaderSetupSuccess) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, shaderSetupInfoLog);
    glDeleteShader(vertexShader);
    return false;
  }

  GLuint shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderSetupSuccess);

  if (!shaderSetupSuccess) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, shaderSetupInfoLog);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return false;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (this->shaderProgram != 0) glDeleteProgram(this->shaderProgram);

  this->shaderProgram = shaderProgram;

  windowSizeUniformLocation = glGetUniformLocation(shaderProgram, "windowSize");
  positionUniformLocation = glGetUniformLocation(shaderProgram, "position");
  zoomUniformLocation = glGetUniformLocation(shaderProgram, "zoom");
  sublinePeriodUniformLocation =
      glGetUniformLocation(shaderProgram, "sublinePeriod");
  microlinePeriodUniformLocation =
      glGetUniformLocation(shaderProgram, "microlinePeriod");
  timeUniformLocation = glGetUniformLocation(shaderProgram, "t");

  return true;
}

void SGCEngine::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    process();

    processGUI();

    draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
}

void SGCEngine::process() {
  if (ImGui::GetIO().WantCaptureKeyboard) return;

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) positionY += 2.0 / zoom;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) positionY -= 2.0 / zoom;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) positionX += 2.0 / zoom;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) positionX -= 2.0 / zoom;
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      zoom *= 0.99;
    else
      zoom *= 1.0 / 0.99;
}

void SGCEngine::processGUI() {
  static bool isInfoWindowOpen = false;
  static bool isGraphsWindowOpen = false;
  bool shouldTeleportPopupOpen = false;
  bool shouldAddGraphPopupOpen = false;
  bool shouldEditGraphPopupOpen = false;
  bool shouldSaveGraphsPopupOpen = false;
  bool shouldLoadGraphsPopupOpen = false;
  static std::size_t editGraphIndex = 0;
  static float newPosition[2];
  static char graphName[33];
  static char graphBody[129];
  static float graphColor[3] = {0.5};
  static float graphThickness = 1.0;
  static bool graphIsFunctional = true;
  bool isGraphToRemove = false;
  std::size_t graphToRemoveIndex = 0;
  static char graphsSavefileName[33];
  static std::vector<std::string> graphsSavefiles;
  static std::vector<const char*> graphsSavefilesCStr;
  static bool graphsSavefileSelected = false;
  static std::size_t graphsSavefilesSelectedItemIndex = -1;

  static auto validateGraphName = [this](const std::string& name) -> bool {
    if (name.empty()) return false;
    for (const auto& graph : graphs) {
      if (name == graph.name) return false;
    }
    return true;
  };

  ImGui::BeginMainMenuBar();

  if (ImGui::BeginMenu("Windows")) {
    if (ImGui::MenuItem("Info")) isInfoWindowOpen = !isInfoWindowOpen;
    if (ImGui::MenuItem("Graphs")) isGraphsWindowOpen = !isGraphsWindowOpen;
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Tools")) {
    if (ImGui::MenuItem("Center pos")) {
      positionX = 0.0;
      positionY = 0.0;
    }

    if (ImGui::MenuItem("Normalize zoom")) zoom = 200.0;

    if (ImGui::MenuItem("Teleport")) shouldTeleportPopupOpen = true;

    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  if (isInfoWindowOpen) {
    ImGui::Begin("Info", &isInfoWindowOpen, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("SGC version: 1.1.0");

    ImGui::TextUnformatted(("WinSize: (" + std::to_string(windowWidth) + ";" +
                 std::to_string(windowHeight) + ")")
                    .c_str());
    ImGui::TextUnformatted(("Zoom: " + std::to_string(zoom)).c_str());
    ImGui::TextUnformatted(("Pos: (" + std::to_string(positionX) + ";" +
                 std::to_string(positionY) + ")")
                    .c_str());

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    float worldX = (cursorX - windowWidth / 2.0) / zoom + positionX;
    float worldY = -(cursorY - windowHeight / 2.0) / zoom + positionY;

    ImGui::TextUnformatted(("MousePos: (" + std::to_string(worldX) + ";" +
                 std::to_string(worldY) + ")")
                    .c_str());

    ImGui::TextUnformatted(("FPS: " + std::to_string(ImGui::GetIO().Framerate)).c_str());

    ImGui::End();
  }

  if (isGraphsWindowOpen) {
    ImGui::Begin("Graphs", &isGraphsWindowOpen,
                 ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Add graph")) shouldAddGraphPopupOpen = true;

    ImGui::SameLine();

    if (ImGui::Button("Save graphs")) shouldSaveGraphsPopupOpen = true;

    ImGui::SameLine();

    if (ImGui::Button("Load graphs")) shouldLoadGraphsPopupOpen = true;

    ImGui::Text("Graphs:");

    for (std::size_t i = 0; i < graphs.size(); i++) {
      bool opened = false;

      if (ImGui::TreeNode(graphs.at(i).name.c_str())) {
        opened = true;

        if (!graphs.at(i).isValid) {
          ImGui::SameLine();
          ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "!");
          ImGui::SetItemTooltip("Graph is not valid.");
        }

        if (graphs.at(i).isFunctional)
          ImGui::Text("Functional");
        else
          ImGui::Text("Equational");

        ImGui::SameLine();

        if (graphs.at(i).isVisible)
          ImGui::Text("Visible");
        else
          ImGui::Text("Invisible");

        if (graphs.at(i).isFunctional)
          ImGui::TextUnformatted(("y = " + graphs.at(i).body).c_str());
        else
          ImGui::TextUnformatted((graphs.at(i).body).c_str());


        if (ImGui::Button("Edit")) {
          shouldEditGraphPopupOpen = true;
          editGraphIndex = i;
        }

        ImGui::SameLine();

        if (ImGui::Button("Change visibility")) {
          graphs.at(i).isVisible = !graphs.at(i).isVisible;
          graphs.at(i).isValid = makeShaderProgram();
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete")) {
          isGraphToRemove = true;
          graphToRemoveIndex = i;
        }

        ImGui::TreePop();
      }

      if (!opened && !graphs[i].isValid) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "!");
        ImGui::SetItemTooltip("Graph is not valid.");
      }
    }

    ImGui::End();
  }

  if (shouldTeleportPopupOpen) {
    newPosition[0] = positionX;
    newPosition[1] = positionY;
    ImGui::OpenPopup("Teleport");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5, 0.5));
  }

  if (ImGui::BeginPopupModal("Teleport", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::InputFloat2("New Position", newPosition);

    if (ImGui::Button("Go")) {
      positionX = newPosition[0];
      positionY = newPosition[1];
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  if (shouldAddGraphPopupOpen) {
    ImGui::OpenPopup("Add graph");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5, 0.5));
    for (std::size_t i = 0; i < sizeof(graphName); i++) graphName[i] = '\0';
    for (std::size_t i = 0; i < sizeof(graphBody); i++) graphBody[i] = '\0';
    for (std::size_t i = 0; i < sizeof(graphColor); i++) graphColor[i] = 0.5;
    graphThickness = 1.0;
    graphIsFunctional = true;
  }

  if (ImGui::BeginPopupModal("Add graph", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::InputText("Name", graphName, sizeof(graphName));
    ImGui::InputText("Body", graphBody, sizeof(graphBody));
    ImGui::ColorEdit3("Color", graphColor);
    ImGui::DragFloat("Thickness", &graphThickness, 0.1f, 0.2f, 5.0f);
    ImGui::SetItemTooltip("Only for functional graphs.");
    ImGui::Checkbox("Is functional", &graphIsFunctional);

    if (ImGui::Button("Add")) {
      std::string name(graphName);
      if (validateGraphName(name)) {
        graphs.push_back(Graph(graphIsFunctional, std::move(graphName),
                               std::string(graphBody), graphColor[0],
                               graphColor[1], graphColor[2], graphThickness));
        ImGui::CloseCurrentPopup();
        graphs.back().isValid = makeShaderProgram();
      } else
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  if (shouldEditGraphPopupOpen) {
    ImGui::OpenPopup("Edit graph");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5, 0.5));
    for (std::size_t i = 0; i < sizeof(graphName); i++) graphName[i] = '\0';
    for (std::size_t i = 0; i < sizeof(graphBody); i++) graphBody[i] = '\0';
    for (std::size_t i = 0; i < sizeof(graphColor); i++) graphColor[i] = 0.5;
    for (std::size_t i = 0;
         i < std::min<std::size_t>(sizeof(graphName),
                                   graphs.at(editGraphIndex).name.size());
         i++)
      graphName[i] = graphs.at(editGraphIndex).name.at(i);
    for (std::size_t i = 0;
         i < std::min<std::size_t>(sizeof(graphBody),
                                   graphs.at(editGraphIndex).body.size());
         i++)
      graphBody[i] = graphs.at(editGraphIndex).body.at(i);
    graphColor[0] = graphs.at(editGraphIndex).r;
    graphColor[1] = graphs.at(editGraphIndex).g;
    graphColor[2] = graphs.at(editGraphIndex).b;
    graphThickness = graphs.at(editGraphIndex).thickness;
    graphIsFunctional = graphs.at(editGraphIndex).isFunctional;
  }

  if (ImGui::BeginPopupModal("Edit graph", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::InputText("Body", graphBody, sizeof(graphBody));
    ImGui::ColorEdit3("Color", graphColor);
    ImGui::DragFloat("Thickness", &graphThickness, 0.1f, 0.2f, 5.0f);
    ImGui::SetItemTooltip("Only for functional graphs.");
    ImGui::Checkbox("Is functional", &graphIsFunctional);

    if (ImGui::Button("Confirm")) {
      graphs.at(editGraphIndex).isFunctional = graphIsFunctional;
      graphs.at(editGraphIndex).body = std::string(graphBody);
      graphs.at(editGraphIndex).r = graphColor[0];
      graphs.at(editGraphIndex).g = graphColor[1];
      graphs.at(editGraphIndex).b = graphColor[2];
      graphs.at(editGraphIndex).thickness = graphThickness;
      ImGui::CloseCurrentPopup();
      graphs.at(editGraphIndex).isValid = makeShaderProgram();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  if (shouldSaveGraphsPopupOpen) {
    ImGui::OpenPopup("Save graphs");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5, 0.5));
    for (std::size_t i = 0; i < sizeof(graphsSavefileName); i++)
      graphsSavefileName[i] = '\0';
  }

  if (ImGui::BeginPopupModal("Save graphs", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::InputText("Savefile name", graphsSavefileName,
                     sizeof(graphsSavefileName));

    if (ImGui::Button("Save")) {
      mINI::INIFile file("./data/saves/" +
                         std::string(graphsSavefileName) + ".ini");

      mINI::INIStructure ini;

      for (const auto& graph : graphs) {
        ini[graph.name]["body"] = graph.body;
        ini[graph.name]["r"] = std::to_string(graph.r);
        ini[graph.name]["g"] = std::to_string(graph.g);
        ini[graph.name]["b"] = std::to_string(graph.b);
        ini[graph.name]["thickness"] = std::to_string(graph.thickness);
        ini[graph.name]["isFunctional"] = std::to_string(graph.isFunctional);
        ini[graph.name]["isVisible"] = std::to_string(graph.isVisible);
      }

      bool a = file.generate(ini);

      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  if (shouldLoadGraphsPopupOpen) {
    ImGui::OpenPopup("Load graphs");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5, 0.5));
    graphsSavefiles.clear();
    graphsSavefilesCStr.clear();

    for (const auto& file :
         std::filesystem::directory_iterator("./data/saves"))
      if (std::filesystem::is_regular_file(file))
        graphsSavefiles.push_back(file.path().stem().string());

    for (const auto& file : graphsSavefiles)
      graphsSavefilesCStr.push_back(file.c_str());

    graphsSavefilesSelectedItemIndex = -1;
    graphsSavefileSelected = false;
  }

  if (ImGui::BeginPopupModal("Load graphs", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::BeginListBox(
            "Savefiles", ImVec2(10.0 * ImGui::GetTextLineHeightWithSpacing(),
                                5.0 * ImGui::GetTextLineHeightWithSpacing()))) {
      for (std::size_t i = 0; i < graphsSavefilesCStr.size(); i++) {
        const bool isSelected = graphsSavefilesSelectedItemIndex == i;

        if (ImGui::Selectable(graphsSavefilesCStr[i], isSelected)) {
          graphsSavefilesSelectedItemIndex = i;
          graphsSavefileSelected = true;
        }

        if (isSelected) ImGui::SetItemDefaultFocus();
      }

      ImGui::EndListBox();
    }

    if (ImGui::Button("Load")) {
      if (graphsSavefileSelected) {
        graphs.clear();

        mINI::INIFile file(
            "./data/saves/" +
            graphsSavefiles.at(graphsSavefilesSelectedItemIndex) + ".ini");

        mINI::INIStructure ini;

        file.read(ini);

        for (auto& graph : ini) {
          graphs.push_back(Graph(std::stoi(ini[graph.first]["isFunctional"]),
                                 graph.first, ini[graph.first]["body"],
                                 std::stof(ini[graph.first]["r"]),
                                 std::stof(ini[graph.first]["g"]),
                                 std::stof(ini[graph.first]["b"]), 1.0));
          graphs.back().isVisible = std::stoi(ini[graph.first]["isVisible"]);
          graphs.back().isValid = makeShaderProgram();
        }
      }

      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete")) {
      if (graphsSavefileSelected) {
        std::filesystem::remove(
            "./data/saves/" +
            graphsSavefiles.at(graphsSavefilesSelectedItemIndex) + ".ini");
        graphsSavefilesSelectedItemIndex = -1;
        graphsSavefileSelected = false;

        graphsSavefiles.clear();
        graphsSavefilesCStr.clear();

        for (const auto& file :
            std::filesystem::directory_iterator("./data/saves"))
          if (std::filesystem::is_regular_file(file))
            graphsSavefiles.push_back(file.path().stem().string());

        for (const auto& file : graphsSavefiles)
          graphsSavefilesCStr.push_back(file.c_str());
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  if (isGraphToRemove) {
    graphs.erase(graphs.begin() + graphToRemoveIndex);
    makeShaderProgram();
  }
}

void SGCEngine::draw() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(displayVAO);
  glUseProgram(shaderProgram);

  glUniform2f(windowSizeUniformLocation, static_cast<GLfloat>(windowWidth),
              static_cast<GLfloat>(windowHeight));
  glUniform2f(positionUniformLocation, positionX, positionY);
  glUniform1f(zoomUniformLocation, zoom);
  glUniform1f(
      sublinePeriodUniformLocation,
      std::pow(10.0,
               std::round(std::log10(std::max<float>((float)(windowWidth),
                                                     (float)(windowHeight)) /
                                     zoom))));
  glUniform1f(
      microlinePeriodUniformLocation,
      std::pow(10.0,
               std::round(std::log10(std::max<float>((float)(windowWidth),
                                                     (float)(windowHeight)) /
                                     zoom / 10.0))));
  glUniform1f(timeUniformLocation, ImGui::GetTime());

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

void SGCEngine::windowSizeCallback(int width, int height) {
  glViewport(0, 0, width, height);
  windowWidth = width;
  windowHeight = height;
}

void SGCEngine::scrollCallback(double offsetX, double offsetY) {
  if (ImGui::GetIO().WantCaptureMouse) return;

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    float worldX = (cursorX - windowWidth / 2.0) / zoom + offsetX;
    float worldY = -(cursorY - windowHeight / 2.0) / zoom + offsetY;

    zoom *= std::pow(2.0, offsetY / 3.0);

    positionX += worldX - ((cursorX - windowWidth / 2.0) / zoom + offsetX);
    positionY += worldY - (-(cursorY - windowHeight / 2.0) / zoom + offsetY);
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    positionX += offsetY / zoom * 50.0;
  else
    positionY += offsetY / zoom * 50.0;
  positionX += offsetX / zoom * 50.0;
}

void SGCEngine::keyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}