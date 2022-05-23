/*-------------------------------------------------------------*/
/* ------------- Proyecto Final Eagle Warrior -----------------*/
/*------------------------- 2022-2 ----------------------------*/
/*-------- Espino de Horta Joaquín Gustavo -315104271----------*/
/*-------- Reyes González Agustín Óscar  417095215 ------------*/
#include <Windows.h>
#include <mmsystem.h>
#include <glad/glad.h>
#include <glfw3.h>	
#include <stdlib.h>		
#include <glm/glm.hpp>	
#include <glm/gtc/matrix_transform.hpp>	
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#define SOLDIER_SIZE  1.45f
#define CAR_X0 -300.0f
#define CAR_Y0 7.0f
#define CAR_Z0 300.0f
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <vector>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 50.0f, 300.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f, lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//Keyframes (Manipulación y dibujo)
typedef struct _frame {
	float pos[4];
}FRAME;
typedef struct Rutina{
	float pos[4];   //Posicionamiento
	float inc[4];   //incrementos
	std::vector<FRAME> frm;   //cuadros	
	//FRAME frm[9];   //cuadros	
	int	velocity;
	int currentFrame;
	int currentStep;
	int	nFrames;

	bool loop;
	bool active;

	void resetPosition() {
		for (int i = 0; i < 4; i++)
				pos[i] = frm[0].pos[i];
		currentFrame = 0;
		currentStep = 0;
	}
	void interpolacion() {
		for (int i = 0; i < 4; i++) 
			inc[i] = (frm[currentFrame + 1].pos[i] - frm[currentFrame].pos[i]) / velocity;
	}
	bool animar() {
		if (active)
			if (currentStep >= velocity) {
				currentFrame++;
				if (currentFrame > nFrames - 2){
					currentFrame = 0;
					active = false;
				}else{
					currentStep = 0;
					interpolacion();
				}
				return true; //Envia un trigger al actualizar el frame
			}else {
				for (int i = 0; i < 4; i++)
					pos[i] += inc[i];
				currentStep++;
			}
		else if (loop) {
			resetPosition();
			interpolacion();
			active = true;
		}
		return false;
	}
}RUTINA;
//Animacion y desencadenadores
RUTINA carromato, patrulla, condrack, sol1, sol2, sol3, sol4;
int  trg[2];
bool drawCarromato = true;

