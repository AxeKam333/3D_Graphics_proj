#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "objloader.hpp"
#include "Object.h"

float speed_x=0;
float speed_y=0;
float aspectRatio=1;
int drinking = 0;
static uint32_t drinkCount = 0;

ShaderProgram *sp;


//Odkomentuj, żeby rysować kostkę
float* vertices1 = myCubeVertices;
float* normals1 = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;


//Odkomentuj, żeby rysować czajnik
float* vertices2 = myTeapotVertices;
float* normals2 = myTeapotVertexNormals;
float* texCoords2 = myTeapotTexCoords;
float* colors2 = myTeapotColors;
int vertexCount2 = myTeapotVertexCount;

static Object OBottle("models\\untitled.obj");
static Object OFloor("floor.obj");
//static Object OTable("models\\stol.obj");

GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;

// --- Obsługa kamery
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraMov_x = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraMov_y = glm::vec3(0.0f, 0.0f, 0.0f); // Wsm bardziej, camMov_z, ale zostawmy

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;

// --- Po nacisnieciu X nastepuje zamkniecie okna
bool XButtonPressed = false;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {

	const float cameraSpeed = 0.05f;

    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;

		if (key == GLFW_KEY_W) cameraMov_x = cameraSpeed * cameraFront;
		if (key == GLFW_KEY_S) cameraMov_x = -cameraSpeed * cameraFront;
		if (key == GLFW_KEY_A) cameraMov_y = - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (key == GLFW_KEY_D) cameraMov_y = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

		if (key == GLFW_KEY_X) XButtonPressed = true;
		if (key == GLFW_KEY_1 && drinking == 0 ) {
			drinking = 1;
		}
		if (key == GLFW_KEY_2 && drinking == 0) {
			drinking = 2;
		}
		if (key == GLFW_KEY_3 && drinking == 0) {
			drinking = 3;
		}
		if (key == GLFW_KEY_4 && drinking == 0) {
			drinking = 4;
		}
		if (key == GLFW_KEY_5 && drinking == 0) {
			drinking = 5;
		}
		if (key == GLFW_KEY_6 && drinking == 0) {
			drinking = 6;
		}
		if (key == GLFW_KEY_7 && drinking == 0) {
			drinking = 7;
		}
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;

		if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
			cameraMov_x = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		if (key == GLFW_KEY_A || key == GLFW_KEY_D){
			cameraMov_y = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

GLuint readTexture(const char* filename, int glParametr = GL_LINEAR) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");

	// bool res = loadOBJ("C:\\Users\\olekk\\OneDrive\\Pulpit\\programming studies\\g&v\\Projekt\\gkiw_st_11a_win\\models\\untitled.obj", vertices, uvs, normals);

	tex0 = readTexture("glass.png");
	tex1 = readTexture("sky.png");
	tex2 = readTexture("wood1.png");
	tex3 = readTexture("marmur.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

    delete sp;

	glDeleteTextures(3, &tex0);
}

void setupVertexAttribs(float * v, float * c, float * n, float * t) {
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, v);

    glEnableVertexAttribArray(sp->a("color"));
    glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, c);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, n);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, t);
}

