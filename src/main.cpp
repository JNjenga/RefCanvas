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

void resize_clb(GLFWwindow*win, int w, int h)
{
	glViewport(0,0,w,h);
	st->scr_height = h;
	st->scr_width= w;

}

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

int main()
{
	GLFWwindow * window = init_engine(720, 720);
		glCheckError();

	if(window == NULL)
		return -1;

    glClearColor(0.0f, 0.7f, 0.6f, 1.0f);

	int index = 0; 
	while(!glfwWindowShouldClose(window))
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for(Item & i : items)
        {
            render_quad(i.x, i.y, i.sx, i.sy, i.r,i.t, i.selected);

			// Check selected item
			if(st->keys[GLFW_MOUSE_BUTTON_RIGHT] == K_PRESS)
			{
				selected = -1;
			}

			else if(st->keys[GLFW_MOUSE_BUTTON_LEFT] == K_PRESS)
			{
				if(selected == index)
				{
					i.selected = false;
					selected = -1;
				}
				else if(st->mx >= i.x && st->mx < (i.x + i.sx) && 
					st->my >= i.y && st->my < (i.y + i.sy))
				{
					selected = index;
					i.selected = true;
				}
			}

			index++;
        }

		if(selected != -1 )
		{
			if(st->keys[GLFW_KEY_G] == K_PRESS)
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
		glCheckError();
        glfwSwapBuffers(window);
        glfwPollEvents();
		index = 0;
	}

	glfwTerminate();
	on_exit();

}

