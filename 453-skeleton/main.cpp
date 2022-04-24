#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <stdlib.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// An example struct for Game Objects.
// You are encouraged to customize this as you see fit.
struct GameObject {
	// Struct's constructor deals with the texture.
	// Also sets default position, theta, scale, and transformationMatrix
	GameObject(std::string texturePath, GLenum textureInterpolation) :
		texture(texturePath, textureInterpolation),
		position(0.0f, 0.0f, 0.0f),
		theta(0),
		scale(1, 1),
		transformationMatrix(1.0f) // This constructor sets it as the identity matrix
	{}

	CPU_Geometry cgeom;
	GPU_Geometry ggeom;
	Texture texture;

	glm::vec3 position;
	float theta; // Object's rotation
	// Alternatively, you could represent rotation via a normalized heading vec:
	// glm::vec3 heading;
	glm::vec2 scale; // Or, alternatively, a glm::vec2 scale;
	glm::mat4 transformationMatrix;
};


class MyCallbacks : public CallbackInterface {

public:
	MyCallbacks(ShaderProgram& shader, int screen_width, int screen_height) : shader(shader),
	screen_dimensions(screen_width, screen_height)
	{}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			shader.recompile();
		}
		if (key == GLFW_KEY_UP) {
			increment = increment + 1.f/240.f;
			up_down_pressed = true;
		}
		if (key == GLFW_KEY_W) {
			increment = increment + 1.f / 240.f;
			up_down_pressed = true;
		}
		if (key == GLFW_KEY_DOWN) {
			increment = increment - 1.f/240.f;
			up_down_pressed = true;
		}
		if (key == GLFW_KEY_S) {
			increment = increment - 1.f / 240.f;
			up_down_pressed = true;
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			reset = true;
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			mouse_coordinates_pressed = mouse_coordinates_normalized;
			mouse_pressed = true;
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) {
		mouse_coordinates_normalized = normalizeCoordinates(glm::vec2(xpos, ypos));
		//std::cout << normalized_coordinates.x << " " << normalized_coordinates.y << std::endl;
	}


	bool getPressed() {
		bool result = mouse_pressed;
		mouse_pressed = false;
		return result;
	}

	bool UpDownPressed() {
		bool result = up_down_pressed;
		up_down_pressed = false;
		return result;
	}

	bool getReset() {
		bool result = reset;
		reset = false;
		return result;
	}

	void handleReset() {
		reset = false;
	}

	glm::vec2 getPressedCoords() {
		return mouse_coordinates_pressed;
	}

	float getIncrement() {
		return increment;
	}

	void setIncrement(float incr) {
		increment = incr;
	}

	glm::vec2 normalizeCoordinates(glm::vec2 mouse_coordinates) {
		glm::vec2 normalized_coordinates = mouse_coordinates / (screen_dimensions - 1.f);
		normalized_coordinates = normalized_coordinates * 2.f;
		normalized_coordinates = normalized_coordinates - 1.f;
		normalized_coordinates.y = -normalized_coordinates.y;
		return normalized_coordinates;
	}

private:
	float increment = 0.f;
	glm::vec2 mouse_coordinates_normalized;
	glm::vec2 mouse_coordinates_pressed;
	bool mouse_pressed = false;
	bool up_down_pressed = false;
	bool reset = false;
	glm::vec2 screen_dimensions;
	ShaderProgram& shader;
};

