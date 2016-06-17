#version 150

uniform mat4 modelView;
uniform mat4 viewMatrix;
uniform mat3 modelViewNormal;
uniform mat4 mvp;
uniform mat4 projectionMatrix;
uniform mat4 inverseProjectionMatrix;
uniform mat4 viewportMatrix;
uniform float nearPlane;
uniform float farPlane;
uniform float width;
uniform float height;

in vec3 viewspacePosition;
in vec3 viewspaceNormal;
in vec3 viewspaceTang;
in vec3 viewspaceBitang;
in vec3 color;


uniform float pointSize;

out vec4 fragColor;

void main(void)
{

//    vec3 eyePos = vec3(viewPoint.xy+(gl_PointCoord.xy - vec2(0.5))*2.0, viewPoint.z); //< this is not enough. perspective division was skipped?
// unproject
    vec3 ndcPos;
    ndcPos.xy = gl_FragCoord.xy / vec2(width, height);
    ndcPos.z = 0.0;
    ndcPos -= 0.5;
    ndcPos *= 2.0;
    vec4 clipPos;
    clipPos.w = projectionMatrix[3][2] / (ndcPos.z - (projectionMatrix[2][2] / projectionMatrix[2][3]));
    //clipPos.w = (2.0 * nearPlane * farPlane / ((nearPlane + farPlane) + ndcPos.z * (nearPlane - farPlane)));
    clipPos.xyz = ndcPos * clipPos.w;
    vec3 viewspacePositionFragment = vec3(inverseProjectionMatrix * clipPos);

    vec3 normal = normalize(viewspaceNormal);

    // line plane intersection
    // t = ( dot(N,O) + d ) / ( dot(N,D) )
    // p = O + t*D
    // vec3 O = vec3(0.0);
    vec3 ray_dir = normalize(viewspacePositionFragment);
    float d = dot(normal, viewspacePosition);
    float t = d / dot(normal, ray_dir);
    vec3 p = t * ray_dir;
//    float distSphere = length(cross(ray_dir, viewspacePosition)); //< sphere
    float distDisc = distance(p, viewspacePosition); //< disc
    float dist = distDisc;
    if(pointSize <= dist) discard;

    fragColor.rgb = color.rgb;
    fragColor.a = 1.0;//smoothstep(pointSize*0.5+fwidth(dist), pointSize*0.5, dist);
    //if(fragColor.a <= 0.0) discard;

    // todo: fix
    //vec3 pp = projectionMatrix * p;
    //vec4 reprojeced_ndc_pos = projectionmatrix * -p;
    //float ndc_depth = reprojeced_ndc_pos.z / reprojeced_ndc_pos.w;

    //float depth = (((viewport.w-viewport.z) * ndc_depth) + viewport.z + viewport.w) / 2.0;
    //gl_FragDepth = depth;
}
