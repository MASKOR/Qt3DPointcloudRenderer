#version 150

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;
uniform mat4 projectionMatrix;
uniform mat4 viewportMatrix;
uniform float fieldOfView;
uniform float fieldOfViewVertical;

out vec3 viewspacePosition;
out vec3 viewspaceNormal;
out vec3 viewspaceTang;
out vec3 viewspaceBitang;
out vec3 color;

uniform float pointSize;

vec4 hsv_to_rgb(float h, float s, float v, float a)
{
        float c = v * s;
        h = mod((h * 6.0), 6.0);
        float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
        vec4 color;

        if (0.0 <= h && h < 1.0) {
                color = vec4(c, x, 0.0, a);
        } else if (1.0 <= h && h < 2.0) {
                color = vec4(x, c, 0.0, a);
        } else if (2.0 <= h && h < 3.0) {
                color = vec4(0.0, c, x, a);
        } else if (3.0 <= h && h < 4.0) {
                color = vec4(0.0, x, c, a);
        } else if (4.0 <= h && h < 5.0) {
                color = vec4(x, 0.0, c, a);
        } else if (5.0 <= h && h < 6.0) {
                color = vec4(c, 0.0, x, a);
        } else {
                color = vec4(0.0, 0.0, 0.0, a);
        }

        color.rgb += v - c;

        return color;
}

void main(void)
{
    vec4 viewSpacePos = modelView * vec4(vertexPosition, 1.0);
    viewspacePosition = viewSpacePos.xyz;
    viewspaceNormal = modelViewNormal * vertexNormal;
    viewspaceTang = normalize(cross(viewspaceNormal, vec3(0.0,0.0,1.0))); //longest radius
    viewspaceBitang = normalize(cross(viewspaceTang, viewspaceNormal)); //smallest radius
    gl_Position = projectionMatrix * viewSpacePos;

    // VERY SLOW
    //vec2 csize = max(max(max(max(max(abs(Ap.xy-Bp.xy), abs(Ap.xy-Cp.xy)), abs(Ap.xy-Dp.xy)), abs(Bp.xy-Cp.xy)), abs(Bp.xy-Dp.xy)), abs(Cp.xy-Dp.xy));

    float dist = -viewSpacePos.z; // == gl_Position.w

    // x2 would be perfect. But the smaller the points are, the better performance is.
    gl_PointSize = 1.9 * viewportMatrix[1][1] * projectionMatrix[1][1] * pointSize / gl_Position.w;
    color = hsv_to_rgb(vertexPosition.y*10.0, 1.0, 1.0, 0.0).rgb;// * 0.1;
}
