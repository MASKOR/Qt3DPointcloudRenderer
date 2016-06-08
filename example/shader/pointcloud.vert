#version 130

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;

out vec3 position;
out vec3 normal;
out vec3 color;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;
uniform mat4 projectionMatrix;
uniform mat4 viewportMatrix;

uniform float pointSize;

void main()
{
    normal = normalize(modelViewNormal * vertexNormal);
    position = vec3(modelView * vec4(vertexPosition, 1.0));
    color = vertexPosition * 0.1;//vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
    gl_PointSize = viewportMatrix[1][1] * projectionMatrix[1][1] * pointSize / gl_Position.w;
}
