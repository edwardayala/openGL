#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GLUT/glut.h>

#undef PI
#define PI 3.141592657

enum {
  NORMAL = 0,
  WEIRD = 1
};

enum {
  STREAK = 0,
  CIRCLE = 1
};

#define MAXstar 400
#define MAXPOS 10000
#define MAXWARP 10
#define MAXANGLES 6000

typedef struct _starRec {
  GLint type;
  float x[2], y[2], z[2];
  float offsetX, offsetY, offsetR, rotation;
} starRec;

GLenum doubleBuffer;
GLint windW = 300, windH = 300;

GLenum flag = NORMAL;
GLint starCount = MAXstar / 2;
float speed = 1.0;
GLint nitro = 0;
starRec star[MAXstar];
float sinTable[MAXANGLES];

float Sin(float angle)
{
  return (sinTable[(GLint) angle]);
}

float Cos(float angle)
{
  return (sinTable[((GLint) angle + (MAXANGLES / 4)) % MAXANGLES]);
}

void NewStar(GLint n, GLint d)
{
  if (rand() % 4 == 0) {
    star[n].type = CIRCLE;
  } else {
    star[n].type = STREAK;
  }
  star[n].x[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
  star[n].y[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
  star[n].z[0] = (float) (rand() % MAXPOS + d);
  star[n].x[1] = star[n].x[0];
  star[n].y[1] = star[n].y[0];
  star[n].z[1] = star[n].z[0];
  if (rand() % 4 == 0 && flag == WEIRD) {
    star[n].offsetX = (float) (rand() % 100 - 100 / 2);
    star[n].offsetY = (float) (rand() % 100 - 100 / 2);
    star[n].offsetR = (float) (rand() % 25 - 25 / 2);
  } else {
    star[n].offsetX = 0.0;
    star[n].offsetY = 0.0;
    star[n].offsetR = 0.0;
  }
}

void RotatePoint(float *x, float *y, float rotation)
{
  float tmpX, tmpY;

  tmpX = *x * Cos(rotation) - *y * Sin(rotation);
  tmpY = *y * Cos(rotation) + *x * Sin(rotation);
  *x = tmpX;
  *y = tmpY;
}

void Movestar(void)
{
  float offset;
  GLint n;

  offset = speed * 60.0;

  for (n = 0; n < starCount; n++) {
    star[n].x[1] = star[n].x[0];
    star[n].y[1] = star[n].y[0];
    star[n].z[1] = star[n].z[0];
    star[n].x[0] += star[n].offsetX;
    star[n].y[0] += star[n].offsetY;
    star[n].z[0] -= offset;
    star[n].rotation += star[n].offsetR;
    if (star[n].rotation > MAXANGLES) {
      star[n].rotation = 0.0;
    }
  }
}

GLenum StarPoint(GLint n)
{
  float x0, y0;

  x0 = star[n].x[0] * windW / star[n].z[0];
  y0 = star[n].y[0] * windH / star[n].z[0];
  RotatePoint(&x0, &y0, star[n].rotation);
  x0 += windW / 2.0;
  y0 += windH / 2.0;

  if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH) {
    return GL_TRUE;
  } else {
    return GL_FALSE;
  }
}

void ShowStar(GLint n)
{
  float x0, y0, x1, y1, width;
  GLint i;

  x0 = star[n].x[0] * windW / star[n].z[0];
  y0 = star[n].y[0] * windH / star[n].z[0];
  RotatePoint(&x0, &y0, star[n].rotation);
  x0 += windW / 2.0;
  y0 += windH / 2.0;

  if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH) {
    if (star[n].type == STREAK) {
      x1 = star[n].x[1] * windW / star[n].z[1];
      y1 = star[n].y[1] * windH / star[n].z[1];
      RotatePoint(&x1, &y1, star[n].rotation);
      x1 += windW / 2.0;
      y1 += windH / 2.0;

      glLineWidth(MAXPOS / 100.0 / star[n].z[0] + 1.0);
      glColor3f(1.0, (MAXWARP - speed) / MAXWARP, (MAXWARP - speed) / MAXWARP);
      if (fabs(x0 - x1) < 1.0 && fabs(y0 - y1) < 1.0) {
        glBegin(GL_POINTS);
        glVertex2f(x0, y0);
        glEnd();
      } else {
        glBegin(GL_LINES);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glEnd();
      }
    } else {
      width = MAXPOS / 10.0 / star[n].z[0] + 1.0;
      glColor3f(1.0, 0.0, 0.0);
      glBegin(GL_POLYGON);
      for (i = 0; i < 8; i++) {
        float x = x0 + width * Cos((float) i * MAXANGLES / 8.0);
        float y = y0 + width * Sin((float) i * MAXANGLES / 8.0);
        glVertex2f(x, y);
      };
      glEnd();
    }
  }
}

