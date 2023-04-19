#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "ShaderProgram.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/SceneClearColor.h"

const int WIDTH = 1200;
const int HEIGHT = 720;
bool WIRE_MODE = false;

static float mouseX;
static float mouseY;
int frameCount = 0;
double startTime = glfwGetTime();

double calculateFrameRate();

double mapDouble(double input, double inputMin, double inputMax, double outputMin, double outputMax) {
	// Calculate the percentage of the input within the input range
	return outputMin + ((input - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	mouseX = static_cast<float>(mapDouble(xpos, 0, WIDTH, -1, 1));
	mouseY = static_cast<float>(mapDouble(ypos, 0, HEIGHT, 1, -1));
}
void renderMode()
{
	if (WIRE_MODE)
	{
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW WAS NOT INITIALIZED" << std::endl;
	}

	//glm::mat4 proj = glm::ortho(-2.5f, 2.5f, -1.5f, 1.5f, -1.0f, 1.0f);
	//glm::mat4 proj = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f); // convert from window space to normal device coordinates (-1, 1)
	//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0)); // move everything to the left simulating camera moving to the right
	//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

	//glm::mat4 mvp = proj * view * model; // these need to be multiplie in reverse order because opengl is in column major (right to left)

	ShaderProgram basicShader("res/shaders/Basic.Shader");
	basicShader.Bind();
	//basicShader.SetUniformMat4f("u_MVP", mvp);
	basicShader.SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 1.0f);
	basicShader.UnBind();

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	{
		float positions[] = {
			 -50.0f, -50.0f, 0.0f, 0.0f, // p0 bottom left
			 50.0f, -50.0f, 1.0f, 0.0f, // p1 bottom right
			 50.0f, 50.0f, 1.0f, 1.0f, // p2 top right
			 -50.0f, 50.0f, 0.0f, 1.0f, // p3 top left
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray	va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		IndexBuffer ib(indices, 6);

		va.UnBind();
		vb.Unbind();
		ib.Unbind();

		float r = 0.0f;
		float increment = 0.01f;

		basicShader.Bind();
		Texture texture("res/textures/cat.png");
		texture.Bind();
		basicShader.SetUniform1i("u_Texture", 0);

		Renderer renderer;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		glm::mat4 proj = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f); // convert from window space to normal device coordinates (-1, 1)
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // move everything to the left simulating camera moving to the right

		float cameraH = 0.0f;
		float cameraV = 0.0f;

		glm::vec3 translationA(200, 200, 0);
		glm::vec3 translationB(400, 400, 0);

		scene::Scene* currentScene = nullptr;
		scene::SceneMenu* sceneMenu = new scene::SceneMenu(currentScene);
		currentScene = sceneMenu;

		sceneMenu->RegisterTest<scene::SceneClearColor>("Clear Color");

		//scene::SceneClearColor clearColor;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			//clearColor.OnUpdate(0.0f);
			//clearColor.OnRender();


			// ImGui setup frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentScene)
			{
				currentScene->OnUpdate(0.0f);
				currentScene->OnRender();
				ImGui::Begin("Scene");
				if (currentScene != sceneMenu && ImGui::Button("<-"))
				{
					delete currentScene;
					currentScene = sceneMenu;
				}
				currentScene->OnImGuiRender();
				ImGui::End();
			}
			//clearColor.OnImGuiRender();


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();

			//renderMode();

			//double fps = calculateFrameRate();

			/* Render here */
			//glClear(GL_COLOR_BUFFER_BIT);
			//basicShader.Bind();

			//{
			//	glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
			//	glm::mat4 mvp = proj * view * model; // these need to be multiplie in reverse order because opengl is in column major (right to left)
			//	basicShader.SetUniformMat4f("u_MVP", mvp);
			//	renderer.Draw(va, ib, basicShader);
			//}
			//
			//{
			//	glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
			//	glm::mat4 mvp = proj * view * model; // these need to be multiplie in reverse order because opengl is in column major (right to left)
			//	basicShader.SetUniformMat4f("u_MVP", mvp);
			//	renderer.Draw(va, ib, basicShader);
			//}

			//static float f = 0.0f;
			//static int counter = 0;


			//ImGui::Begin("My name  is window");
			//ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, (float)WIDTH);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, (float)WIDTH);            // Edit 1 float using a slider from 0.0f to 1.0f
			////ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			//ImGui::End();


			//ImGui::Render();
			//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			///* Swap front and back buffers */
			//glfwSwapBuffers(window);

			///* Poll for and process events */
			//glfwPollEvents();
		}

		glDeleteProgram(basicShader.shader);
		delete currentScene;

		if (currentScene != sceneMenu)
			delete sceneMenu;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

double calculateFrameRate() {
	double currentTime = glfwGetTime();
	frameCount++;
	// If a second has passed.
	if (currentTime - startTime >= 1.0)
	{
		// Display the frame count here any way you want.
		std::cout << "FPS: " + std::to_string(frameCount) << std::endl;

		frameCount = 0;
		startTime = currentTime;
		return (double)frameCount;
	}

	return (double)0;
}

