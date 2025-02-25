#include "engine.h"
#include <vector>
#include <iostream>

struct Item
{
    float x, y; // Position
    float sx; // Scale
    float sy; // Scale
    float r; // Rotation
    int t; // Texture ID
	bool selected;
};

std::vector<Item> items;

bool moving = false;
int selected = -1;

extern Gsettings * st;

glm::mat4 v_mat;

void drop_clb(GLFWwindow * win, int count, const char ** paths)
{
	int i, w, h;
	for(i = 0; i < count; i ++)
	{
		int tex = create_texture(paths[i], w, h);
		items.push_back({st->mx, st->my, static_cast<float>(w), static_cast<float>(h),0.0f, tex, false});
	}
}

void mpos_clb(GLFWwindow* win, double xpos, double ypos)
{
    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);

    st->dmx = xposf - st->mx ;
    st->dmy =  yposf - st->my ;

    st->mx = xposf;
    st->my = yposf;

}

void update()
{
	// Panning over the world
	if(st->keys[GLFW_MOUSE_BUTTON_MIDDLE] == K_PRESS)
	{
        v_mat = glm::translate(v_mat, glm::vec3(st->dmx, st->dmy, 0.0f));

        set_uniform("v_mat", v_mat);
	}

    // Deleting, key x

    // Deleting, moving, scaling up and scaling down and object
    if(selected != -1 )
    {
        
        if(st->keys[GLFW_KEY_X] == K_PRESS) 
        {
            GLuint texture_id = (GLuint) items[selected].t;
            glDeleteTextures(1, &texture_id);
            items.erase(items.begin()+selected);
            selected = -1;
        }
        else if(st->keys[GLFW_KEY_G] == K_PRESS)
        {
            items[selected].x += st->dmx;
            items[selected].y += st->dmy;
        }
        else if(st->keys[GLFW_KEY_J] == K_PRESS)
        {
            items[selected].sx += 5.0f;
            items[selected].sy += 5.0f;
        }
        else if(st->keys[GLFW_KEY_K] == K_PRESS)
        {
            items[selected].sx -= 10.2f;
            items[selected].sy -= 10.2f;
        }
    }

	st->dmx = 0.0f;
	st->dmy = 0.0f;
}

int main()
{
	GLFWwindow * window = init_engine(720, 720);
	glCheckError();

	if(window == NULL)
		return -1;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Initialize variables
	v_mat = glm::mat4(1.0f);

	int index = 0; 
	while(!glfwWindowShouldClose(window))
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(Item & i : items)
        {
			// Clear selection 
            if(st->keys[GLFW_MOUSE_BUTTON_LEFT] == K_PRESS)
            {
                if(is_hovered(i.x, i.y, i.sx, i.sy))
                {
                    if(selected == -1)
                    {
                        i.selected = true;
                        selected = index;
                    }
                    else if(selected == index )
                    {
                        i.selected = false; // Disable preselected
                        selected = -1;
                    }
                    else
                    {
                        items[selected].selected = false; // Disable preselected
                        i.selected = true; // Disable preselected
                        selected = index;
                    }
                }
            }

            render_quad(i.x, i.y, i.sx, i.sy, i.r,i.t, i.selected); 

			index++;
        }

		index = 0;

		update();

		// glCheckError();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}

	glfwTerminate();
	on_exit();

}

