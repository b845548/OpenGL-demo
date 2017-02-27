#version 330
uniform sampler2D myTexture;
uniform vec4 lumPos;
uniform int phong, tc;
in  vec3 gsoNormal;
in  vec3 gsoModPos;
in  vec3 gsoColor;
in  vec2 gsoTexCoord;
out vec4 fragColor;

void main(void) {
  if(tc != 0) {
      vec3 L = normalize(gsoModPos - lumPos.xyz);
      fragColor = texture(myTexture, gsoTexCoord)*vec4(vec3(dot(gsoNormal, -L)), 1.0);

  } else {
    if(phong != 0) {
      vec3 L = normalize(gsoModPos - lumPos.xyz);
      fragColor = vec4(vec3(dot(gsoNormal, -L)), 1.0);
    } else
      fragColor = vec4(vec3(gsoColor.r), 1.0);
  }
}
