#include "TextureManager.h"
#include <cstdio>

/* stb_image – single-header image loader (included once here) */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* ────────────────────────────────────────────────────────────── */

TextureManager& TextureManager::instance() {
    static TextureManager tm;
    return tm;
}

bool TextureManager::isCached(const std::string& path) const {
    return cache_.count(path) > 0;
}

GLuint TextureManager::load(const std::string& path) {
    /* Return cached texture if already loaded */
    auto it = cache_.find(path);
    if (it != cache_.end()) return it->second;

    /* Load image via stb_image */
    stbi_set_flip_vertically_on_load(1);   /* OpenGL Y=0 is bottom */
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);
    if (!data) {
        std::printf("TextureManager: FAILED to load '%s'\n", path.c_str());
        cache_[path] = 0;
        return 0;
    }

    GLenum fmt = GL_RGB;
    if      (ch == 1) fmt = GL_LUMINANCE;
    else if (ch == 3) fmt = GL_RGB;
    else if (ch == 4) fmt = GL_RGBA;

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Upload + generate mipmaps */
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    gluBuild2DMipmaps(GL_TEXTURE_2D, ch, w, h, fmt, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    cache_[path] = id;
    std::printf("TextureManager: loaded '%s' (%dx%d, id=%u)\n", path.c_str(), w, h, id);
    return id;
}
