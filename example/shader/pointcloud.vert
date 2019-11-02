#version 110

attribute highp vec3 vertexPosition;
//varying vec3 vertexNormal;
//varying vec3 vertexColor;

//varying vec3 position;
//varying vec3 normal;
//varying highp vec3 color;

//uniform mat4 modelView;
//uniform mat3 modelViewNormal;
uniform highp mat4 modelMatrix;
uniform highp mat4 mvp;
//uniform mat4 projectionMatrix;
//uniform mat4 viewportMatrix;

uniform float pointSize;

void main()
{
//    normal = normalize(modelViewNormal * vertexNormal);
    //position = vec3(modelView * vec4(vertexPosition, 1.0));
    // color = vertexPosition * 0.1;//vertexColor;
    gl_Position = mvp * ( vec4(vertexPosition, 1.0));
    //gl_PointSize = 1.0;//viewportMatrix[1][1] * projectionMatrix[1][1] * pointSize / gl_Position.w;
}
