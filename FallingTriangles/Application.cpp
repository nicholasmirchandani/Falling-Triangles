#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

enum VAO_IDS {Triangles, NumVAOs};
enum Buffer_IDs {ArrayBuffer, ColorBuffer, NumBuffers};
enum Attrib_IDs {vPosition = 0, vColor = 1};

GLuint CreateShader(GLenum shaderType, std::string filename);

int main(void)
{
	//GLFW/GLEW Initialization
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1080, 1080, "Falling Triangles", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glewInit();

	//RNG Initialization
	srand(time(0));

	//Initialize Vertex Data; Let's draw a triangle.
	GLuint VAOs[NumVAOs];
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	const int numTriangles = 400;
	const int numVertices = 3;
	GLfloat vertexPos[numVertices * numTriangles][2];

	//Setting triangle vertices off screen so everything about them can be calculated in the first render loop
	for (int i = 0; i < numVertices * numTriangles; ++i) {
		for (int j = 0; j < 2; ++j) {
			vertexPos[i][j] = -1;
		}
	}

	GLuint Buffers[NumBuffers];
	glCreateBuffers(NumBuffers, Buffers);
	glNamedBufferStorage(Buffers[ArrayBuffer], sizeof(vertexPos), vertexPos, 0);


	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	
	//Now let's initialize Color Data per vertex
	GLfloat vertexColors[numVertices * numTriangles][4];

	//Randomly assigning each triangle a color
	for (int i = 0; i < numTriangles; ++i) {
		float red = (float)rand() / RAND_MAX;
		float blue = (float)rand() / RAND_MAX;
		float green = (float)rand() / RAND_MAX;
		for (int j = 0; j < numVertices; ++j) {
			vertexColors[i*numVertices+j][0] = red;
			vertexColors[i*numVertices+j][1] = blue;
			vertexColors[i*numVertices+j][2] = green;
			vertexColors[i*numVertices+j][3] = 1.0f;
		}
	}

	glNamedBufferStorage(Buffers[ColorBuffer], sizeof(vertexColors), vertexColors, 0);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColorBuffer]);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	//Shader Setup
	GLuint program = glCreateProgram();

	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, "passthrough_vert.shader");
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "frag.shader");	
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glValidateProgram(program);
	glLinkProgram(program);
	glUseProgram(program);

	//Uniform preparation
	int pausedLocation = glGetUniformLocation(program, "paused");
	int speedMultLocation = glGetUniformLocation(program, "speedMult");

	//NOTE: Not using Element Buffers this time around, since it's just generating a bunch of triangles, so none of them would actually share any vertices
	
	//Setting Line Width for nice thick trinagles
	glLineWidth(5);

	//Simulation setup
	float triangleSpeeds[numTriangles][2] = {
		{0.0025f, -0.005f},
		{0.0025f, -0.005f}
	};
	float sideLength = 0.1;
	float horizontalOffset = ((float) rand() / (RAND_MAX)) - 0.5f;
	bool paused = false;
	bool escapeWasDown = false;
	float speedMultiplier = 1.0f;

	while (!glfwWindowShouldClose(window)) {

		//Scales and resizes window as user scales it
		int xsize, ysize;
		glfwGetWindowSize(window, &xsize, &ysize);
		glViewport(0, 0, xsize, ysize);

		//Set pause properly; instead of holding escape to keep the game paused, every time you press it it should pause/unpause (provided you relased it)
		bool escapeIsDown = glfwGetKey(window, GLFW_KEY_ESCAPE);
		if (escapeIsDown && !escapeWasDown) {
			paused = !paused;
		}
		escapeWasDown = escapeIsDown;
		glUniform1i(pausedLocation, paused);

		//Slow down time if space is held down and not paused
		if (!paused) {
			bool spaceIsDown = glfwGetKey(window, GLFW_KEY_SPACE);
			if (spaceIsDown) {
				if (speedMultiplier > 0.25f) {
					speedMultiplier -= 0.01f;
				}
				else {
					speedMultiplier = 0.25f;
				}
			}
			else {
				if (speedMultiplier < 1.0f) {
					speedMultiplier += 0.01f;
				}
				else {
					speedMultiplier = 1.0f;
				}
			}
			glUniform1f(speedMultLocation, speedMultiplier);
		}

		//If not paused, move all of the triangles
		if (!paused) {
			glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
			float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			//For loop will become more useful when there's more triangles
			for (int triangleNum = 0; triangleNum < numTriangles; ++triangleNum) {
				for (int i = 0; i < numVertices; ++i) {
					vertices[triangleNum * 6 + i * 2 + 1] += triangleSpeeds[triangleNum][1] * speedMultiplier;
					vertices[triangleNum * 6 + i * 2] += triangleSpeeds[triangleNum][0] * speedMultiplier;
					//If All vertices are below the bottom of the screen or off a side, reset it to the top
					if ((abs(vertices[triangleNum * 6 + 1]) >= 1 && abs(vertices[triangleNum * 6 + 3]) >= -1 && abs(vertices[triangleNum * 6 + 5]) >= 1) || (abs(vertices[triangleNum * 6]) >= 1 && abs(vertices[triangleNum * 6 + 2]) >= -1 && abs(vertices[triangleNum * 6 + 4]) >= 1)) {
						//NOTE: Triangle isn't actually equilateral unless screen is square since coords are by screen
						horizontalOffset = (((float)rand() / (RAND_MAX)) - 0.5f)/4;
						vertices[triangleNum * 6 + 0] = horizontalOffset - (sideLength / 2);
						vertices[triangleNum * 6 + 1] = 1.0; //Vertical displacement = 1.0 at top
						vertices[triangleNum * 6 + 2] = horizontalOffset + (sideLength / 2);
						vertices[triangleNum * 6 + 3] = 1.0;
						vertices[triangleNum * 6 + 4] = horizontalOffset;
						vertices[triangleNum * 6 + 5] = 1.0 + (sideLength * (1.732 / 2)); //Height of an equilateral triangle is sqrt(3)/2 * side length
						triangleSpeeds[triangleNum][0] = (((float)rand() / (RAND_MAX)) - 0.5f)* -0.01f; //Guaranteeing that it'll be at least half speed of 0.01
						triangleSpeeds[triangleNum][1] = (((float)rand() / (RAND_MAX)) / 2 + 0.5f)* -0.01f; //Guaranteeing that it'll be at least half speed of 0.01
					}
				}
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}


		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearBufferfv(GL_COLOR, 0, black);

		glUseProgram(program);

		//Drawing with Line loops to make unfilled triangles instead of filled ones for aesthetic
		for (int i = 0; i < numTriangles; ++i) {
			glDrawArrays(GL_LINE_LOOP, i * 3, numVertices);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

GLuint CreateShader(GLenum shaderType, std::string filename) {
	GLuint shader = glCreateShader(shaderType);
	std::ifstream is(filename);
	std::string source;
	std::string line;
	while (!is.eof()) {
		std::getline(is, line);
		source += line + '\n';
	}
	const GLchar* sourceChar(source.c_str());
	glShaderSource(shader, 1, &sourceChar, 0);
	glCompileShader(shader);
	return shader;
}