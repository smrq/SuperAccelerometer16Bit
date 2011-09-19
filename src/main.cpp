#include <cstdio>
#include <cmath>

#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"

#include "Animation.h"
#include "Exceptions.h"
#include "FileIO.h"
#include "RingBuffer.h"
#include "Shader.h"
#include "TransformationMatrix.h"
#include "Vector3f.h"

///////////////////////////////////////////////////////////////////////////////
// Constants

const int GRAPH_HISTORY_SIZE = 256;

const std::string VERTEX_SHADER_FILE = "shader.vert";
const std::string FRAGMENT_SHADER_FILE = "shader.frag";

///////////////////////////////////////////////////////////////////////////////
// Types

struct Model {
	float velocity;
	float acceleration;
	float jerk;
	Model(float v=0.0f, float a=0.0f, float j=0.0f)
		:velocity(v),acceleration(a),jerk(j) {}
};

///////////////////////////////////////////////////////////////////////////////
// Globals

SDL_Surface  *g_Surface;
SDL_Joystick *g_Accelerometer;

TransformationMatrix *g_ProjectionMatrix;
Shader *g_Shader;
Animation *g_Animation;

Model g_Model;

///////////////////////////////////////////////////////////////////////////////
// Initialization

// Initialize the OpenGL system
void InitializeGL()
{
	// Set up the GLSL shader
	g_Shader = new Shader(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    
    // Set up the Projection matrix
	g_ProjectionMatrix = new TransformationMatrix();
	
	//g_ProjectionMatrix->perspectiveMatrix(g_Surface->h, g_Surface->w, 70.0f, 0.1f, 200.0f);
	g_ProjectionMatrix->orthographicMatrix(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

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

// Initialize animations
void InitializeAnimations()
{
	std::vector <std::string> frames;
	frames.push_back("animation1/frame01.jpg");
	frames.push_back("animation1/frame02.jpg");
	frames.push_back("animation1/frame03.jpg");
	frames.push_back("animation1/frame04.jpg");
	frames.push_back("animation1/frame05.jpg");
	frames.push_back("animation1/frame06.jpg");
	frames.push_back("animation1/frame07.jpg");
	frames.push_back("animation1/frame08.jpg");

	g_Animation = new Animation(frames);
}

// Initialize our program
void Initialize()
{
    InitializeSDL();
    InitializeGL();
	InitializeAnimations();
}

///////////////////////////////////////////////////////////////////////////////
// Rendering

int GetFrameNumber(float acceleration)
{
	int frameCount = g_Animation->frameCount();
	float normalizationConstant = 0.5f; // determine via calibration?
	float normalizedAcceleration = acceleration * normalizationConstant;

	int frame = frameCount / 2;
	frame += (frameCount / 2) * normalizedAcceleration;
	if (frame < 0)           { frame = 0; }
	if (frame >= frameCount) { frame = frameCount - 1; }

	return frame;
}

void RenderImage()
{	
	// Get animation frame
	int frame = GetFrameNumber(g_Model.acceleration);

	// Get model coordinates
	float vertexCoords[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	// Clip texture to fill screen
	float textureAspectRatio = g_Animation->aspectRatio(frame);
	float screenAspectRatio = ((float)g_Surface->h) / ((float)g_Surface->w);

	float wMin, wMax, hMin, hMax;
	if (textureAspectRatio > screenAspectRatio) {
		wMin = (1.0f - (screenAspectRatio / textureAspectRatio)) / 2;
		wMax = (1.0f + (screenAspectRatio / textureAspectRatio)) / 2;
		hMin = 0.0f;
		hMax = 1.0f;
	}
	else {
		wMin = 0.0f;
		wMax = 1.0f;
		hMin = (1.0f - (textureAspectRatio / screenAspectRatio)) / 2;
		hMax = (1.0f + (screenAspectRatio / screenAspectRatio)) / 2;
	}

	// Crop texture border
	float wSize = g_Animation->frameWCoord(frame);
	float hSize = g_Animation->frameHCoord(frame);
	wMin *= wSize;
	wMax *= wSize;
	hMin *= hSize;
	hMax *= hSize;

	// Get texture coordinates
	float textureCoords[] = {
		wMax, hMin,
		wMin, hMin,
		wMax, hMax,
		wMin, hMax
	};

	// Set up modelview matrix
	TransformationMatrix *modelviewMatrix = new TransformationMatrix();

	// Bind shader
    g_Shader->bind();

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	g_Animation->bindFrame(frame);
	
	// Set up uniforms
    int iProjectionMatrix = g_Shader->uniform("ProjectionMatrix");
    int iModelviewMatrix  = g_Shader->uniform("ModelviewMatrix");
	int iTexture          = g_Shader->uniform("Texture");
    glUniformMatrix4fv(iProjectionMatrix, 1, false, g_ProjectionMatrix->getRawMatrix());
    glUniformMatrix4fv(iModelviewMatrix, 1, false, modelviewMatrix->getRawMatrix());
	glUniform1i(iTexture, 0);

	// Bind coordinates
    glVertexAttribPointer(Shader::ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, vertexCoords);
	glEnableVertexAttribArray(Shader::ATTRIB_POSITION);
    glVertexAttribPointer(Shader::ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
	glEnableVertexAttribArray(Shader::ATTRIB_TEXCOORD);

	// Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	g_Shader->unbind();
}

void Render()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
	RenderImage();
    SDL_GL_SwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////
// Game logic

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

void UpdateModel(const float dt)
{
    Vector3f accelerometerInput;
    accelerometerInput.x = (float) SDL_JoystickGetAxis(g_Accelerometer, 0) / 32768.0;
    accelerometerInput.y = (float) SDL_JoystickGetAxis(g_Accelerometer, 1) / 32768.0;
    accelerometerInput.z = (float) SDL_JoystickGetAxis(g_Accelerometer, 2) / 32768.0;
	float acceleration = GetTrueYAcceleration(accelerometerInput);
	
	g_Model.jerk = (acceleration - g_Model.acceleration) / dt;
	g_Model.acceleration = acceleration;
	g_Model.velocity += acceleration * dt;

	//printf("V: %.5f, A: %.5f, J: %.5f\n",
	//		g_Model.velocity, g_Model.acceleration, g_Model.jerk);
}

void Update(const int t, const int dt)
{
	float dtSec = 0.001f * dt;
    UpdateModel(dtSec);
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