void Updatestar(void)
{
  GLint n;

  glClear(GL_COLOR_BUFFER_BIT);

  for (n = 0; n < starCount; n++) {
    if (star[n].z[0] > speed || (star[n].z[0] > 0.0 && speed < MAXWARP)) {
      if (StarPoint(n) == GL_FALSE) {
        NewStar(n, MAXPOS);
      }
    } else {
      NewStar(n, MAXPOS);
    }
  }
}

void Showstar(void)
{
  GLint n;

  glClear(GL_COLOR_BUFFER_BIT);

  for (n = 0; n < starCount; n++) {
    if (star[n].z[0] > speed || (star[n].z[0] > 0.0 && speed < MAXWARP)) {
      ShowStar(n);
    }
  }
}

static void Init(void)
{
  float angle;
  GLint n;

  srand((unsigned int) time(NULL));

  for (n = 0; n < MAXstar; n++) {
    NewStar(n, 100);
  }

  angle = 0.0;
  for (n = 0; n <= MAXANGLES; n++) {
    sinTable[n] = sin(angle);
    angle += PI / (MAXANGLES / 2.0);
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glDisable(GL_DITHER);
}

void Reshape(int width, int height)
{
  windW = width;
  windH = height;

  glViewport(0, 0, windW, windH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-0.5, windW + 0.5, -0.5, windH + 0.5);
  glMatrixMode(GL_MODELVIEW);
}

/* ARGSUSED1 */
static void Key(unsigned char key, int x, int y)
{
  switch (key) {
  case ' ':
    flag = (flag == NORMAL) ? WEIRD : NORMAL;
    break;
  case 't':
    nitro = 1;
    break;
  case 27:
    exit(0);
  }
}

void Idle(void)
{
  Movestar();
  Updatestar();
  if (nitro > 0) {
    speed = (float) (nitro / 10) + 1.0;
    if (speed > MAXWARP) {
      speed = MAXWARP;
    }
    if (++nitro > MAXWARP * 10) {
      nitro = -nitro;
    }
  } else if (nitro < 0) {
    nitro++;
    speed = (float) (-nitro / 10) + 1.0;
    if (speed > MAXWARP) {
      speed = MAXWARP;
    }
  }
  glutPostRedisplay();
}

void Display(void)
{
  Showstar();
  if (doubleBuffer) {
    glutSwapBuffers();
  } else {
    glFlush();
  }
}

void Visible(int state)
{
  if (state == GLUT_VISIBLE) {
    glutIdleFunc(Idle);
  } else {
    glutIdleFunc(NULL);
  }
}

static void Args(int argc, char **argv)
{
  GLint i;

  doubleBuffer = GL_TRUE;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-sb") == 0) {
      doubleBuffer = GL_FALSE;
    } else if (strcmp(argv[i], "-db") == 0) {
      doubleBuffer = GL_TRUE;
    }
  }
}

int main(int argc, char **argv)
{
  GLenum type;

  glutInitWindowSize(windW, windH);
  glutInit(&argc, argv);
  Args(argc, argv);

  type = GLUT_RGB;
  type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
  glutInitDisplayMode(type);
  glutCreateWindow("star");

  Init();

  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Key);
  glutVisibilityFunc(Visible);
  glutDisplayFunc(Display);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
