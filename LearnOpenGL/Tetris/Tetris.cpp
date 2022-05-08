#include "../glad/glad.h"
#include <glfw3.h>
#include <iostream>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../common/shader/shader_s.h"

// 隐藏控制台窗口
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void check_timer(double time);

// 宽高暂时设置为相同，避免图形被拉伸
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

float up_distance = 0.9f;
float right_distance = 0.0f;
float rotate_angle = 0.0f;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("../common/glsl/tetris.vs", "../common/glsl/tetris.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.1f, -0.1f, 0.0f,  // 0
		-0.1f, 0.0f, 0.0f,  // 1
		0.0f, -0.1f, 0.0f,  // 2
		0.0f, 0.0f, 0.0f,  // 3
		-0.1f, 0.1f, 0.0f,  // 4
		0.0f, 0.1f, 0.0f,  // 5
		0.1f, 0.0f, 0.0f,  // 6
		0.1f, -0.1f, 0.0f,  // 7

		0.2f, -0.1f, 0.0f,  // 8
		0.2f, 0.0f, 0.0f,  // 9
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,  // first Triangle
		1, 2, 3,   // second Triangle
		3, 4, 1,
		3, 4, 5,
		3, 2, 7,
		3, 6, 7,

		6, 7, 8,
		6, 9, 8,
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		check_timer(glfwGetTime());

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// create transformations
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(right_distance, up_distance, 0.0f));
		transform = glm::rotate(transform, glm::radians(rotate_angle), glm::vec3(0.0f, 0.0f, 1.0f));

		ourShader.use();
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
								//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	//glDeleteProgram(shaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
		{
			if (rotate_angle == -270.0f)
			{
				rotate_angle = 0.0f;
			}
			else
			{
				rotate_angle -= 90.0f;
			}
		}
		break;
		default:
			break;
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		break;
	case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS)
		{
			right_distance -= 0.1f;
			if (right_distance <= -0.9f)
			{
				right_distance = -0.9f;
			}
		}
		break;
	case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS)
		{
			right_distance += 0.1f;
			if (right_distance >= 0.8f)
			{
				right_distance = 0.8f;
			}
		}
		break;
	case GLFW_KEY_DOWN:
		{
			static int count = 0;
			if (action == GLFW_PRESS)
			{
			}
			else if (action == GLFW_REPEAT)
			{
				// 检测长按
				if (++count >= 5)
				{
					up_distance = -0.9f;
				}
			}
			else if (action == GLFW_RELEASE)
			{
				count = 0;
			}
		}
		break;
	}
}

void fall()
{
	if (up_distance <= -0.9f)
	{
		return;
	}
	up_distance -= 0.1f;
}

void on_timer(int count)
{
	fall();
}

void check_timer(double time)
{
	static double previous_time = 0;
	static const double TIMER_INTERVAL_ONE_SECOND = 1.0f;
	static int count = 0;
	if (previous_time == 0)
	{
		previous_time = time;
		return;
	}
	if (time - previous_time >= TIMER_INTERVAL_ONE_SECOND)
	{
		on_timer(++count);
		previous_time = time;
	}
}
