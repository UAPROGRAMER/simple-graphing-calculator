#include <SGC/error.hpp>
#include <SGC/utils.hpp>
#include <fstream>

std::string readFileToString(const std::filesystem::path& filepath) {
  if (!std::filesystem::exists(filepath))
    throw SGCError(SGCErrorType::ENGINE_ERROR,
                   "[SGCEngine]: Failed to read file \"" + filepath.string() +
                       "\". File doesn't exist.\n");

  if (!std::filesystem::is_regular_file(filepath))
    throw SGCError(SGCErrorType::ENGINE_ERROR,
                   "[SGCEngine]: Failed to read file \"" + filepath.string() +
                       "\". Not a regular file.\n");

  std::ifstream file(filepath);

  if (!file)
    throw SGCError(SGCErrorType::ENGINE_ERROR,
                   "[SGCEngine]: Failed to read file \"" + filepath.string() +
                       "\". Failed to open file.\n");

  file.seekg(0, std::ios::end);
  std::size_t size = static_cast<size_t>(file.tellg());
  std::string source(size, '\0');
  file.seekg(0);
  file.read(source.data(), size);

  return std::move(source);
}