#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform bool IsGreyScale;
uniform float factor;

void main()
{
    if(IsGreyScale) {
        float h = (Height + 16)/128.0f;	// shift and scale the height in to a grayscale value
        FragColor = vec4(h, h, h, 1.0);
    }
    else {
        if (factor >= 0.0) {

            FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), factor);
        }
        else {
            FragColor = mix(texture(Texture2, TexCoord), texture(Texture1, TexCoord), -factor);
        }

    }
}