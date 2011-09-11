/**
    Palm disclaimer
**/
#include <stdio.h>
#include <math.h>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"

#include "RingBuffer.h"
#include "Vector3f.h"

SDL_Surface *Surface;               // Screen surface to retrieve width/height information

int         Program;                // Totalling one program
float       ProjectionMatrix[4][4]; // Projection matrix
int         iProjectionMatrix,      // Our uniforms
            iModelviewMatrix,
            iColor;

///////////////////////////////////////////////////////////////////////////////
// Globals

typedef struct {
	RingBuffer<Vector3f> *data;
	int dataCount;
	Vector3f calibratedG;
} AccelerometerData;
AccelerometerData g_AccelerometerData;

typedef struct {
	float position;
	float velocity;
	float acceleration;
	float angle; // Angle for test shape
} Model;
Model g_Model;

SDL_Joystick *g_Joystick;

///////////////////////////////////////////////////////////////////////////////
// Initialization

bool InitializeGlobals()
{
	g_Model.position = 0.0f;
	g_Model.velocity = 0.0f;
	g_Model.acceleration = 0.0f;
	g_Model.angle = 0.0f;

	g_AccelerometerData.dataCount = 16;
	g_AccelerometerData.data = new RingBuffer<Vector3f>(g_AccelerometerData.dataCount);
	g_AccelerometerData.calibratedG.x = 0.0f;
	g_AccelerometerData.calibratedG.y = 0.0f;
	g_AccelerometerData.calibratedG.z = 0.0f;
	return true;
}

// Standard GL perspective matrix creation
void MakePerspectiveMatrix(float ProjectionMatrix[4][4], const float FOV, const float ZNear, const float ZFar)
{
    memset(ProjectionMatrix, 0, sizeof(ProjectionMatrix));

    ProjectionMatrix[0][0] = 1.0f / tanf(FOV * 3.1415926535f / 360.0f);
    ProjectionMatrix[1][1] = ProjectionMatrix[0][0] / ((float)Surface->h / Surface->w);
    ProjectionMatrix[2][2] = -(ZFar + ZNear) / (ZFar - ZNear);
    ProjectionMatrix[2][3] = -1.0f;
    ProjectionMatrix[3][2] = -2.0f * ZFar * ZNear / (ZFar - ZNear);
}

void MakeOrthographicMatrix(float ProjectionMatrix[4][4], const float left, const float right, const float top, const float bottom, const float near, const float far)
{
    memset(ProjectionMatrix, 0, sizeof(ProjectionMatrix));

	ProjectionMatrix[0][0] = 2.0f / (right - left);
	ProjectionMatrix[1][1] = 2.0f / (top - bottom);
	ProjectionMatrix[2][2] = -2.0f / (far - near);
	ProjectionMatrix[3][0] = -(right + left)/(right - left);
	ProjectionMatrix[3][1] = -(top + bottom)/(top - bottom);
	ProjectionMatrix[3][2] = -(far + near)/(far - near);
	ProjectionMatrix[3][3] = 1.0f;
}

// Simple function to create a shader
void LoadShader(char *Code, int ID)
{
    // Compile the shader code
    glShaderSource  (ID, 1, (const char **)&Code, NULL); 
    glCompileShader (ID);

    // Verify that it worked
    int ShaderStatus;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &ShaderStatus); 

    // Check the compile status
    if (ShaderStatus != GL_TRUE) {
		printf("Error: Failed to compile GLSL program\n");
        int Len = 1024;
        char Error[1024];
        glGetShaderInfoLog(ID, 1024, &Len, Error);
        printf("%s", Error);
        exit (-1);
    }
}

