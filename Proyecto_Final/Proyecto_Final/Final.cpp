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

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

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
float	carreta[4], patrol1[4], patrol2[4];
float	incCarreta[4], incPatrol1[4], incPatrol2[4];

#define MAX_FRAMES 25
int i_max_steps = 120;
int i_curr_steps = 0;
typedef struct _frame {
	float Comp[4];
}FRAME;

FRAME CAR_KeyFrame[MAX_FRAMES], PRT1_KeyFrame[MAX_FRAMES], PRT2_KeyFrame[MAX_FRAMES];
const int FrameIndex = MAX_FRAMES;
bool play = false;
int playIndex = 0;

void resetElements() {
	for (int i = 0; i < 4; i++) {
		carreta[i] = CAR_KeyFrame[0].Comp[i];
		patrol1[i] = PRT1_KeyFrame[0].Comp[i];
		patrol2[i] = PRT2_KeyFrame[0].Comp[i];
	}
}

void interpolation() {
	for (int i = 0; i < 4; i++) {
		incCarreta[i] = (CAR_KeyFrame[playIndex + 1].Comp[i] - CAR_KeyFrame[playIndex].Comp[i]) / i_max_steps;
		incPatrol1[i] = (PRT1_KeyFrame[playIndex + 1].Comp[i] - PRT1_KeyFrame[playIndex].Comp[i]) / i_max_steps;
		incPatrol2[i] = (PRT2_KeyFrame[playIndex + 1].Comp[i] - PRT2_KeyFrame[playIndex].Comp[i]) / i_max_steps;
	}
}

