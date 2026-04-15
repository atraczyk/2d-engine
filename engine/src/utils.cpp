//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
//GPL 	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	misc functions
//-----------------------------------------------------------------------------

#include "utils.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <vector>

Point worldToScreen(Vector2 world_point, Camera camera, ScreenRect screen) {
  int x = (int)(((world_point.x - camera.position.x) / camera.size.x) *
                (float)screen.w);
  int y = (int)(((world_point.y - camera.position.y) / camera.size.y) * -1 *
                    ((float)screen.h) +
                (float)screen.h);
  return Point(x, y);
}

Vector2 screenToWorld(Point screen_point, Camera camera, ScreenRect screen) {
  float x = (float)(((screen_point.x * camera.size.x) / screen.w) +
                    camera.position.x);
  float y =
      (float)(((screen_point.y - screen.h) / screen.h) * -1 * +camera.size.y +
              camera.position.y);
  return Vector2(x, y);
}

Point worldToTile(Vector2 position, Vector2 size) {
  return Point((int)(position.x / size.x), (int)(position.y / size.y));
}

bool isInRange(int x, int min, int max) { return (x >= min && x <= max); }

int roundUp(int number, int fixedBase) {
  return (number + fixedBase - 1) & ~(fixedBase - 1);
}

float roundUp(float number, float fixedBase) {
  if (fixedBase != 0 && number != 0) {
    float sign = number > 0.0f ? 1.0f : -1.0f;
    number *= sign;
    number /= fixedBase;
    int fixedPoint = (int)ceil(number);
    number = fixedPoint * fixedBase;
    number *= sign;
  }
  return number;
}

wchar_t *convertCharArrayToLPCWSTR(const char *charArray) {
  size_t length = std::mbstowcs(nullptr, charArray, 0);
  if (length == static_cast<size_t>(-1)) {
    wchar_t *fallback = new wchar_t[1];
    fallback[0] = L'\0';
    return fallback;
  }

  wchar_t *wString = new wchar_t[length + 1];
  std::mbstowcs(wString, charArray, length + 1);
  return wString;
}

static bool fileExists(const std::string &path) {
  std::ifstream file(path.c_str(), std::ios::binary);
  return file.good();
}

static std::string normalizePath(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  return path;
}

static std::string ensureTrailingSlash(std::string path) {
  path = normalizePath(path);
  if (!path.empty() && path.back() != '/') {
    path.push_back('/');
  }
  return path;
}

static std::string buildCandidate(const std::string &root,
                                  const std::string &relativePath) {
  return ensureTrailingSlash(root) + normalizePath(relativePath);
}

std::string resolveResourcePath(const std::string &relativePath) {
  if (relativePath.empty()) {
    return relativePath;
  }

  const std::string normalizedRelative = normalizePath(relativePath);
  std::vector<std::string> roots;

  const char *envRoot = std::getenv("ENGINE_RESOURCE_ROOT");
  if (envRoot && envRoot[0] != '\0') {
    roots.push_back(envRoot);
  }

  char *basePath = SDL_GetBasePath();
  if (basePath) {
    roots.push_back(std::string(basePath) + "resources");
    roots.push_back(basePath);
    SDL_free(basePath);
  }

  roots.push_back("resources");
  roots.push_back(".");

  for (const std::string &root : roots) {
    const std::string candidate = buildCandidate(root, normalizedRelative);
    if (fileExists(candidate)) {
      return candidate;
    }
  }

  return normalizedRelative;
}

std::string resolveResourcePath(const char *relativePath) {
  return resolveResourcePath(std::string(relativePath ? relativePath : ""));
}