// Initialize our shaders
bool InitializeShader() 
{
    // Very basic ambient+diffusion model
    const char VertexShader[] = "                   \
        attribute vec3 Position;                    \
        attribute vec3 Normal;                      \
                                                    \
        uniform mat4 ProjectionMatrix;              \
        uniform mat4 ModelviewMatrix;               \
                                                    \
        varying vec3 NormVec;                       \
        varying vec3 LighVec;                       \
                                                    \
        void main(void)                             \
        {                                           \
            vec4 Pos = ModelviewMatrix * vec4(Position, 1.0); \
            gl_Position = ProjectionMatrix * Pos;   \
                                                    \
            NormVec     = (ModelviewMatrix * vec4(Normal,0.0)).xyz; \
            LighVec     = -Pos.xyz;                 \
        }                                           \
    ";

    const char FragmentShader[] = "                                             \
        uniform highp vec3 Color;                                               \
                                                                                \
        varying highp vec3 NormVec;                                             \
        varying highp vec3 LighVec;                                             \
                                                                                \
        void main(void)                                                         \
        {                                                                       \
            mediump vec3 Norm  = normalize(NormVec);                            \
            mediump vec3 Light = normalize(LighVec);                            \
                                                                                \
            mediump float Diffuse = dot(Norm, Light);                           \
                                                                                \
            gl_FragColor = vec4(Color * (max(Diffuse, 0.0) * 0.6 + 0.4), 0.5);  \
        }                                                                       \
    ";

    // Create 2 shader programs
    int vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    LoadShader((char *)VertexShader, vertexShader);
    LoadShader((char *)FragmentShader, fragmentShader);

    // Create the prorgam and attach the shaders & attributes
    Program   = glCreateProgram();

    glAttachShader(Program, vertexShader);
    glAttachShader(Program, fragmentShader);

    glBindAttribLocation(Program, 0, "Position");
    glBindAttribLocation(Program, 1, "Normal");

    // Link
    glLinkProgram(Program);

    // Validate our work thus far
    int ShaderStatus;
    glGetProgramiv(Program, GL_LINK_STATUS, &ShaderStatus); 

    if (ShaderStatus != GL_TRUE) {
        printf("Error: Failed to link GLSL program\n");
        int Len = 1024;
        char Error[1024];
        glGetProgramInfoLog(Program, 1024, &Len, Error);
        printf("%s",Error);
        return false;
    }

    glValidateProgram(Program);

    glGetProgramiv(Program, GL_VALIDATE_STATUS, &ShaderStatus); 

    if (ShaderStatus != GL_TRUE) {
        printf("Error: Failed to validate GLSL program\n");
        return false;
    }

    // Enable the program
    glUseProgram                (Program);
    glEnableVertexAttribArray   (0);
    glEnableVertexAttribArray   (1);

    // Retrieve our uniforms
    iProjectionMatrix = glGetUniformLocation(Program, "ProjectionMatrix");
    iModelviewMatrix  = glGetUniformLocation(Program, "ModelviewMatrix");
    iColor            = glGetUniformLocation(Program, "Color");

    return true;
}

// Initialize the OpenGL system
bool InitializeGL()
{
    if (!InitializeShader())
        return false;

    // Setup the Projection matrix
    MakePerspectiveMatrix(ProjectionMatrix, 70.0f, 0.1f, 200.0f);
	//MakeOrthographicMatrix(ProjectionMatrix, 0.0f, (float)Surface->w, 0.0f, (float)Surface->h, 0.1f, 200.0f);

    // Basic GL setup
    glClearColor    (0.0, 0.0, 0.0, 1.0);
    glEnable        (GL_CULL_FACE);
    glCullFace      (GL_BACK);

    return true;
}

// Initialize the accelerometer
bool InitializeAccelerometer()
{
    g_Joystick = SDL_JoystickOpen(0);
    return true;
}

// Initialize the SDL system
bool InitializeSDL()
{
    // Initialize the SDL library with the Video subsystem
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE | SDL_INIT_JOYSTICK);
    atexit(SDL_Quit);
    
    // start the PDL library
    PDL_Init(0);
    atexit(PDL_Quit);
    
    // Tell it to use OpenGL version 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

    // Set the video mode to full screen with OpenGL-ES support
    // use zero for width/height to use maximum resolution
    Surface = SDL_SetVideoMode(0, 0, 0, SDL_OPENGL);

    return true;
}

