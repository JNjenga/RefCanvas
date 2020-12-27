#include "engine.h"
#include <unordered_map>
#include <iostream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



std::unordered_map<const char *, int> uniforms;
int shader_id;
int quad_id;

Gsettings *st;

void key_clb(GLFWwindow * w, int key, int scancode, int action, int modes)
{
    if(action == GLFW_PRESS)
    {
        st->keys[key] = K_PRESS;
    }else if(action == GLFW_RELEASE)
    {
        st->keys[key] = K_RELEASE;
    }
}

void mkey_clb(GLFWwindow * w, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        st->keys[button] = K_PRESS;
    }else if(action == GLFW_RELEASE)
    {
        st->keys[button] = K_RELEASE;
    }
}

extern void resize_clb(GLFWwindow*win, int w, int h);
extern void drop_clb(GLFWwindow * win, int count, const char ** paths);

extern void mpos_clb(GLFWwindow* win, double xpos, double ypos);

GLFWwindow * init_engine(int width, int height)
{
	if(glfwInit() != GLFW_TRUE)
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = 
		glfwCreateWindow(width, height, "Ref Canvas", NULL, NULL);

	if(window == NULL)
	{
		glfwTerminate();
		return NULL;
	}
    
    // Create settings
    st = new Gsettings{NULL, 0.0f, 0.0f, 0.0f,0.0f, width, height};

    st->keys = new int[1024]; 

	st->scr_height = height;
	st->scr_width = width;

	glfwSetKeyCallback(window, key_clb);
	glfwSetMouseButtonCallback(window, mkey_clb);
    glfwSetDropCallback(window, drop_clb);
    glfwSetCursorPosCallback(window, mpos_clb);

	auto fb_clbk = [](GLFWwindow *win, int w, int h){
		glViewport(0,0,w,h);
		st->scr_height = h;
		st->scr_width = w;
        glm::mat4 p_mat = glm::ortho(0.0f, static_cast<float>(w),  static_cast<float>(h), 0.0f, -1.0f, 100.0f);
        set_uniform("p_mat", p_mat);
	};

	glfwSetFramebufferSizeCallback(window, fb_clbk);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(5);

	if(glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init glew" << std::endl;

		glfwTerminate();
		return NULL;
	}

	shader_id = create_shader();
	glUseProgram(shader_id);
    glm::mat4 p_mat = glm::ortho(0.0f, static_cast<float>(st->scr_width),  static_cast<float>(st->scr_height), 0.0f, -1.0f, 100.0f);
    set_uniform("p_mat", p_mat);

    quad_id = create_quad();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return window;
}

int create_shader()
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"uniform mat4 m_mat = mat4(1.0f);\n"
		"uniform mat4 v_mat = mat4(1.0f);;\n"
		"uniform mat4 p_mat = mat4(1.0f);\n"
		"out vec2 tex_coord;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = p_mat * v_mat * m_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"   tex_coord =  aTexCoord;\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 tex_coord;\n"
		"uniform int selected;\n"
		"uniform vec3 color;\n"
		"uniform sampler2D tex;\n"
		"void main()\n"
		"{\n"
		"   if(selected == -1)\n"
		"       FragColor = texture(tex, tex_coord);\n"
        "   else\n"
		"       FragColor = vec4(1.0f, 0.5f,0.5f,1.0f) * texture(tex, tex_coord);\n"
		"}\n\0";

	int s;
	unsigned int v, f;
	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vertexShaderSource, NULL);
	glCompileShader(v);

	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &fragmentShaderSource, NULL);
	glCompileShader(f);

	s = glCreateProgram();
	glAttachShader(s, v);
	glAttachShader(s, f);

	glLinkProgram(s);

	glDeleteShader(v);
	glDeleteShader(f);
	glCheckError();
	return s;
}

int get_uniform_loc(const char *name)
{
	auto it = uniforms.find(name);

	if(it != uniforms.end())
		return it->second;

	int index = glGetUniformLocation(shader_id, name);
	if(index == -1)
		uniforms.insert({name, index});

	return index;
}

void set_uniform(const char *name, float value)
{
	glUniform1f(get_uniform_loc(name), value);
}

void set_uniform(const char *name, int value)
{
	glUniform1i(get_uniform_loc(name), value);
}

void set_uniform(const char *name, glm::vec3 value)
{
	glUniform3f(get_uniform_loc(name),value.x, value.y, value.z);
}

void set_uniform(const char *name, glm::mat4 value)
{
	glUniformMatrix4fv(get_uniform_loc(name),1,GL_FALSE, glm::value_ptr(value));	
}

int create_quad()
{
	float vertices[] = {
		// Front
		0.0f, 0.0f, 0.0f, // Top left
		1.0f, 0.0f, 0.0f, // Top right
		1.0f, 1.0f, 0.0f, // Bottom right
		1.0f, 1.0f, 0.0f, // Bottom right
		0.0f, 0.0f, 0.0f, // Top left
		0.0f, 1.0f, 0.0f, // Bottom left
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	unsigned int tbo;

	float t_coords [] = {
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		1.0f,0.0f,
		0.0f,1.0f,
		0.0f,0.0f,
	};

	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t_coords), t_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);

	return vao;
}

int create_texture(const char *path, int& width, int& height)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int nrChannels;
    stbi_set_flip_vertically_on_load(true);
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        if(nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }
    stbi_image_free(data);
	return texture;
}

void render_texture(int texture)
{
	glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(shader_id);

	glBindVertexArray(quad_id);
	// glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void render_quad(float x, float y, float sx, float sy, float r, int t, bool selected)
{

	glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, t);
	glUseProgram(shader_id);
    if(selected)
        set_uniform("selected", 1);
    else
        set_uniform("selected", -1);
	glm::mat4 m_mat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
	m_mat = glm::scale(m_mat, glm::vec3(sx, sy, 1.0f));
	set_uniform("m_mat", m_mat);
	glBindVertexArray(quad_id);
	// glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6); 
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:
				error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:
				error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:
									   error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:
									   error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:
									   error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
									   error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}

		printf("%s | %s (%d) \n", error.c_str(), file, line);
	}
	return errorCode;
}

void print_st()
{
    std::cout << "MX : " << st->mx;
    std::cout << "MY : " << st->my;
    std::cout << "DMX : " << st->dmx;
    std::cout << "DMY : " << st->dmy;
    std::cout << "\n";

}

void on_exit()
{
    delete [] st->keys;
    delete st;
}

bool is_hovered(float x, float y, float sx, float sy)
{
    if(st->mx >= x && st->mx < (x + sx) && st->my >= y && st->my < (y + sy))
    {
        return true;
    }

    return false;
}
