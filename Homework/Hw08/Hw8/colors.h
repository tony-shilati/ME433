#ifndef COLORS_H
#define COLORS_H

// Define the wsColor struct
typedef struct {
    unsigned char r; // Red component (0-255)
    unsigned char g; // Green component (0-255)
    unsigned char b; // Blue component (0-255)
} wsColor;

// Function prototype for converting HSB to RGB
wsColor HSBtoRGB(float hue, float sat, float brightness);

#endif // COLORS_H