void animate() {
	if (play)
		if (i_curr_steps >= i_max_steps){
			playIndex++;
			if (playIndex > FrameIndex - 2){
				playIndex = 0;
				play = false;
			}else{
				i_curr_steps = 0;
				interpolation();
			}
		}else{
			for (int i = 0; i < 4; i++) {
				carreta[i] += incCarreta[i];
				patrol1[i] += incPatrol1[i];
				patrol2[i] += incPatrol2[i];
			}
			i_curr_steps++;
		}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void playSoundTrack() {
	///////////////////////////////////////
	///Sonido; Requiere de hilos
	///////////////////////////////////////
	PlaySound(TEXT("resources/Audio/Soundtrack/Calm.wav"), NULL, SND_FILENAME);
	PlaySound(TEXT("resources/Audio/Soundtrack/Control.wav"), NULL, SND_FILENAME);
	PlaySound(TEXT("resources/Audio/Soundtrack/Anticipation.wav"), NULL, SND_FILENAME);
	PlaySound(TEXT("resources/Audio/Soundtrack/Action1.wav"), NULL, SND_FILENAME);
	PlaySound(TEXT("resources/Audio/Soundtrack/Action2.wav"), NULL, SND_FILENAME);
	PlaySound(TEXT("resources/Audio/Soundtrack/Action3.wav"), NULL, SND_FILENAME);
}

void setKeyFrames(){
////Grupo_KeyFrame[nFrame].Comp[0] = X  Grupo_KeyFrame[nFrame].Comp[1] = Y Grupo_KeyFrame[nFrame].Comp[2] = Z   Grupo_KeyFrame[nFrame].Comp[3] = rotZ
	CAR_KeyFrame[0].Comp[0] = -300.0f;  CAR_KeyFrame[0].Comp[1] =  7.0f;  CAR_KeyFrame[0].Comp[2] =  300.0f;  CAR_KeyFrame[0].Comp[3] = 90.0f;
	CAR_KeyFrame[1].Comp[0] = -100.0f;  CAR_KeyFrame[1].Comp[1] =  7.0f;  CAR_KeyFrame[1].Comp[2] =  300.0f;  CAR_KeyFrame[1].Comp[3] = 90.0f;
	CAR_KeyFrame[2].Comp[0] =    0.0f;  CAR_KeyFrame[2].Comp[1] =  7.0f;  CAR_KeyFrame[2].Comp[2] =  300.0f;  CAR_KeyFrame[2].Comp[3] = 90.0f;
	CAR_KeyFrame[3].Comp[0] =  100.0f;  CAR_KeyFrame[3].Comp[1] =  7.0f;  CAR_KeyFrame[3].Comp[2] =  300.0f;  CAR_KeyFrame[3].Comp[3] = 90.0f;
	CAR_KeyFrame[4].Comp[0] =  300.0f;  CAR_KeyFrame[4].Comp[1] =  7.0f;  CAR_KeyFrame[4].Comp[2] =  300.0f;  CAR_KeyFrame[4].Comp[3] = 90.0f;
	CAR_KeyFrame[5].Comp[0] =  350.0f;  CAR_KeyFrame[5].Comp[1] =  7.0f;  CAR_KeyFrame[5].Comp[2] =  300.0f;  CAR_KeyFrame[5].Comp[3] =  180.0f;
	CAR_KeyFrame[6].Comp[0] =  350.0f;  CAR_KeyFrame[6].Comp[1] =   7.0f;  CAR_KeyFrame[6].Comp[2] = 300.0f;   CAR_KeyFrame[6].Comp[3] =  180.0f;
	CAR_KeyFrame[7].Comp[0] =  350.0f;  CAR_KeyFrame[7].Comp[1] =  7.0f;  CAR_KeyFrame[7].Comp[2] = 100.0f;   CAR_KeyFrame[7].Comp[3] = 180.0f;
	CAR_KeyFrame[8].Comp[0] =  350.0f;  CAR_KeyFrame[8].Comp[1] =   7.0f;  CAR_KeyFrame[8].Comp[2] =  0.0f;   CAR_KeyFrame[8].Comp[3] = 180.0f;
	CAR_KeyFrame[9].Comp[0] =  350.0f;  CAR_KeyFrame[9].Comp[1] =   7.0f;  CAR_KeyFrame[9].Comp[2] = -100.0f;   CAR_KeyFrame[9].Comp[3] = 180.0f;
	CAR_KeyFrame[10].Comp[0] = 350.0f;  CAR_KeyFrame[10].Comp[1] =  7.0f;  CAR_KeyFrame[10].Comp[2] =-300.0f;  CAR_KeyFrame[10].Comp[3] = 180.0f;
	CAR_KeyFrame[11].Comp[0] = 350.0f;  CAR_KeyFrame[11].Comp[1] =  7.0f;  CAR_KeyFrame[11].Comp[2] =-300.0f;  CAR_KeyFrame[11].Comp[3] = 270.0f;
	CAR_KeyFrame[12].Comp[0] = 300.0f;  CAR_KeyFrame[12].Comp[1] =  7.0f;  CAR_KeyFrame[12].Comp[2] = -300.0f;  CAR_KeyFrame[12].Comp[3] = 270.0f;
	CAR_KeyFrame[13].Comp[0] = 100.0f;  CAR_KeyFrame[13].Comp[1] = 7.0f;  CAR_KeyFrame[13].Comp[2] = -300.0f;  CAR_KeyFrame[13].Comp[3] = 270.0f;
	CAR_KeyFrame[14].Comp[0] =   0.0f;  CAR_KeyFrame[14].Comp[1] = 7.0f;  CAR_KeyFrame[14].Comp[2] = -300.0f;  CAR_KeyFrame[14].Comp[3] = 270.0f;
	CAR_KeyFrame[15].Comp[0] =-100.0f;  CAR_KeyFrame[15].Comp[1] = 7.0f;  CAR_KeyFrame[15].Comp[2] = -300.0f;  CAR_KeyFrame[15].Comp[3] = 270.0f;
	CAR_KeyFrame[16].Comp[0] =-300.0f;  CAR_KeyFrame[16].Comp[1] =  7.0f;  CAR_KeyFrame[16].Comp[2] = -300.0f;  CAR_KeyFrame[16].Comp[3] = 270.0f;
	CAR_KeyFrame[17].Comp[0] =-300.0f;  CAR_KeyFrame[17].Comp[1] =  7.0f;  CAR_KeyFrame[17].Comp[2] = -300.0f;  CAR_KeyFrame[17].Comp[3] = 360.0f;
	CAR_KeyFrame[18].Comp[0] = -300.0f;  CAR_KeyFrame[18].Comp[1] =  7.0f;  CAR_KeyFrame[18].Comp[2] = -300.0f;  CAR_KeyFrame[18].Comp[3] =360.0f;
	CAR_KeyFrame[19].Comp[0] = -300.0f;  CAR_KeyFrame[19].Comp[1] = 7.0f;  CAR_KeyFrame[19].Comp[2] = -100.0f;  CAR_KeyFrame[19].Comp[3] = 360.0f;
	CAR_KeyFrame[20].Comp[0] = -300.0f;  CAR_KeyFrame[20].Comp[1] = 7.0f;  CAR_KeyFrame[20].Comp[2] =  -50.0f;  CAR_KeyFrame[20].Comp[3] = 360.0f;
	CAR_KeyFrame[21].Comp[0] = -300.0f;  CAR_KeyFrame[21].Comp[1] = 7.0f;  CAR_KeyFrame[21].Comp[2] =   50.0f;  CAR_KeyFrame[21].Comp[3] = 360.0f;
	CAR_KeyFrame[22].Comp[0] = -300.0f;  CAR_KeyFrame[22].Comp[1] = 7.0f;  CAR_KeyFrame[22].Comp[2] =  100.0f;  CAR_KeyFrame[22].Comp[3] = 360.0f;
	CAR_KeyFrame[23].Comp[0] = -300.0f;  CAR_KeyFrame[23].Comp[1] =  7.0f;  CAR_KeyFrame[23].Comp[2] =  300.0f;  CAR_KeyFrame[23].Comp[3] = 360.0f;
	CAR_KeyFrame[24].Comp[0] = -300.0f;  CAR_KeyFrame[24].Comp[1] =  7.0f;  CAR_KeyFrame[24].Comp[2] =  300.0f;  CAR_KeyFrame[24].Comp[3] = 450.0f;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PRT1_KeyFrame[0].Comp[0] = -250.0f;  PRT1_KeyFrame[0].Comp[1] = -5.0f;  PRT1_KeyFrame[0].Comp[2] = -300.0f;  PRT1_KeyFrame[0].Comp[3] = 90.0f;
	PRT1_KeyFrame[1].Comp[0] = -150.0f;  PRT1_KeyFrame[1].Comp[1] = -5.0f;  PRT1_KeyFrame[1].Comp[2] = -300.0f;  PRT1_KeyFrame[1].Comp[3] = 90.0f;
	PRT1_KeyFrame[2].Comp[0] =    0.0f;  PRT1_KeyFrame[2].Comp[1] = -5.0f;  PRT1_KeyFrame[2].Comp[2] = -300.0f;  PRT1_KeyFrame[2].Comp[3] = 90.0f;
	PRT1_KeyFrame[3].Comp[0] =  150.0f;  PRT1_KeyFrame[3].Comp[1] = -5.0f;  PRT1_KeyFrame[3].Comp[2] = -300.0f;  PRT1_KeyFrame[3].Comp[3] = 90.0f;
	PRT1_KeyFrame[4].Comp[0] =  300.0f;  PRT1_KeyFrame[4].Comp[1] = -5.0f;  PRT1_KeyFrame[4].Comp[2] = -300.0f;  PRT1_KeyFrame[4].Comp[3] = 90.0f;
	PRT1_KeyFrame[5].Comp[0] =  300.0f;  PRT1_KeyFrame[5].Comp[1] = -5.0f;  PRT1_KeyFrame[5].Comp[2] = -300.0f;  PRT1_KeyFrame[5].Comp[3] =  0.0f;
	PRT1_KeyFrame[6].Comp[0] =  300.0f;  PRT1_KeyFrame[6].Comp[1] = -5.0f;  PRT1_KeyFrame[6].Comp[2] = -200.0f;  PRT1_KeyFrame[6].Comp[3] =  0.0f;
	PRT1_KeyFrame[7].Comp[0] =  300.0f;  PRT1_KeyFrame[7].Comp[1] = -5.0f;  PRT1_KeyFrame[7].Comp[2] = -100.0f;  PRT1_KeyFrame[7].Comp[3] =  0.0f;
	PRT1_KeyFrame[8].Comp[0] =  300.0f;  PRT1_KeyFrame[8].Comp[1] = -5.0f;  PRT1_KeyFrame[8].Comp[2] =    0.0f;  PRT1_KeyFrame[8].Comp[3] =  0.0f;
	PRT1_KeyFrame[9].Comp[0] =  300.0f;  PRT1_KeyFrame[9].Comp[1] = -5.0f;  PRT1_KeyFrame[9].Comp[2] =   100.0f;  PRT1_KeyFrame[9].Comp[3] = 0.0f;
	PRT1_KeyFrame[10].Comp[0] = 300.0f;  PRT1_KeyFrame[10].Comp[1] = -5.0f; PRT1_KeyFrame[10].Comp[2] =  200.0f; PRT1_KeyFrame[10].Comp[3] = 0.0f;
	PRT1_KeyFrame[11].Comp[0] = 300.0f;  PRT1_KeyFrame[11].Comp[1] = -5.0f; PRT1_KeyFrame[11].Comp[2] =  300.0f; PRT1_KeyFrame[11].Comp[3] = 0.0f;
	PRT1_KeyFrame[12].Comp[0] = 300.0f;  PRT1_KeyFrame[12].Comp[1] = -5.0f; PRT1_KeyFrame[12].Comp[2] =  300.0f; PRT1_KeyFrame[12].Comp[3] = 180.0f;
	PRT1_KeyFrame[13].Comp[0] = 300.0f;  PRT1_KeyFrame[13].Comp[1] = -5.0f; PRT1_KeyFrame[13].Comp[2] =  200.0f; PRT1_KeyFrame[13].Comp[3] = 180.0f;
	PRT1_KeyFrame[14].Comp[0] = 300.0f;  PRT1_KeyFrame[14].Comp[1] = -5.0f; PRT1_KeyFrame[14].Comp[2] =  100.0f; PRT1_KeyFrame[14].Comp[3] = 180.0f;
	PRT1_KeyFrame[15].Comp[0] = 300.0f;  PRT1_KeyFrame[15].Comp[1] = -5.0f; PRT1_KeyFrame[15].Comp[2] =    0.0f; PRT1_KeyFrame[15].Comp[3] = 180.0f;
	PRT1_KeyFrame[16].Comp[0] = 300.0f;  PRT1_KeyFrame[16].Comp[1] = -5.0f; PRT1_KeyFrame[16].Comp[2] = -100.0f; PRT1_KeyFrame[16].Comp[3] = 180.0f;
	PRT1_KeyFrame[17].Comp[0] = 300.0f;  PRT1_KeyFrame[17].Comp[1] = -5.0f; PRT1_KeyFrame[17].Comp[2] = -200.0f; PRT1_KeyFrame[17].Comp[3] = 180.0f;
	PRT1_KeyFrame[18].Comp[0] = 300.0f;  PRT1_KeyFrame[18].Comp[1] = -5.0f; PRT1_KeyFrame[18].Comp[2] = -300.0f; PRT1_KeyFrame[18].Comp[3] = 180.0f;
	PRT1_KeyFrame[19].Comp[0] = 300.0f;  PRT1_KeyFrame[19].Comp[1] = -5.0f; PRT1_KeyFrame[19].Comp[2] = -300.0f; PRT1_KeyFrame[19].Comp[3] = 270.0f;
	PRT1_KeyFrame[20].Comp[0] = 150.0f;  PRT1_KeyFrame[20].Comp[1] = -5.0f;  PRT1_KeyFrame[20].Comp[2] = -300.0f;  PRT1_KeyFrame[20].Comp[3] = 270.0f;
	PRT1_KeyFrame[21].Comp[0] =   0.0f;  PRT1_KeyFrame[21].Comp[1] = -5.0f;  PRT1_KeyFrame[21].Comp[2] = -300.0f;  PRT1_KeyFrame[21].Comp[3] = 270.0f;
	PRT1_KeyFrame[22].Comp[0] =-150.0f;  PRT1_KeyFrame[22].Comp[1] = -5.0f;  PRT1_KeyFrame[22].Comp[2] = -300.0f;  PRT1_KeyFrame[22].Comp[3] = 270.0f;
	PRT1_KeyFrame[23].Comp[0] =-250.0f;  PRT1_KeyFrame[23].Comp[1] = -5.0f;  PRT1_KeyFrame[23].Comp[2] = -300.0f;  PRT1_KeyFrame[23].Comp[3] = 270.0f;
	PRT1_KeyFrame[24].Comp[0] =-250.0f;  PRT1_KeyFrame[24].Comp[1] = -5.0f;  PRT1_KeyFrame[24].Comp[2] = -300.0f;  PRT1_KeyFrame[24].Comp[3] = 450.0f;
}
void resetAnimation() {
	play = true;
	playIndex = 0;
	i_curr_steps = 0;
	resetElements();
	interpolation();
}
int main() {
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
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

	ModelAnim Condrack("resources/objects/Condrack/Condrack_Escena1.dae");
	Condrack.initShaders(animShader.ID);
	ModelAnim Caballo("resources/objects/Carretilla/Caballo.dae");
	Caballo.initShaders(animShader.ID);
	ModelAnim Soldado("resources/objects/Soldado/Marchar.dae");
	Soldado.initShaders(animShader.ID);

	std::thread soundtrack(&playSoundTrack);

	//Reproduccion del Soundtrack
	soundtrack.detach();
	setKeyFrames();
	resetAnimation();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)){
		skyboxShader.setInt("skybox", 0);
		lastFrame = SDL_GetTicks();

		animate();
		if (!play)
			resetAnimation();
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

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, -5.0f, 300.0f));
		model = glm::scale(model, glm::vec3(2.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		animShader.setMat4("model", model);
		Condrack.Draw(animShader);*/

		model = glm::translate(glm::mat4(1.0f), glm::vec3( patrol1[0], patrol1[1], patrol1[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrol1[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrol1[0] + 13.0f, patrol1[1], patrol1[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrol1[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrol1[0] - 13.0f, patrol1[1], patrol1[2]));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrol1[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrol1[0], patrol1[1], patrol1[2] - 15.0f));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrol1[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(patrol1[0], patrol1[1], patrol1[2] + 15.0f));
		model = glm::scale(model, glm::vec3(SOLDIER_SIZE));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(patrol1[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		animShader.setMat4("model", model);
		Soldado.Draw(animShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(carreta[0], carreta[1] - 7.0f, carreta[2]));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(carreta[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		animShader.setMat4("model", model);
		Caballo.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		// -------------------------------------------------------------------------------------------------------------------------
		// Carreta
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0, 6.5f, -3.0f));
		staticShader.setMat4("model", model);
		Carretilla.Draw(staticShader);
		model = glm::translate(model, glm::vec3(6.25f,  -2.9f, -11.0f));
		staticShader.setMat4("model", model);
		Rueda.Draw(staticShader);
		model = glm::translate(model, glm::vec3(-12.5f,  0.0f,  0.0f));
		model = glm::scale(model, glm::vec3(-1.0f));
		staticShader.setMat4("model", model);
		Rueda.Draw(staticShader);
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