#version 110

//varying vec3 normal;
//varying vec3 position;
//varying highp vec3 color;

//uniform vec3 finalColor;

//varying vec4 fragColor;

void main()
{
//    vec3 n = normalize(normal);
//    vec3 s = normalize(vec3(1.0, 0.0, 1.0) - position);
//    vec3 v = normalize(-position);
//    float diffuse = max(dot(s, n), 0.0);
//    fragColor = vec4(diffuse * finalColor, 1.0);
//    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
//    gl_FragColor = vec4(color, 1.0);
    gl_FragColor = vec4(0.0,0.5,0.5, 0.5);
}
