#version 330
uniform sampler2D myTexture;
uniform vec4 lumpos;
in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;

void main(void) {
  vec3 color;
  vec3 lum = normalize(vsoModPosition.xyz - lumpos.xyz);
  color = 0.3 + 0.7 * vec3(1) * dot(normalize(vsoNormal), -lum);
  vec4 tex = texture(myTexture, vsoTexCoord);
  fragColor = vec4(color * tex.rgb, tex.a); //vec4(vsoTexCoord.xy, 0, 1);//VISUALISER COORDONNE DE TEXTURE
}