// Initialize our program
bool Initialize()
{
    if (!InitializeSDL())
        return false;

    if (!InitializeGL())
        return false;

    if (!InitializeAccelerometer())
        return false;

    if (!InitializeGlobals())
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Rendering

// Main-loop workhorse function for displaying the object
void Render()
{
    // Clear the screen
    glClear (GL_COLOR_BUFFER_BIT);

    float ModelviewMatrix[4][4];

    memset(ModelviewMatrix, 0, sizeof(ModelviewMatrix));

    // Setup the modelview matrix so that the object rotates around the Y axis
    // We'll also translate it appropriately to Display
    ModelviewMatrix[0][0] = cosf(g_Model.angle);
    ModelviewMatrix[1][1] = 1.0f;
    ModelviewMatrix[2][0] = sinf(g_Model.angle);
    ModelviewMatrix[0][2] = -sinf(g_Model.angle);
    ModelviewMatrix[2][2] = cos(g_Model.angle);
    ModelviewMatrix[3][2] = -1.0f;   
    ModelviewMatrix[3][3] = 1.0f;

    // Vertex information
    float PtData[][3] = {
        {0.5f, 0.0380823f, 0.028521f},
        {0.182754f, 0.285237f, 0.370816f},
        {0.222318f, -0.2413f, 0.38028f},
        {0.263663f, -0.410832f, -0.118163f},
        {0.249651f, 0.0109279f, -0.435681f},
        {0.199647f, 0.441122f, -0.133476f},
        {-0.249651f, -0.0109279f, 0.435681f},
        {-0.263663f, 0.410832f, 0.118163f},
        {-0.199647f, -0.441122f, 0.133476f},
        {-0.182754f, -0.285237f, -0.370816f},
        {-0.222318f, 0.2413f, -0.38028f},
        {-0.5f, -0.0380823f, -0.028521f},
    };

    // Face information
    unsigned short FaceData[][3] = {
        {0,1,2,},
        {0,2,3,},
        {0,3,4,},
        {0,4,5,},
        {0,5,1,},
        {1,5,7,},
        {1,7,6,},
        {1,6,2,},
        {2,6,8,},
        {2,8,3,},
        {3,8,9,},
        {3,9,4,},
        {4,9,10,},
        {4,10,5,},
        {5,10,7,},
        {6,7,11,},
        {6,11,8,},
        {7,10,11,},
        {8,11,9,},
        {9,11,10,},
    };


    // Draw the icosahedron
    glUseProgram            (Program);
    glUniformMatrix4fv      (iProjectionMatrix, 1, false, (const float *)&ProjectionMatrix[0][0]);
    glUniformMatrix4fv      (iModelviewMatrix, 1, false, (const float *)&ModelviewMatrix[0][0]);
    glUniform3f             (iColor, 0.8f, 0.3f, 0.5f);

    glVertexAttribPointer   (0, 3, GL_FLOAT, 0, 0, &PtData[0][0]);
    glVertexAttribPointer   (1, 3, GL_FLOAT, GL_TRUE, 0, &PtData[0][0]);

    glDrawElements          (GL_TRIANGLES, sizeof(FaceData) / sizeof(unsigned short), 
                             GL_UNSIGNED_SHORT, &FaceData[0][0]);

    SDL_GL_SwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////
// Game logic

void evaluate(float initialX, float initialV, float a, float dx, float dv, float t, float dt, float *resultdx, float *resultdv)
{
	float x = initialX + dx * dt;
	float v = initialV + dv * dt;
	*resultdx = v;
	*resultdv = a;
}

void integrate(float x, float v, float a, float t, float dt, float *resultx, float *resultv)
{
	float adx, adv, bdx, bdv, cdx, cdv, ddx, ddv;
	evaluate(x, v, a, 0.0f, 0.0f, t,           0.0f,    &adx, &adv);
	evaluate(x, v, a, adx,  adv,  t + dt*0.5f, dt*0.5f, &bdx, &bdv);
	evaluate(x, v, a, bdx,  bdv,  t + dt*0.5f, dt*0.5f, &cdx, &cdv);
	evaluate(x, v, a, cdx,  cdv,  t + dt,      dt,      &ddx, &ddv);

	const float dxdt = 1.0f/6.0f * (adx + 2.0f*(bdx + cdx) + ddx);
	const float dvdt = 1.0f/6.0f * (adv + 2.0f*(bdv + cdv) + ddv);

	*resultx = x + dxdt * dt;
	*resultv = v + dvdt * dt;
}

void Update(int t, int dt)
{
    integrate(g_Model.position, g_Model.velocity, g_Model.acceleration,
              0.001f * t, 0.001f * dt,
              &g_Model.position, &g_Model.velocity);
	printf("X: %.5f, V: %.5f, A: %.5f\n", g_Model.position, g_Model.velocity, g_Model.acceleration);

    // Constantly rotate the object as a function of time
    g_Model.angle += g_Model.velocity * dt * 0.01f;

}

///////////////////////////////////////////////////////////////////////////////
// Input polling

Vector3f GetTrueAcceleration(Vector3f acceleration)
{
	// Update average G vector
	float mag = acceleration.magnitude();
	if (0.95f < mag && mag < 1.05f) {
		Vector3f old = g_AccelerometerData.data->push(acceleration);
		g_AccelerometerData.calibratedG += ((acceleration - old) / g_AccelerometerData.dataCount);
//		printf("New calibrated G: %.5f, %.5f, %.5f\n", g_AccelerometerData.calibratedG.x, g_AccelerometerData.calibratedG.y, g_AccelerometerData.calibratedG.z);
		return Vector3f();
	}

	// Subtract G from acceleration
	return acceleration - g_AccelerometerData.calibratedG;
}

void PollInput()
{
    Vector3f a;
    a.x = (float) SDL_JoystickGetAxis(g_Joystick, 0) / 32768.0;
    a.y = (float) SDL_JoystickGetAxis(g_Joystick, 1) / 32768.0;
    a.z = (float) SDL_JoystickGetAxis(g_Joystick, 2) / 32768.0;
	
	Vector3f trueAcceleration = GetTrueAcceleration(a);
//	printf("Acceleration input: %.5f, %.5f, %.5f\n", a.x, a.y, a.z);
//	printf("Calibrated G:       %.5f, %.5f, %.5f\n", g_AccelerometerData.calibratedG.x, g_AccelerometerData.calibratedG.y, 	g_AccelerometerData.calibratedG.z);
//	printf("True acceleration:  %.5f, %.5f, %.5f\n", trueAcceleration.x, trueAcceleration.y, trueAcceleration.z);

	g_Model.acceleration = trueAcceleration.y;
}

///////////////////////////////////////////////////////////////////////////////
// Main loop

int main(int argc, char** argv)
{
	if (!Initialize())
		return -1;

    SDL_Event Event;
    bool paused = false;

	// Timestep variables
	const unsigned int dt = 33; // Update physics at ~30fps
	const unsigned int maxFrameTime = 250; // Slow down physics simulation if going slower than 4fps
	unsigned int currentTime = SDL_GetTicks(),
				 newTime = 0,
				 frameTime = 0,
				 accumulator = 0,
				 t = 0;

    while (1) {

        /////////////////////////////////////////////////////////////////////////////
        // Timing for game loop

        newTime = SDL_GetTicks();
        frameTime = newTime - currentTime;
        if (frameTime > maxFrameTime) {
            frameTime = maxFrameTime;
        }
        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= dt) {
            Update(t, dt);
            accumulator -= dt;
            t += dt;
        }

        /////////////////////////////////////////////////////////////////////////////
        // Input polling

        PollInput();

        /////////////////////////////////////////////////////////////////////////////
        // Event handling
		
        bool gotEvent;
        if (paused) {
            SDL_WaitEvent(&Event);
            gotEvent = true;
        }
        else {
            gotEvent = SDL_PollEvent(&Event);
        }
        
        while (gotEvent) {
            switch (Event.type) {
                // List of keys that have been pressed
                case SDL_KEYDOWN:
                    switch (Event.key.keysym.sym) {
                        case PDLK_GESTURE_BACK: /* also maps to ESC */
                            if (PDL_GetPDKVersion() >= 200) {
                                // standard behavior is to minimize to a card when you perform a back
                                // gesture at the top level of the app
                                PDL_Minimize();
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_ACTIVEEVENT:
                    if (Event.active.state == SDL_APPACTIVE) {
                        paused = !Event.active.gain;
                    }
                    break;

                case SDL_QUIT:
                    // We exit anytime we get a request to quit the app
                    // all shutdown code is registered via atexit() so this is clean.
                    exit(0);
                    break;

                default:
                    break;
            }
            gotEvent = SDL_PollEvent(&Event);
        }

        /////////////////////////////////////////////////////////////////////////////
        // Rendering

        Render();
    }

	// What are you doing here?
    return 0;
}
