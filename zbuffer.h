//z buffer

#ifndef _ZBUFFER_INCLUDED_
#define _ZBUFFER_INCLUDED_

struct ZBuffer {
    int height;
    int width;
    std::vector<std::vector<float>> depthBuffer;

    // Constructor to initialize the Z-buffer with given in main.cpp height and width
    ZBuffer(int h = DRAW_HEIGHT, int w = DRAW_WIDTH)
        : height(h), width(w), depthBuffer(h, std::vector<float>(w, std::numeric_limits<float>::max())) {
    }
};

ZBuffer zbuffer(DRAW_HEIGHT, DRAW_WIDTH);

void resetZBuffer() {
    float maxFloat = std::numeric_limits<float>::max();
    for (int i = 0; i < zbuffer.height; ++i) {
        for (int j = 0; j < zbuffer.width; ++j) {
            zbuffer.depthBuffer[i][j] = maxFloat;
        }
    }
}

#endif // ZBUFFER_INCLUDED