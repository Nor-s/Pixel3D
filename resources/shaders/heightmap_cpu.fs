#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord;

uniform sampler2D Texture1;
uniform bool IsGreyScale;

void main()
{
    if(IsGreyScale) {
        float h = (Height + 16)/128.0f;	// shift and scale the height in to a grayscale value
        FragColor = vec4(h, h, h, 1.0);
    }
    else {
        FragColor = texture(Texture1, TexCoord);
    }
}