#version 330
uniform float movement;
uniform int emposY;
uniform int emposX;
uniform int body;
uniform int drawBody;
uniform int faceNormal;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lumPos;
in  vec3 vsoNormal[];
in  vec3 vsoPosition[];
in  vec3 vsoModPos[];
in  vec2 vsoTexCoord[];
out vec3 gsoNormal;
out vec3 gsoModPos;
out vec3 gsoColor;
out vec2 gsoTexCoord;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

float rand(vec2 co){
    float a = 12.9898;
    float b = 78.233;
     float c = 43758.5453;
    float dt= dot(co.xy ,vec2(a,b));
    float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

void main(void) {
int angle = 30;

  mat4 rot = mat4( cos(angle),    0.0, sin(angle), 0.0,
		   0.0,       1.0,    0.0, 0.0, 
		   -sin(angle),   0.0, cos(angle), 0.0,
		   0.0,       0.0,    0.0, 1.0 );


  vec3 normal = vec3(0);

  if(gl_in.length() == 3) {
    if(faceNormal != 0) {
      normal = normalize(cross(vsoPosition[1].xyz - vsoPosition[0].xyz, 
			       vsoPosition[2].xyz - vsoPosition[1].xyz));
      normal = (transpose(inverse(modelViewMatrix)) * vec4(normal, 0.0)).xyz;
    }
    for(int i = 0; i < gl_in.length(); i++) {
      vec3 L = normalize(vsoModPos[i] - lumPos.xyz);
if(drawBody!=0){
if(body!=0)
if(gl_in[i].gl_Position.x<-80&&gl_in[i].gl_Position.y+emposY>-20&&gl_in[i].gl_Position.y+emposY<80){
gl_in[i].gl_Position.x=emposX-20;
}
if(!(body!=0))
if(!(gl_in[i].gl_Position.z<181&&gl_in[i].gl_Position.z>154&&gl_in[i].gl_Position.y+emposY<38&&gl_in[i].gl_Position.y+emposY>-20)){
gl_in[i].gl_Position.x=0+emposX;
gl_in[i].gl_Position.y=50+emposY;
gl_in[i].gl_Position.z=200;
}

}

 gl_Position = gl_in[i].gl_Position;


//   gl_in[i].gl_Position.x*=rand(gl_in[i].gl_Position.xy);
	
  // gl_in[i].gl_Position.y*=rand(gl_in[i].gl_Position.xz);

    //gl_in[i].gl_Position.z*=rand(gl_in[i].gl_Position.yz);
      if(faceNormal != 0) {
	gsoNormal = normal;
	gsoColor = vec3(1) * dot(normal, -L);
      }else{
	gsoNormal = vsoNormal[i];
	gsoColor = vec3(1) * dot(vsoNormal[i], -L);
      }
      gsoModPos = vsoModPos[i];
      gsoTexCoord = vsoTexCoord[i];
      EmitVertex();
    }
    EndPrimitive();
  }
}
