attribute vec3 Position;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelviewMatrix;

void main(void)
{
	gl_Position = ProjectionMatrix * ModelviewMatrix * vec4(Position, 1.0);
}
