#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Function used to handle errors
void error_callback(int error, const char* description, char data = NULL) {
	if (data != NULL) {
		std::cerr << "Error: " << description << " " << data << std::endl;
		return;
	}
	else {
		std::cerr << "Error: " << description << std::endl;
	}
}

int main () {
	// Initialize GLFW
	glfwInit();

	// Set OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Set OpenGL profile to core
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// For compatibility with macOS
# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
# endif

	//Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);

	// Checking if window was created
	if (window == NULL) {
		error_callback(-1, "Failed to create window");
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		error_callback(-1, "Failed to initialize GLAD");
		return -1;
	}

	// Set the viewport
	glViewport(0, 0, 800, 600);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		// Render here
		glClear(GL_COLOR_BUFFER_BIT);
		// Swap front and back buffers
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();

		// Set the background color
		glClearColor(0.5f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}


	return 0;
}