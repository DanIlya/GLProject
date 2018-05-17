// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in Drawer.cpp.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
//initial point
unsigned int i = 7;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}
unsigned int getcurrentnumber(){
	return i;
}



// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 3.14f;
// Initial Field of View
float initialFoV = 45.0f;

int speed = 185; // 3 units / second
int ispeed = 50;
float mouseSpeed = 0.004f;

//initial radius
float radius = 200.0f;

void computeMatricesFromInputs(std::vector<glm::vec3> vertices){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move closer to trajectory
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		radius -=  deltaTime * speed;
	}
	// Move further
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		radius += deltaTime * speed;
	}
	// Move forward along trajectory
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		i +=  ispeed; //int(deltaTime * speed);
		if (i > vertices.size()-1)
            i = vertices.size()-1;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        if (int(i - ispeed) < 7)
            i = 7;
        else
            i -=  ispeed; //int(deltaTime * speed);

	}

	float FoV = initialFoV;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 10000 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 10000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								vertices[i] - radius*direction,           // Camera is here
								vertices[i] , // and looks here : at the same current point of trajectory
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

