//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
// GPL 	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	load texture functions and atlas class
//-----------------------------------------------------------------------------

#include "texture.h"
#include "utils.h"
#include <memory>
#include <string>
#include <vector>

TileAtlas::TileAtlas() { sourceRectangles = NULL; }

TileAtlas::~TileAtlas()
{
    if (sourceRectangles != NULL)
        delete[] sourceRectangles;
}

void TileAtlas::initialize(unsigned int* ptex, int frames_wide, int frames_high,
                           int frame_width, int frame_height)
{
    texture = ptex;
    totalFrames = frames_wide * frames_high;
    frames = Point(frames_wide, frames_high);
    imageSize = Point(frame_width * frames_wide, frame_height * frames_high);
    imageFrameSize = Point(frame_width, frame_height);
    texel = Vector2(1.0f / imageSize.x, 1.0f / imageSize.y);
    textureFrameSize = Vector2(1.0f / frames.x, 1.0f / frames.y);

    sourceRectangles = new ScreenRect[totalFrames];

    int currentFrame = 0;
    for (int y = 0; y < frames.y; y++)
    {
        for (int x = 0; x < frames.x; x++)
        {
            sourceRectangles[currentFrame].x = x * textureFrameSize.x;
            sourceRectangles[currentFrame].y = 1.0f - y * textureFrameSize.y;
            sourceRectangles[currentFrame].w = textureFrameSize.x;
            sourceRectangles[currentFrame].h = -textureFrameSize.y;
            currentFrame++;
        }
    }
}

unsigned int loadBMPTexture_ARGB(const char* filename)
{
    unsigned int texture;
    int width, height;
    std::vector<unsigned char> data;

    printf("Loading texture: %s\n", filename);

    data = std::move(loadTextureARGBfromfile(filename, width, height));
    if (data.empty())
    {
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data.data());

    return texture;
}

std::vector<unsigned char> loadTextureARGBfromfile(const char* filename, int& w,
                                                   int& h)
{
    std::vector<unsigned char> data;
    FILE* file;
    unsigned char header[54];

    const std::string resolvedPath = resolveResourcePath(filename);
    file = fopen(resolvedPath.c_str(), "rb");

    if (file == NULL)
    {
        printf("loadTexture error! %s\n", filename);
        return data;
    }

    fread(header, 1, 54, file);
    w = *(int*)&(header[0x12]);
    h = *(int*)&(header[0x16]);

    data.resize(w * h * 4);
    fread(data.data(), 16, 1, file);
    fread(data.data(), w * h * 4, 1, file);

    fclose(file);

    for (int i = 0; i < w * h; ++i)
    {
        int index = i * 4;
        unsigned char A, R, G, B;
        B = data[index];
        G = data[index + 1];
        R = data[index + 2];
        A = data[index + 3];

        data[index] = A;
        data[index + 1] = R;
        data[index + 2] = G;
        data[index + 3] = B;
    }
    return data;
}
