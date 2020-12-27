/*
 * Opengl boiler plate code.
 * Initilize engine
 * Create and use shaders
 * Set up uniforms for shader
 * Create and render square 
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#define glCheckError() glCheckError_(__FILE__, __LINE__)

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

// Globals
struct Gsettings
{
	int *keys;
	float mx, my;
	float dmx, dmy;
	int scr_height;
	int scr_width;
};

#define K_PRESS 1
#define K_RELEASE 0

GLFWwindow * init_engine(int width, int height);
// extern void mpos_clb(GLFWwindow* win, double xpos, double ypos);
void mkey_clb(GLFWwindow * w, int button, int action, int mods);
void key_clb(GLFWwindow * w, int key, int scancode, int action, int modes);
void resize_clb(GLFWwindow*win, int w, int h);

void on_exit();
bool is_hovered(float x, float y, float sx, float sy);

// Shader stuff
int create_shader();
int get_uniform_loc(const char *name);
void set_uniform(const char * name, float value);
void set_uniform(const char * name, int value);
void set_uniform(const char * name, glm::vec3 value);
void set_uniform(const char * name, glm::mat4 value);

// Rendering
int create_quad();
int create_texture(const char *path, int& w, int& h);
void render_texture(int texture);
void render_quad(float x, float y, float sx, float sy, float r, int t, bool selected);

GLenum glCheckError_(const char *file, int line);
void print_st();
#endif
