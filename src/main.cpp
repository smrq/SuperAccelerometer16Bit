#include <cstdio>
#include <cmath>

#include <string>
#include <fstream>
#include <stdexcept>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"

#include "Exceptions.h"
#include "FileIO.h"
#include "RingBuffer.h"
#include "Shader.h"
#include "TransformationMatrix.h"
#include "Vector3f.h"

///////////////////////////////////////////////////////////////////////////////
// Constants

const int GRAPH_HISTORY_SIZE = 256;

std::string VERTEX_SHADER_FILE = "shader.vert";
std::string FRAGMENT_SHADER_FILE = "shader.frag";

///////////////////////////////////////////////////////////////////////////////
// Types

struct Model {
	float position;
	float velocity;
	float acceleration;
	Model(float p, float v, float a):position(p),velocity(v),acceleration(a) {}
};

///////////////////////////////////////////////////////////////////////////////
// Globals

SDL_Surface  *g_Surface;
SDL_Joystick *g_Accelerometer;

int g_iProjectionMatrix,
    g_iModelviewMatrix,
    g_iColor;

TransformationMatrix *g_ProjectionMatrix;
Shader *g_Shader;

Model g_Model(0.0f, 0.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
// Initialization

// Initialize the OpenGL system
void InitializeGL()
{
	// Set up the GLSL shader
	g_Shader = new Shader(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    
	// Retrieve our uniforms
    g_iProjectionMatrix = glGetUniformLocation(g_Shader->id(), "ProjectionMatrix");
    g_iModelviewMatrix  = glGetUniformLocation(g_Shader->id(), "ModelviewMatrix");
    g_iColor            = glGetUniformLocation(g_Shader->id(), "Color");

    // Set up the Projection matrix
	g_ProjectionMatrix = new TransformationMatrix();
	
	//g_ProjectionMatrix->perspectiveMatrix(g_Surface->h, g_Surface->w, 70.0f, 0.1f, 200.0f);
	g_ProjectionMatrix->orthographicMatrix(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

    // Basic GL setup
    glClearColor    (0.0, 0.0, 0.0, 1.0);
    glEnable        (GL_CULL_FACE);
    glCullFace      (GL_BACK);
}

// Initialize the SDL system
void InitializeSDL()
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
    g_Surface = SDL_SetVideoMode(0, 0, 0, SDL_OPENGL);

	// Initialize the accelerometer
    g_Accelerometer = SDL_JoystickOpen(0);
}

// Initialize our program
void Initialize()
{
    InitializeSDL();
    InitializeGL();
}

void Render2DTest()
{
	TransformationMatrix *modelviewMatrix = new TransformationMatrix();

    // Vertex information
    float PtData[][3] = {
        {-1.0f, -1.0f, 0.0f},
        {-1.0f,  1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f}
    };

    // Face information
    unsigned short FaceData[][3] = {
        {0,1,2},
        {2,3,0}
    };

	// Transform the square
	//modelviewMatrix->rotateZ(g_Model.acceleration);
	modelviewMatrix->translate(g_Model.acceleration, 0.0f);
	//modelviewMatrix->scale(1.0f + g_Model.acceleration, 1.0f);
	//modelviewMatrix->scale(1.0f + g_Model.acceleration);

    // Draw the square
    g_Shader->bind();

    glUniformMatrix4fv(g_iProjectionMatrix, 1, false, g_ProjectionMatrix->getRawMatrix());
    glUniformMatrix4fv(g_iModelviewMatrix, 1, false, modelviewMatrix->getRawMatrix());
    glUniform3f       (g_iColor, 0.8f, 0.3f, 0.5f);

    glVertexAttribPointer(Shader::ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, &PtData[0][0]);

    glDrawElements       (GL_TRIANGLES, sizeof(FaceData) / sizeof(unsigned short), 
                          GL_UNSIGNED_SHORT, &FaceData[0][0]);

	g_Shader->unbind();
}

void Render()
{
    // Clear the screen
    glClear (GL_COLOR_BUFFER_BIT);
	Render2DTest();
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
}

///////////////////////////////////////////////////////////////////////////////
// Input polling

float GetTrueYAcceleration(Vector3f acceleration)
{
	const float g = 1.0f;
	float mag = acceleration.magnitude();
	// gy^2 should never be negative obviously, but it might be barely
	// negative due to floating point error for very low true acceleration
	float gy = sqrt( fabs(acceleration.y * acceleration.y + g * g - mag * mag) );
    float a1 = acceleration.y + gy;
	float a2 = acceleration.y - gy;

	// Return a1 or a2, whichever is closer to 0
	return (fabs(a1) < fabs(a2))
        ? a1
        : a2;
}

void PollInput()
{
    Vector3f a;
    a.x = (float) SDL_JoystickGetAxis(g_Accelerometer, 0) / 32768.0;
    a.y = (float) SDL_JoystickGetAxis(g_Accelerometer, 1) / 32768.0;
    a.z = (float) SDL_JoystickGetAxis(g_Accelerometer, 2) / 32768.0;
	float trueAcceleration = GetTrueYAcceleration(a);

	if (fabs(trueAcceleration) > 0.05f) {
		g_Model.acceleration = trueAcceleration;
	}
	else
	{
		g_Model.acceleration = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Main loop

int main(int argc, char** argv)
{
	Initialize();

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
