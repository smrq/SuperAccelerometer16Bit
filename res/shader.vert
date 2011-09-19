attribute vec3 Position;
attribute vec2 TexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelviewMatrix;

varying vec2 v_TexCoord;

void main(void)
{
	gl_Position = ProjectionMatrix * ModelviewMatrix * vec4(Position, 1.0);
	v_TexCoord = TexCoord;
}
