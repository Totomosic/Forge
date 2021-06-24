#shader VERTEX
layout (location = 0) in vec3 v_Position;

uniform mat4 frg_ModelMatrix;

void main()
{
    gl_Position = frg_ModelMatrix * vec4(v_Position, 1.0);
}

#shader GEOMETRY
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 frg_PointShadowMatrices[6];
out vec4 f_FragPosition;
void main()
{
   for (int face = 0; face < 6; face++)
   {
       gl_Layer = face;
       for (int i = 0; i < 3; i++)
       {
           f_FragPosition = gl_in[i].gl_Position;
           gl_Position = frg_PointShadowMatrices[face] * f_FragPosition;
           EmitVertex();
       }
       EndPrimitive();
   }
}

#shader FRAGMENT
in vec4 f_FragPosition;
uniform vec3 frg_ShadowLightPosition;
uniform float frg_FarPlane;
void main()
{
   float lightDistance = length(f_FragPosition.xyz - frg_ShadowLightPosition);
   lightDistance = lightDistance / frg_FarPlane;
   gl_FragDepth = lightDistance;
}
