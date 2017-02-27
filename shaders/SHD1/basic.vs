#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec2 vsiTexCoord;
 
out vec3 vsoNormal;
out vec3 vsoPosition;
out vec3 vsoModPos;
out vec2 vsoTexCoord;

void main(void) {
  vec4 mp = modelViewMatrix * vec4(vsiPosition, 1.0);
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiPosition, 0.0)).xyz;
  vsoPosition = vsiPosition;
  vsoModPos = mp.xyz;
  vsoTexCoord = vsiTexCoord;
  gl_Position = projectionMatrix * mp;
}
