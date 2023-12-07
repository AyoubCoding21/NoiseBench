#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glxew.h>

#define NAME "NoiseBench"
int WIDTH = 2560;  
int HEIGHT = 1440;

GLuint vert, frag, program;

/**
 * Prints any OpenGL errors.
 * @return 1 if there are errors, 0 if there are none.
 */
#define printOpenGLError() printGLError(__FILE__, __LINE__)
int printGLError(char* file, int line) {
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

/**
 * Reads the contents of a text file.
 * @param filename The name of the file to read.
 * @return The content of the file.
 */
char* textFileRead(char* filename) {
    FILE* fp;
    char* content = NULL;
    int count = 0;

    if (filename != NULL) {
        fp = fopen(filename, "rt");

        if (fp != NULL) {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}


/**
 * Prints the compile info log for a shader.
 * @param object The shader object.
 */
void printShaderInfoLog(GLuint object) {
    int infologLength = 0;
    int charsWritten = 0;
    char* infoLog;

    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char*)malloc(infologLength);
        glGetShaderInfoLog(object, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

/**
 * Prints the attaching info log for a shader program.
 * @param object The program object.
 */
void printProgramInfoLog(GLuint object) {
    int infologLength = 0;
    int charsWritten = 0;
    char* infoLog;

    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char*)malloc(infologLength);
        glGetProgramInfoLog(object, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

/**
 * Sets up the shaders.
 */
void setShaders() {
    char* vs = NULL, *fs = NULL;

    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);

    vs = textFileRead((char*)"shaders/vert.glsl");
    fs = textFileRead((char*)"shaders/frag.glsl");

    const char* vv = vs;
    const char* ff = fs;

    glShaderSource(vert, 1, &vv, NULL);
    glShaderSource(frag, 1, &ff, NULL);

    free(vs);
    free(fs);

    glCompileShader(vert);
    glCompileShader(frag);

    printShaderInfoLog(vert);
    printShaderInfoLog(frag);

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);

    glLinkProgram(program);
    printProgramInfoLog(program);
    glUseProgram(program);
}

/**
 * Processes keyboard input.
 * @param key The pressed key.
 * @param x The x-coordinate of the mouse.
 * @param y The y-coordinate of the mouse.
 */
void keycheck(unsigned char key, int x, int y) {
    if (key == 27)
        exit(0);
}

void setupCamera() 
{
    if(HEIGHT<=0) HEIGHT=1; // Safeguard against iconified/closed window
    glViewport(0, 0, WIDTH, HEIGHT); // The entire window
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity
}

/*
 * renderScene() - draw the scene with the shader active
 */
void renderScene(GLuint listID, GLuint programObject)
{
  GLint location_time = -1;
  float time = 0.0f;
  glUseProgram(programObject);
  location_time = glGetUniformLocation(programObject, "time");
  if ( location_time != -1 ) {
    time = (float)glutGet(GLUT_ELAPSED_TIME);
    glUniform1fv(location_time, 1, &time);
  }
  glCallList(listID);
  glUseProgram(0);
}

/*
 * computeFPS() - Calculate, display and return samples per second.
 * Stats are recomputed only once per second.
 */
static double computeFPS() {

    static long double t0 = 0.0;
    static int frames = 0;
    static long double Msamplespersecond = 0.0;
    static char titlestring[200];

    double t, fps;

    // Get current time
    t = glutGet(GLUT_ELAPSED_TIME);  // Gets number of seconds since glfwInit()
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (double)frames / (t-t0);
        Msamplespersecond = 1e-6*fps*WIDTH*HEIGHT;
        sprintf(titlestring, "NoiseBench (%.60Lf M samples/s)", Msamplespersecond);
        glutSetWindowTitle(titlestring);
        printf("Speed: %.136Lf M samples/s\n", Msamplespersecond);
        t0 = t;
        frames = 0;
    }
    frames++;
    return Msamplespersecond;
}

void initDisplayList(GLuint *listID)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
    glColor3f(1.0f, 1.0f, 1.0f); // White base color
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-1.0f, -1.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(1.0f, -1.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-1.0f, 1.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();
  glEndList();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow(NAME);
	glutSetOption(GLUT_MULTISAMPLE, 256);
    glewInit();
    glxewInit();
    GLuint displayList;
    glDisable(GL_DEPTH_TEST);
    glXSwapIntervalMESA(0);
    initDisplayList(&displayList);
    int running = 1;
    while (running)
    {
        setShaders();
        long double performance = computeFPS();
        setupCamera();
        renderScene(displayList, program);
        glutSwapBuffers();
    }
    
    return 0;
}
