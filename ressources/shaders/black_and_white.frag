// Fragment shader for grayscale effect
#version 120  // Using GLSL 120 for better compatibility with SFML

// The input texture sampler
uniform sampler2D texture;

void main() {
    // Get the current pixel color using SFML's built-in gl_TexCoord
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    
    // Calculate grayscale value using luminance conversion factors
    // These weights account for human perception of different color channels
    float grayscale = dot(pixel.rgb, vec3(0.299, 0.587, 0.114));
    
    // Create the output color by using the grayscale value for all RGB channels
    // Preserve the original alpha value
    vec4 finalColor = vec4(grayscale, grayscale, grayscale, pixel.a);
    
    // Set the output color
    gl_FragColor = finalColor;
    
    // Apply the texture coordinates
    gl_FragColor = gl_FragColor * gl_Color;
}