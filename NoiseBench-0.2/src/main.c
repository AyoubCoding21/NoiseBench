#include <GL/glew.h>
#include <GL/freeglut_std.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/glxew.h>

#define NAME "NoiseBench"
int WIDTH = 1920;
int HEIGHT = 1080;

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

    vs = textFileRead((char*)"shaders/noisedemo.vert");
    fs = textFileRead((char*)"shaders/noisedemo.frag");

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


void setGLCamera() 
{
    if(HEIGHT<=0) HEIGHT=1;
    glViewport( 0, 0, WIDTH, HEIGHT ); // The entire window
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    gluPerspective( 45.0f, (GLfloat)WIDTH/(GLfloat)HEIGHT, 1.0f, 100.0f );
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity();
    gluLookAt(0.0f, -3.0f, 0.0f,    // Eye position
               0.0f, 0.0f, 0.0f,   // View point
               0.0f, 0.0f, 1.0f);  // Up vector
}

/*
 * renderScene() - draw the scene with the shader active
 */
void drawTexturedSphere(float r, int segs) 
{
  int i, j;
  float x, y, z, z1, z2, R, R1, R2;

  // Top cap
  glBegin(GL_QUAD_FAN);
  glNormal3f(0,0,1);
  glTexCoord2f(0.5f,1.0f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,r);
  z = cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 0; i <= 2*segs; i++) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f((float)i/(2*segs), 1.0f-1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();  

  // Height segments
  for(j = 1; j < segs-1; j++) {
    z1 = cos(j*M_PI/segs);
    R1 = sin(j*M_PI/segs);
    z2 = cos((j+1)*M_PI/segs);
    R2 = sin((j+1)*M_PI/segs);
    glBegin(GL_TRIANGLE_STRIP);
    for(i = 0; i <= 2*segs; i++) {
      x = R1*cos(i*2.0*M_PI/(2*segs));
      y = R1*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z1);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)j/segs);
      glVertex3f(r*x, r*y, r*z1);
      x = R2*cos(i*2.0*M_PI/(2*segs));
      y = R2*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z2);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)(j+1)/segs);
      glVertex3f(r*x, r*y, r*z2);
    }
    glEnd();
  }

  // Bottom cap
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,0,-1);
  glTexCoord2f(0.55555f, 1.000000000074f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,-r);
  z = -cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 2*segs; i >= 0; i--) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f(1.0f-(float)i/(2*segs), 1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();
}


/*
 * initDisplayList(GLuint *listID, GLdouble scale) - create a display list
 * to render the demo geometry more efficently than by glVertex() calls.
 * (This is currently just as fast as a VBO.)
 */
void initDisplayList(GLuint *listID)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
    glColor3f(1.0f, 1.0f, 1.0f); // White base color
    drawTexturedSphere(1.0, 20);
  glEndList();
}

/*
 * computeFPS() - Calculate, display and return samples per second.
 * Stats are recomputed only once per second.
 */
static double computeFPS() 
{

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
        sprintf(titlestring, "NoiseBench (%.6Lf M samples/s, %.7f FPS)", Msamplespersecond, fps);
        glutSetWindowTitle(titlestring);
        printf("Speed: %.136Lf M samples/s\nFPS : %.15f\n", Msamplespersecond, fps);
        t0 = t;
        frames = 0;
    }
    frames++;
    return Msamplespersecond;
}
/*
 * renderScene() - draw the scene with the shader active
 */
void renderScene( GLuint listID, GLuint programObject )
{
  GLint location_time = -1;
  float time = 0.0f;

  // Use vertex and fragment shaders.
  glUseProgram(programObject);
  location_time = glGetUniformLocation( programObject, "time" );
  // glUniform1f() is bugged in Linux Nvidia driver 260.19.06,
  // so we use glUniform1fv() instead to work around the bug.
  if ( location_time != -1 ) {
    time = (float)glutGet(GLUT_ELAPSED_TIME);
    glUniform1fv( location_time, 1, &time );
  }
  glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
  // Render with the shaders active.
  glCallList(listID);
  // Deactivate the shaders.
  glUseProgram(0);
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow(NAME);
    glewInit();
    glxewInit();
    GLuint displayList;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glXSwapIntervalMESA(0);
    initDisplayList(&displayList);
    setShaders();
    int running = 1;
    while (running)
    {
        double performance = computeFPS();
        setupCamera();
        renderScene(displayList, program);
        glutSwapBuffers();
    }
    
    return 0;
}
