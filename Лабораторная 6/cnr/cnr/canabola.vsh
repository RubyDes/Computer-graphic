float getCanabolaCoef(float x)
{
	return (1 + sin(x)) * (1 + 0.9 * cos(8 * x)) * (1 + 0.1 * cos(24 * x)) * (0,5 + 0.3 * cos(140 * x));
}

void main()
{
	vec4 position = gl_Vertex;
	float canabolaCoef = getCanabolaCoef(position.x);

	position.y = canabolaCoef * sin(position.x);
	position.x = canabolaCoef * cos(position.x);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}