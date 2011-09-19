uniform sampler2D Texture;
varying highp vec2 v_TexCoord;

void main(void)
{
	gl_FragColor = texture2D(Texture, v_TexCoord);
}
