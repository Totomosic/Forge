float clipPlane(vec3 worldPosition, vec4 plane)
{
   return dot(vec4(worldPosition, 1.0), plane);
}

void clipPlanes(vec3 worldPosition, vec4 plane[MAX_CLIPPING_PLANES], int usedClippingPlanes)
{
   for (int i = 0; i < usedClippingPlanes; i++)
   {
       gl_ClipDistance[i] = clipPlane(worldPosition, plane[i]);
   }
}
