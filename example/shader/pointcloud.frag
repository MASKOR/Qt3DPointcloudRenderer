#version 130

in vec3 normal;
in vec3 position;
in vec3 color;

uniform vec3 finalColor;

out vec4 fragColor;

void main()
{
//    vec3 n = normalize(normal);
//    vec3 s = normalize(vec3(1.0, 0.0, 1.0) - position);
//    vec3 v = normalize(-position);
//    float diffuse = max(dot(s, n), 0.0);
//    fragColor = vec4(diffuse * finalColor, 1.0);
//    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    fragColor = vec4(color, 1.0);
}