void setupTextures(GLuint t0, GLuint t1, int option = 0) {
    glUniform1i(sp->u("textureMap0"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t0);
	if(option == 0) {
		glUniform1i(sp->u("textureMap1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, t1);
	}
}

void disableVertexAttribs() {
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("color"));
    glDisableVertexAttribArray(sp->a("normal"));
    glDisableVertexAttribArray(sp->a("texCoord0"));
}


glm::mat4 drink(glm::mat4 M,float angle_x, float angle_y,int drink_ID, bool show[]) {
	if (drinking != drink_ID) {
		
		M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
		
	}


	// Przesyłanie danych i rysowanie itema
	else {
		static float drink_time[10] = {0.0f,0.0f ,0.0f,0.0f , 0.0f,0.0f ,0.0f,0.0f , 0.0f,0.0f };
		const float max_drink_time = 2.5;
		drink_time[drink_ID] += 0.03;
		
		if (drink_time[drink_ID] > 3.4) {
			show[drink_ID] = false;
			drinkCount++;
			drinking = 0;
		}
		float drink_angle = fmin(drink_time[drink_ID], max_drink_time);
		//printf("%d\n", drink_angle);
		M = glm::translate(M, glm::vec3(0.0f, drink_angle / 5, 0.0f));
		M = glm::rotate(M, drink_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	return M;
}
glm::vec3 generateRandomOffset(float amplitude) {
	float offsetX = ((rand() % 2000) / 1000.0f - 1.0f) * amplitude;
	float offsetY = ((rand() % 2000) / 1000.0f - 1.0f) * amplitude;
	float offsetZ = ((rand() % 2000) / 1000.0f - 1.0f) * amplitude;

	

	return glm::vec3(offsetX, 0, offsetZ);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float time) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static bool show[10] = { true, true, true, true, true, true, true, true, true, true };

	glm::vec3 randomOffset = generateRandomOffset(drinkCount*0.003*drinkCount);
	 cameraPos = cameraPos + randomOffset;
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);

	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	// Macierz główna
	glm::mat4 M = glm::mat4(1.0f);

	if (drinkCount >= 6) {
		M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	//itemy tex
	setupTextures(tex0, tex1, 1);

	// Macierz modelu dla itema
	glm::mat4 MItem = M;
	MItem = glm::translate(MItem, glm::vec3(-2.0f, 0.0f, 0.0f)); // Przesuń pierwszy obiekt
	MItem = glm::scale(MItem, glm::vec3(0.1f, 0.1f, 0.3f));
	MItem = drink(MItem, angle_x, angle_y, 1, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem));


	if (show[1]) {
		OBottle.draw(sp);
	}


	glm::mat4 MItem2 = M;
	MItem2 = glm::translate(MItem2, glm::vec3(-1.0f, 0.0f, 0.0f));
	MItem2 = glm::scale(MItem2, glm::vec3(0.1f, 0.1f, 0.1f));
	MItem2 = drink(MItem2, angle_x, angle_y, 2, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem2));

	if (show[2]) {
	OBottle.draw(sp);
	}

	glm::mat4 MItem3 = M;
	MItem3 = glm::translate(MItem3, glm::vec3(0.0f, 0.0f, 0.0f));
	MItem3 = glm::scale(MItem3, glm::vec3(0.1f, 0.1f, 0.1f));
	MItem3 = drink(MItem3, angle_x, angle_y, 3, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem3));

	if (show[3]) {
		OBottle.draw(sp);
	}

	glm::mat4 MItem4 = M;
	MItem4 = glm::translate(MItem4, glm::vec3(1.0f, 0.0f, 0.0f));
	MItem4 = glm::scale(MItem4, glm::vec3(0.1f, 0.1f, 0.1f));
	MItem4 = drink(MItem4, angle_x, angle_y, 4, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem4));

	if (show[4]) {
		OBottle.draw(sp);
	}

	glm::mat4 MItem5 = M;
	MItem5 = glm::translate(MItem5, glm::vec3(2.0f, 0.0f, 0.0f));
	MItem5 = glm::scale(MItem5, glm::vec3(0.1f, 0.1f, 0.1f));
	MItem5 = drink(MItem5, angle_x, angle_y, 5, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem5));

	if (show[5]) {
		OBottle.draw(sp);
	}
	glm::mat4 MItem6 = M;
	MItem6 = glm::translate(MItem6, glm::vec3(3.0f, 0.0f, 0.0f));
	MItem6 = glm::scale(MItem6, glm::vec3(0.1f, 0.1f, 0.1f));
	MItem6 = drink(MItem6, angle_x, angle_y, 6, show);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MItem6));

	if (show[6]) {
		OBottle.draw(sp);
	}

	//TEXTURY TABLe
	setupTextures(tex3, tex1, 1);

	glm::mat4 MTable = M;
	MTable = glm::translate(MTable, glm::vec3(0.0f, -0.3f, 0.0f));
	MTable = glm::scale(MTable, glm::vec3(0.6f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MTable));
	
	OFloor.draw(sp);

	//podloga i sciany tex
	setupTextures(tex2, tex1, 1);


    // Macierz modelu dla podlogi
    glm::mat4 MFloor = M;
    MFloor=glm::translate(MFloor, glm::vec3(0, -2, 0));
	// MFloor=glm::scale(MFloor, glm::vec3(10, 0.5, 10));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MFloor));

    // Przesyłanie danych i rysowanie podlogi
	OFloor.draw(sp);
	
	glm::mat4 MCeiling = M;
    MCeiling=glm::translate(MCeiling, glm::vec3(0, 6, 0));
	// MCeiling=glm::scale(MCeiling, glm::vec3(10, 0.5, 10));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MCeiling));

    // Przesyłanie danych i rysowanie podlogi
	// glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	OFloor.draw(sp);

	for(int i=0; i<4; i++) {
		glm::mat4 MWall = M;
		MWall=glm::rotate(MWall, PI/2, glm::vec3(0, 0, 1));
		MWall=glm::rotate(MWall, i*PI/2, glm::vec3(1, 0, 0));
		MWall=glm::translate(MWall, glm::vec3(0, 8, 0));

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MWall));

		OFloor.draw(sp);
	}
    disableVertexAttribs();

    glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 750, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Główna pętla
	float angle_x=0; //Aktualny kąt obrotu obiektu
	float angle_y=0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window) && !XButtonPressed) //Tak długo jak okno nie powinno zostać zamknięte
	{
        angle_x+=(PI/2)*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		cameraPos += cameraMov_x + cameraMov_y; // Przesuwanie prawo/lewo oraz przód/tył przy wciskaniu WSAD
		cameraPos.y = 0.0f;
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y,glfwGetTime()); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
