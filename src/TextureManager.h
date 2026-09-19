#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

/*  ═══════════════════════════════════════════════════════════════
    TextureManager  –  loads image files into OpenGL textures
    Uses stb_image for JPG/PNG decoding.
    CG Concept: Texture Mapping (GL_TEXTURE_2D, mipmaps)
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& instance();

    /* Load a texture from file, returns OpenGL texture ID (0 on failure). */
    GLuint load(const std::string& path);

    /* Check if a texture is already cached. */
    bool   isCached(const std::string& path) const;

private:
    TextureManager() = default;
    std::unordered_map<std::string, GLuint> cache_;
};

#endif /* TEXTUREMANAGER_H */