void setKeyFrames(){
	carromato.active = true; carromato.loop = false; carromato.velocity = 300.0f; carromato.nFrames = 8; carromato.frm.resize(8);
	carromato.frm[0].pos[0] = -300.0f;  carromato.frm[0].pos[1] =  7.0f;  carromato.frm[0].pos[2] =  300.0f;  carromato.frm[0].pos[3] =  90.0f;
	carromato.frm[1].pos[0] =  285.0f;  carromato.frm[1].pos[1] =  7.0f;  carromato.frm[1].pos[2] =  300.0f;  carromato.frm[1].pos[3] =  90.0f;
	carromato.frm[2].pos[0] =  350.0f;  carromato.frm[2].pos[1] =  7.0f;  carromato.frm[2].pos[2] =  300.0f;  carromato.frm[2].pos[3] = 180.0f;
	carromato.frm[3].pos[0] =  350.0f;  carromato.frm[3].pos[1] =  7.0f;  carromato.frm[3].pos[2] = -285.0f;  carromato.frm[3].pos[3] = 180.0f;
	carromato.frm[4].pos[0] =  350.0f;  carromato.frm[4].pos[1] =  7.0f;  carromato.frm[4].pos[2] = -300.0f;  carromato.frm[4].pos[3] = 270.0f;
	carromato.frm[5].pos[0] = -285.0f;  carromato.frm[5].pos[1] =  7.0f;  carromato.frm[5].pos[2] = -300.0f;  carromato.frm[5].pos[3] = 270.0f;
	carromato.frm[6].pos[0] = -300.0f;  carromato.frm[6].pos[1] =  7.0f;  carromato.frm[6].pos[2] = -300.0f;  carromato.frm[6].pos[3] = 360.0f;
	carromato.frm[7].pos[0] = -300.0f;  carromato.frm[7].pos[1] =  7.0f;  carromato.frm[7].pos[2] =  400.0f;  carromato.frm[7].pos[3] = 360.0f;
	carromato.resetPosition(); carromato.interpolacion();

	patrulla.active = true; patrulla.loop = true; patrulla.velocity = 400.0f; patrulla.nFrames = 9; patrulla.frm.resize(9);
	patrulla.frm[0].pos[0] = -250.0f;  patrulla.frm[0].pos[1] = -5.0f;  patrulla.frm[0].pos[2] = -300.0f;  patrulla.frm[0].pos[3] = 90.0f;
	patrulla.frm[1].pos[0] =  280.0f;  patrulla.frm[1].pos[1] = -5.0f;  patrulla.frm[1].pos[2] = -300.0f;  patrulla.frm[1].pos[3] = 90.0f;
	patrulla.frm[2].pos[0] =  300.0f;  patrulla.frm[2].pos[1] = -5.0f;  patrulla.frm[2].pos[2] = -300.0f;  patrulla.frm[2].pos[3] =  0.0f;
	patrulla.frm[3].pos[0] =  300.0f;  patrulla.frm[3].pos[1] = -5.0f;  patrulla.frm[3].pos[2] =  280.0f;  patrulla.frm[3].pos[3] =  0.0f;
	patrulla.frm[4].pos[0] =  300.0f;  patrulla.frm[4].pos[1] = -5.0f;  patrulla.frm[4].pos[2] =  300.0f;  patrulla.frm[4].pos[3] = 180.0f;
	patrulla.frm[5].pos[0] =  300.0f;  patrulla.frm[5].pos[1] = -5.0f;  patrulla.frm[5].pos[2] = -280.0f;  patrulla.frm[5].pos[3] = 180.0f;
	patrulla.frm[6].pos[0] =  300.0f;  patrulla.frm[6].pos[1] = -5.0f;  patrulla.frm[6].pos[2] = -300.0f;  patrulla.frm[6].pos[3] = 270.0f;
	patrulla.frm[7].pos[0] = -250.0f;  patrulla.frm[7].pos[1] = -5.0f;  patrulla.frm[7].pos[2] = -300.0f;  patrulla.frm[7].pos[3] = 270.0f;
	patrulla.frm[8].pos[0] = -230.0f;  patrulla.frm[8].pos[1] = -5.0f;  patrulla.frm[8].pos[2] = -300.0f;  patrulla.frm[8].pos[3] = 450.0f;
	patrulla.resetPosition(); patrulla.interpolacion();

	condrack.active = false; condrack.loop = false; condrack.velocity = 400.0f; condrack.nFrames = 2; condrack.frm.resize(2);
	condrack.frm[0].pos[0] = 530.0f;  condrack.frm[0].pos[1] = 65.0f;  condrack.frm[0].pos[2] = 93.0f;  condrack.frm[0].pos[3] = 90.0f;
	condrack.frm[1].pos[0] = 530.0f;  condrack.frm[1].pos[1] = 65.0f;  condrack.frm[1].pos[2] = 93.0f;  condrack.frm[1].pos[3] = 90.0f;
	condrack.resetPosition(); condrack.interpolacion();
}
void triggers() {
	if (carromato.animar()) {
		switch (trg[0]) {
			case 6:
				drawCarromato = false;
				trg[0] = 0;
		}
		trg[0]++;
	}
	if (patrulla.animar()) {
		//switch (trg[1]) {
		//}
		//trg[1]++;
	}
	if (condrack.animar()) {
		//switch (trg[1]) {
		//}
		//trg[1]++;
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}
void playSoundTrack(int nOST) {
	switch(nOST){
		case 0:
			PlaySound(TEXT("resources/Audio/Soundtrack/Calm.wav"), NULL, SND_FILENAME);
			break;
		case 1:
			PlaySound(TEXT("resources/Audio/Soundtrack/Control.wav"), NULL, SND_FILENAME);
			break;
		case 2:
			PlaySound(TEXT("resources/Audio/Soundtrack/Anticipation.wav"), NULL, SND_FILENAME);
			break;
		case 3:
			PlaySound(TEXT("resources/Audio/Soundtrack/Action1.wav"), NULL, SND_FILENAME);
			break;
		case 4:
			PlaySound(TEXT("resources/Audio/Soundtrack/Action2.wav"), NULL, SND_FILENAME);
			break;
		case 5:
			PlaySound(TEXT("resources/Audio/Soundtrack/Action3.wav"), NULL, SND_FILENAME);
	}
}

int main() {
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC_Eagle_Warrior", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");

	//Modelos del proyecto final//
	Model pisoCatedral("resources/objects/Piso_Catedral/Piso_Catedral_1.obj");
	Model iglesia("resources/objects/Iglesia/Iglesia.obj");
	Model escalera("resources/objects/Escaleras/Escaleras.obj");
	Model puesto1("resources/objects/Puesto_1/Puesto_1.obj");
	Model estatua("resources/objects/Estatua/Estatua.obj");
	Model jardinera("resources/objects/Jardinera/Jardinera_1.obj");
	Model jardinera2("resources/objects/Jardinera_2/Jardinera_2.obj");
	Model casa1("resources/objects/Casa1/Casa1.obj");
	Model casa2("resources/objects/Casa2/Casa2.obj");
	Model casa5("resources/objects/Casa5/Casa5.obj");
	Model casa6("resources/objects/Casa6/Casa6.obj");
	Model base6("resources/objects/Base6/Base6.obj");
	Model casa4("resources/objects/Casa_Tipo4/Casa_Tipo4.obj");
	Model arbol2("resources/objects/Arbol2/Arbol2.obj");
	Model muro("resources/objects/Muro/Muro.obj");
	Model casa8("resources/objects/Casa8/casa8.obj");
	Model lampara("resources/objects/Lampara/Lampara.obj");
	Model pozo("resources/objects/pozo/pozo.obj");
	Model fuente2("resources/objects/fuente2/fuente2.obj");
	Model cementerio("resources/objects/cementerio/cementerio.obj");

	Model Carretilla("resources/objects/Carretilla/Carretilla.obj");
	Model Rueda("resources/objects/Carretilla/Ruedas.obj");

	ModelAnim Condrack("resources/objects/Condrack/Vigilar_Techo.dae");
	Condrack.initShaders(animShader.ID);
	ModelAnim Caballo("resources/objects/Carretilla/Caballo.dae");
	Caballo.initShaders(animShader.ID);
	ModelAnim Soldado("resources/objects/Soldado/Marchar.dae");
	Soldado.initShaders(animShader.ID);
	/*ModelAnim Dragon_Medio("resources/objects/MidDragon/Raptar.dae");
	Soldado.initShaders(animShader.ID);
	ModelAnim Dragon_Pesado("resources/objects/HeavyDragon/Volar.dae");
	Soldado.initShaders(animShader.ID);*/
	std::thread soundtrack(&playSoundTrack,0);

	//Reproduccion del Soundtrack
	soundtrack.detach();
	setKeyFrames();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)){
		skyboxShader.setInt("skybox", 0);
		lastFrame = SDL_GetTicks();

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		staticShader.use();
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.9f, 0.8f, 0.6f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

		triggers();
		// -------------------------------------------------------------------------------------------------------------------------
		// Personajes Con Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(condrack.pos[0], condrack.pos[1], condrack.pos[2]));
		model = glm::scale(model, glm::vec3(-2.3f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(condrack.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Condrack.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3( patrulla.pos[0], patrulla.pos[1], patrulla.pos[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrulla.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrulla.pos[0] + 13.0f, patrulla.pos[1], patrulla.pos[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrulla.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrulla.pos[0] - 13.0f, patrulla.pos[1], patrulla.pos[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrulla.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrulla.pos[0], patrulla.pos[1], patrulla.pos[2] - 15.0f));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrulla.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrulla.pos[0], patrulla.pos[1], patrulla.pos[2] + 15.0f));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrulla.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);
		if (drawCarromato) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(carromato.pos[0], carromato.pos[1] - 7.0f, carromato.pos[2]));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(carromato.pos[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(2.0f));
			animShader.setMat4("model", model);
			Caballo.Draw(animShader);
		}
		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		// -------------------------------------------------------------------------------------------------------------------------
		// Carreta
		// -------------------------------------------------------------------------------------------------------------------------
		if (drawCarromato) {
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0, 6.5f, -3.0f));
			staticShader.setMat4("model", model);
			Carretilla.Draw(staticShader);
			model = glm::translate(model, glm::vec3(6.25f, -2.9f, -11.0f));
			staticShader.setMat4("model", model);
			Rueda.Draw(staticShader);
			model = glm::translate(model, glm::vec3(-12.5f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(-1.0f));
			staticShader.setMat4("model", model);
			Rueda.Draw(staticShader);
		}
		// -------------------------------------------------------------------------------------------------------------------------
		// Carreta Fin
		// -------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -5.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.23f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Jardines
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(5.0f));

		model = glm::translate(model, glm::vec3(100.0f, -2.0f, -90.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);


		model = glm::translate(model, glm::vec3(7.0f, 0.0f, 5.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, -30.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(8.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -6.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(6.0f, 0.0f, -6.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -6.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(20.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 6.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-220.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(1.0f));

		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		arbol2.Draw(staticShader);
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Fin del Jardin 
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		pisoCatedral.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 250.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		escalera.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -3.0f, -55.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		staticShader.setMat4("model", model);
		iglesia.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(160.0f, -5.0f, 40.0f));
		model = glm::rotate(model, glm::radians(95.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, -5.0f, 80.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, -5.0f, 120.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -5.0f, 50.0f));
		model = glm::rotate(model, glm::radians(200.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -5.0f, 95.0f));
		model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -5.0f, 140.0f));
		model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		staticShader.setMat4("model", model);
		puesto1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, -5.0f, -98.0f));
		model = glm::rotate(model, glm::radians(92.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, 0.0f, 100.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f));
		staticShader.setMat4("model", model);
		casa2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 0.0f, -100.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f));
		staticShader.setMat4("model", model);
		casa2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, -5.0f, 280.0f));
		model = glm::rotate(model, glm::radians(272.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-520.0f, -4.5f, -275.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-520.0f, -4.5f, 250.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(520.0f, -4.5f, -245.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(520.0f, -4.5f, 105.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, -4.5f, -520.0f));		//
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(195.0f, -4.5f, -520.0f));		//
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-190.0f, 0.0f, -520.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f));
		staticShader.setMat4("model", model);
		casa2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, -4.5f, -520.0f));		//
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		casa6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 75.0f, 200.0f));		//
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.5f));
		staticShader.setMat4("model", model);
		casa4.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 75.0f, 200.0f));		//
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.5f));
		staticShader.setMat4("model", model);
		casa4.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(470.0f, 25.0f, 470.0f));		//
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(35.0f));
		staticShader.setMat4("model", model);
		casa8.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-485.0f, -4.5f, -260.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-485.0f, -4.5f, -85.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-485.0f, -4.5f, 90.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-485.0f, -4.5f, 265.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(485.0f, -4.5f, -260.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(485.0f, -4.5f, -85.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(485.0f, -4.5f, 90.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(485.0f, -4.5f, 265.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.5f, -490.0f));			//
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, -4.5f, -490.0f));			//
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -4.5f, -490.0f));			//
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		staticShader.setMat4("model", model);
		base6.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, 18.0f, -170.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		estatua.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-170.0f, 18.0f, -170.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		estatua.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-170.0f, -2.5f, -170.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, -2.5f, -170.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -2.5f, 175.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -2.5f, 70.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, -2.5f, 70.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, -2.5f, 180.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		jardinera2.Draw(staticShader);


		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, 220.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, 139.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, 57.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, -24.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		//muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, -134.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(253.0f, -8.0f, -215.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(213.0f, -8.0f, -259.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(132.0f, -8.0f, -259.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-136.0f, -8.0f, -259.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-217.0f, -8.0f, -259.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-90.0f, 0.0f, 100.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, 220.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, 139.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, 57.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, -24.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		//muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, -134.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-259.0f, -8.0f, -215.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(400.0f, 4.0f, 177.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(400.0f, 4.0f, 2.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(400.0f, 4.0f, -173.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-400.0f, 4.0f, 177.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-400.0f, 4.0f, 2.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-400.0f, 4.0f, -173.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-90.0f, 4.0f, -405.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(90.0f, 4.0f, -405.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		lampara.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(430.0f, -5.0f, 145.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		pozo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(430.0f, -5.0f, -30.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		pozo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-430.0f, -5.0f, 230.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		pozo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-430.0f, -5.0f, -135.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		pozo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(50.0f, -5.0f, -450.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		pozo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 135.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		fuente2.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -135.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		cementerio.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(75.0f, -8.0f, -119.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(75.0f, -8.0f, -160.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(75.0f, -8.0f, -119.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-75.0f, -8.0f, -160.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-75.0f, -8.0f, -119.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		deltaTime = SDL_GetTicks() - lastFrame;
		if (deltaTime < LOOP_TIME)
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}