CPU_Geometry createGeometry() {

	CPU_Geometry retGeom;

	retGeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(-1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
	retGeom.verts.push_back(glm::vec3(1.f, 1.f, 0.f));

	// texture coordinates
	retGeom.texCoords.push_back(glm::vec2(0.f, 1.f));
	retGeom.texCoords.push_back(glm::vec2(0.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(0.f, 1.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 0.f));
	retGeom.texCoords.push_back(glm::vec2(1.f, 1.f));
	return retGeom;
}

float normVector(glm::vec3 vec)
{
	float result = sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	return result;
}


//Resource used - https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection

bool checkCollision(GameObject& ship, GameObject& diamond)
{
	bool collisionX = ship.position.x + ship.scale.x >= diamond.position.x &&
		diamond.position.x + diamond.scale.x >= ship.position.x;
	bool collisionY = ship.position.y + ship.scale.y >= diamond.position.y &&
		diamond.position.y + diamond.scale.y >= ship.position.y;
	return collisionX && collisionY;
}




int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	int screen_width = 800;
	int screen_height = 800;
	int score = 0;
	Window window(screen_width, screen_height, "CPSC 453"); // can set callbacks at construction if desired


	GLDebug::enable();

	// SHADERS
	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	// CALLBACKS
	auto callbacks = std::make_shared<MyCallbacks>(shader, screen_width, screen_height);
	window.setCallbacks(callbacks); // can also update callbacks to new ones

	// GL_NEAREST looks a bit better for low-res pixel art than GL_LINEAR.
	// But for most other cases, you'd want GL_LINEAR interpolation.
	GameObject ship("textures/ship.png", GL_NEAREST);

	ship.cgeom = createGeometry();
	ship.ggeom.setVerts(ship.cgeom.verts);
	ship.ggeom.setTexCoords(ship.cgeom.texCoords);

	ship.scale = glm::vec2(0.09f, 0.06f);


	GameObject diamond1("textures/diamond.png", GL_NEAREST);

	diamond1.cgeom = createGeometry();
	diamond1.ggeom.setVerts(diamond1.cgeom.verts);
	diamond1.ggeom.setTexCoords(diamond1.cgeom.texCoords);


	diamond1.scale = glm::vec2(0.07f, 0.07f);


	GameObject diamond2("textures/diamond.png", GL_NEAREST);

	diamond2.cgeom = createGeometry();
	diamond2.ggeom.setVerts(diamond2.cgeom.verts);
	diamond2.ggeom.setTexCoords(diamond2.cgeom.texCoords);

	diamond2.scale = glm::vec2(0.07f, 0.07f);


	GameObject diamond3("textures/diamond.png", GL_NEAREST);

	diamond3.cgeom = createGeometry();
	diamond3.ggeom.setVerts(diamond3.cgeom.verts);
	diamond3.ggeom.setTexCoords(diamond3.cgeom.texCoords);

	diamond3.scale = glm::vec2(0.07f, 0.07f);

	GameObject diamond4("textures/diamond.png", GL_NEAREST);

	diamond4.cgeom = createGeometry();
	diamond4.ggeom.setVerts(diamond4.cgeom.verts);
	diamond4.ggeom.setTexCoords(diamond4.cgeom.texCoords);

	diamond4.scale = glm::vec2(0.07f, 0.07f);

	float target_rotation = 0.f;
	float current_rotation = 0.f;
	float shipIncrement = 0.f;

	// RENDER LOOP
	while (!window.shouldClose()) {

		glfwPollEvents();

		shader.use();

		if (callbacks->getPressed())
		{
			glm::vec2 mousePos = callbacks->getPressedCoords();
			glm::vec3 starting_vector = glm::vec3(0.f, 1.f, 0.f);
			glm::vec3 ending_vector = glm::vec3(mousePos.x, mousePos.y, 0.f);
			float dotProduct = glm::dot(starting_vector, ending_vector);
			float normalized_starting_vector = normVector(starting_vector);
			float normalized_ending_vector = normVector(ending_vector);
			glm::vec3 direction = cross(starting_vector, ending_vector);
			target_rotation = - direction.z * (acos(dotProduct/(normalized_starting_vector * normalized_ending_vector))) / abs(direction.z);
		}

		if (callbacks->UpDownPressed())
		{
			shipIncrement = callbacks->getIncrement();
		}

		glm::mat4 initial_ship = glm::mat4{
		ship.scale.x, 0.f, 0.f, 0.f,
		0.f, ship.scale.y, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};

		glm::mat4 ship_rotation = glm::mat4{
		cos((current_rotation)), sin((current_rotation)), 0.f, 0.f,
		-sin((current_rotation)), cos((current_rotation)), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};

		glm::mat4 ship_translation;
		if (current_rotation >= 0) {
			ship_translation = glm::mat4{
			1.0f, 0.f, 0.f, shipIncrement * sin(current_rotation),
			0.f, 1.0f, 0.f, shipIncrement * cos(current_rotation),
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
			};
		}
		else
		{
			ship_translation = glm::mat4{
			1.0f, 0.f, 0.f, shipIncrement * sin(current_rotation),
			0.f, 1.0f, 0.f, shipIncrement * cos(-current_rotation),
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
			};
		}

		if (callbacks->getReset())
		{
			ship.scale = glm::vec2(0.09f, 0.06f);
			current_rotation = 0.f;
			target_rotation = 0.f;
			callbacks->setIncrement(0.f);
			shipIncrement = 0.f;
			score = 0;
			diamond1.scale = glm::vec2(0.07f, 0.07f);
			diamond2.scale = glm::vec2(0.07f, 0.07f);
			diamond3.scale = glm::vec2(0.07f, 0.07f);
			diamond4.scale = glm::vec2(0.07f, 0.07f);
		}

		ship.ggeom.bind();
		
		ship.transformationMatrix = initial_ship * ship_rotation * ship_translation;
		ship.position = glm::vec4(0.f, 0.f, 0.f, 1.f) * ship.transformationMatrix;
		
		unsigned myLoc = glGetUniformLocation(shader.getProgram(), "transformationMatrix");

		glUniformMatrix4fv(myLoc, 1, true, glm::value_ptr(ship.transformationMatrix));

		if (current_rotation < target_rotation)
		{
			current_rotation += 1.f/480.f;
		}
		else if (current_rotation > target_rotation)
		{
			current_rotation -= 1.f / 480.f;
		}



		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ship.texture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		ship.texture.unbind();


		bool collision = checkCollision(ship, diamond1);
		if (collision)
		{
			if (score == 0 && diamond1.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.1f;
				score += 1;
			}
			else if (score == 1 && diamond1.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.15f;
				score += 1;
			}
			else if (score == 2 && diamond1.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.2f;
				score += 1;
			}
			else if (score == 3 && diamond1.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.25f;
				score += 1;
			}
			else if (score == 4 && diamond1.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.3f;
			}
			diamond1.scale = glm::vec2(0.f, 0.f);
			
		}

		diamond1.transformationMatrix = glm::mat4{
		diamond1.scale.x, 0.f, 0.f, 0.47f,
		0.f, diamond1.scale.y, 0.f, 0.53f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};

		diamond1.position = glm::vec3(0.47f, 0.53f, 0.f);
		diamond1.ggeom.bind();
		diamond1.texture.bind();

		glUniformMatrix4fv(myLoc, 1, true, glm::value_ptr(diamond1.transformationMatrix));
		
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		diamond1.texture.unbind();

		bool collision1 = checkCollision(ship, diamond2);

		if (collision1 == true)
		{
			if (score == 0 && diamond2.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.1f;
				score += 1;
			}
			else if (score == 1 && diamond2.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.15f;
				score += 1;
			}
			else if (score == 2 && diamond2.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.2f;
				score += 1;
			}
			else if (score == 3 && diamond2.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.25f;
				score += 1;
			}
			else if (score == 4 && diamond2.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.3f;
			}
			diamond2.scale = glm::vec2(0.f, 0.f);
		}

		diamond2.transformationMatrix = glm::mat4{
		diamond2.scale.x, 0.f, 0.f, -0.5f,
		0.f, diamond2.scale.y, 0.f, 0.52f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};
		diamond2.position = glm::vec3(-0.5f, 0.52f, 0.f);
		diamond2.ggeom.bind();

		glUniformMatrix4fv(myLoc, 1, true, glm::value_ptr(diamond2.transformationMatrix));

		diamond2.texture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		diamond2.texture.unbind();

		bool collision2 = checkCollision(ship, diamond3);

		if (collision2)
		{
			if (score == 0 && diamond3.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.1f;
				score += 1;
			}
			else if (score == 1 && diamond3.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.15f;
				score += 1;
			}
			else if (score == 2 && diamond3.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.2f;
				score += 1;
			}
			else if (score == 3 && diamond3.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.25f;
				score += 1;
			}
			else if (score == 4 && diamond3.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.3f;
			}
			diamond3.scale = glm::vec2(0.f, 0.f);
		}

		diamond3.transformationMatrix = glm::mat4{
		diamond3.scale.x, 0.f, 0.f, -0.5f,
		0.f, diamond3.scale.y, 0.f, -0.25f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};
		diamond3.position = glm::vec3(-0.5f, -0.25f, 0.f);
		diamond3.ggeom.bind();

		glUniformMatrix4fv(myLoc, 1, true, glm::value_ptr(diamond3.transformationMatrix));

		diamond3.texture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		diamond3.texture.unbind();

		bool collision3 = checkCollision(ship, diamond4);

		if (collision3)
		{
			if (score == 0 && diamond4.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.1f;
				score += 1;
			}
			else if (score == 1 && diamond4.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.15f;
				score += 1;
			}
			else if (score == 2 && diamond4.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.2f;
				score += 1;
			}
			else if (score == 3 && diamond4.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.25f;
				score += 1;
			}
			else if (score == 4 && diamond4.scale != glm::vec2(0.f, 0.f)) {
				ship.scale = ship.scale * 1.3f;
			}
			diamond4.scale = glm::vec2(0.f, 0.f);
		}

		diamond4.transformationMatrix = glm::mat4{
		diamond4.scale.x, 0.f, 0.f, -0.25f,
		0.f, diamond4.scale.y, 0.f, -0.5f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
		};
		diamond4.position = glm::vec3(-0.25f, -0.5f, 0.f);
		diamond4.ggeom.bind();

		glUniformMatrix4fv(myLoc, 1, true, glm::value_ptr(diamond4.transformationMatrix));

		diamond4.texture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		diamond4.texture.unbind();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

		// Starting the new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Putting the text-containing window in the top-left of the screen.
		ImGui::SetNextWindowPos(ImVec2(5, 5));

		// Setting flags
		ImGuiWindowFlags textWindowFlags =
			ImGuiWindowFlags_NoMove |				// text "window" should not move
			ImGuiWindowFlags_NoResize |				// should not resize
			ImGuiWindowFlags_NoCollapse |			// should not collapse
			ImGuiWindowFlags_NoSavedSettings |		// don't want saved settings mucking things up
			ImGuiWindowFlags_AlwaysAutoResize |		// window should auto-resize to fit the text
			ImGuiWindowFlags_NoBackground |			// window should be transparent; only the text should be visible
			ImGuiWindowFlags_NoDecoration |			// no decoration; only the text should be visible
			ImGuiWindowFlags_NoTitleBar;			// no title; only the text should be visible

		// Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
		ImGui::Begin("scoreText", (bool*)0, textWindowFlags);

		// Scale up text a little, and set its value
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("Score: %d", score); // Second parameter gets passed into "%d"

		if (score >= 4)
		{
			ImGui::Text("Congratulations! You have won the game.");
		}
		// End the window.
		ImGui::End();

		ImGui::Render();	// Render the ImGui window
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

		window.swapBuffers();
	}
	// ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
