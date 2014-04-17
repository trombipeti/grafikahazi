//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Trombitas Peter
// Neptun : G08HLM
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

float FOK = 0;

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector
{
	float x, y, z;

	Vector()
	{
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0)
	{
		x = x0;
		y = y0;
		z = z0;
	}
	Vector operator*(float a) const
	{
		return Vector(x * a, y * a, z * a);
	}
	Vector operator/(float a) const
	{
		return Vector(x / a, y / a, z / a);
	}
	Vector operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) const  	// dot product
	{
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) const  	// cross product
	{
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vector norm() const
	{
		float invl = 1 / Length();  	// Q_rsqrt(x * x + y * y + z * z);
		return (*this * invl);
	}
};

Vector operator*(float f, const Vector& v)
{
	return Vector(v.x * f, v.y * f, v.z * f);
}

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color
{
	float r, g, b;

	Color()
	{
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0)
	{
		r = r0;
		g = g0;
		b = b0;
	}
	Color operator*(float a)
	{
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c)
	{
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c)
	{
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

struct Object
{
	virtual void draw() = 0;
	virtual ~Object()
	{
	}
};

struct Camera
{
	Vector eye;
	Vector lookat;
	Vector vup;

	float fov;
	float asp;
	float znear;
	float zfar;

	Camera(Vector theEye, Vector lookAt, Vector up, float fovy, float aspect,
			float zNear, float zFar)
	{
		eye = theEye;
		lookat = lookAt;
		vup = up;
		fov = fovy;
		asp = aspect;
		znear = zNear;
		zfar = zFar;
	}

	void setOGL()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, asp, znear, zfar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z, lookat.x, lookat.y, lookat.z, vup.x,
				vup.y, vup.z);
		glRotatef(FOK, 0, 1, 0);
	}
};

struct Light
{
	GLenum id;
	Vector pos;
	Color Id;
	Color Ia;
	Color Is;

	Light(GLenum ID, Vector position, Color _Id, Color _Ia, Color _Is)
	{
		id = ID;
		pos = position;
		Id = _Id;
		Ia = _Ia;
		Is = _Is;
		glEnable(id);

		setOGL();
	}

	void setOGL()
	{
		float p[4] =
		{ pos.x, pos.y, pos.z, 1 };
		glLightfv(id, GL_POSITION, p);
		glLightfv(id, GL_DIFFUSE, (float*) &Id);
		glLightfv(id, GL_AMBIENT, (float*) &Ia);
		glLightfv(id, GL_SPECULAR, (float*) &Is);

		glLightf(id, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(id, GL_QUADRATIC_ATTENUATION, 0.9f);
	}
};

struct Henger: public Object
{
	float r;
	float h;
	float rings;
	float sides;

	Henger(float radius, float height, float numRings, float numSides)
	{
		r = radius;
		h = height;
		rings = numRings;
		sides = numSides;
	}

	float x(float u, float v)
	{
		return r * cos(2 * M_PI * u);
	}

	float y(float u, float v)
	{
		return r * sin(2 * M_PI * u);
	}

	float z(float u, float v)
	{
		return -h * v;
	}

	float xdu(float u, float v)
	{
		return (-1.0f) * r * sin(2 * M_PI * u);
	}

	float ydu(float u, float v)
	{
		return r * cos(2 * M_PI * u);
	}

	float zdu(float u, float v)
	{
		return 0;
	}

	float xdv(float u, float v)
	{
		return 0;
	}

	float ydv(float u, float v)
	{
		return 0;
	}

	float zdv(float u, float v)
	{
		return -h;
	}

	void draw()
	{
		float u_step = 1.0f / sides;
		float v_step = (h / rings);

		glBegin(GL_TRIANGLES);
		for (float f = 0; f < 2 * M_PI; f += v_step * 2 * M_PI)
		{
			glNormal3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(cos(f) * r, sin(f) * r, 0);
			glVertex3f(cos(f + v_step * 2 * M_PI) * r,
					sin(f + v_step * 2 * M_PI) * r, 0);
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		for (float f = 0; f < 2 * M_PI; f += v_step * 2 * M_PI)
		{
			glNormal3f(0, 0, -1);
			glVertex3f(0, 0, -h);
			glVertex3f(cos(f) * r, sin(f) * r, -h);
			glVertex3f(cos(f + v_step * 2 * M_PI) * r,
					sin(f + v_step * 2 * M_PI) * r, -h);
		}
		glEnd();

		for (float u = 0; u < 1; u += u_step)
		{
			for (float v = 0; v < 1; v += v_step)
			{
				glBegin(GL_TRIANGLES);

				float x1 = x(u, v);
				float y1 = y(u, v);
				float z1 = z(u, v);
				float x1du = xdu(u, v);
				float y1du = ydu(u, v);
				float z1du = zdu(u, v);
				float x1dv = xdv(u, v);
				float y1dv = ydv(u, v);
				float z1dv = zdv(u, v);
				Vector n1 = Vector(x1dv, y1dv, z1dv) % Vector(x1du, y1du, z1du);

				float x2 = x(u + u_step, v);
				float y2 = y(u + u_step, v);
				float z2 = z(u + u_step, v);
				float x2du = xdu(u + u_step, v);
				float y2du = ydu(u + u_step, v);
				float z2du = zdu(u + u_step, v);
				float x2dv = xdv(u + u_step, v);
				float y2dv = ydv(u + u_step, v);
				float z2dv = zdv(u + u_step, v);
				Vector n2 = Vector(x2dv, y2dv, z2dv) % Vector(x2du, y2du, z2du);

				float x3 = x(u + u_step, v + v_step);
				float y3 = y(u + u_step, v + v_step);
				float z3 = z(u + u_step, v + v_step);
				float x3du = xdu(u + u_step, v + v_step);
				float y3du = ydu(u + u_step, v + v_step);
				float z3du = zdu(u + u_step, v + v_step);
				float x3dv = xdv(u + u_step, v + v_step);
				float y3dv = ydv(u + u_step, v + v_step);
				float z3dv = zdv(u + u_step, v + v_step);
				Vector n3 = Vector(x3dv, y3dv, z3dv) % Vector(x3du, y3du, z3du);

				float x4 = x(u, v + v_step);
				float y4 = y(u, v + v_step);
				float z4 = z(u, v + v_step);
				float x4du = xdu(u, v + v_step);
				float y4du = ydu(u, v + v_step);
				float z4du = zdu(u, v + v_step);
				float x4dv = xdv(u, v + v_step);
				float y4dv = ydv(u, v + v_step);
				float z4dv = zdv(u, v + v_step);
				Vector n4 = Vector(x4dv, y4dv, z4dv) % Vector(x4du, y4du, z4du);

				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(x1, y1, z1);
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(x2, y2, z2);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(x3, y3, z3);

				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(x1, y1, z1);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(x3, y3, z3);
				glNormal3f(n4.x, n4.y, n4.z);
				glVertex3f(x4, y4, z4);

				glEnd();
			}
		}
	}

};

struct Torus: public Object
{
	float outR;

	float inR;

	float sides;

	float rings;

	Torus(float innerRadius, float outerRadius, float numSides, float numRings)
	{
		outR = outerRadius;
		inR = innerRadius;
		sides = numSides;
		rings = numRings;
	}

	float x(float u, float v)
	{
		return (outR + inR * cos(2 * M_PI * u)) * cos(2 * M_PI * v);
	}

	float y(float u, float v)
	{
		return (outR + inR * cos(2 * M_PI * u)) * sin(2 * M_PI * v);
	}

	float z(float u, float v)
	{
		return inR * sin(2 * M_PI * u);
	}

	float xdu(float u, float v)
	{
		return (-1.0f) * sin(2 * M_PI * u) * cos(2 * M_PI * v);
	}

	float ydu(float u, float v)
	{
		return (-1.0f) * sin(2 * M_PI * u) * sin(2 * M_PI * v);
	}

	float zdu(float u, float v)
	{
		return cos(2 * M_PI * u);
	}

	float xdv(float u, float v)
	{
		return (-1.0f) * sin(2 * M_PI * v);
	}

	float ydv(float u, float v)
	{
		return cos(2 * M_PI * v);
	}

	float zdv(float u, float v)
	{
		return 0;
	}

	void draw()
	{
		float u_step = (1.0f / sides);
		float v_step = (1.0f / rings);

//		glRotatef(FOK, 0, 1, 0);

		for (float u = 0; u < 1; u += u_step)
		{
			for (float v = 0; v < 1; v += v_step)
			{
				glBegin(GL_TRIANGLES);

				float x1 = x(u, v);
				float y1 = y(u, v);
				float z1 = z(u, v);
				float x1du = xdu(u, v);
				float y1du = ydu(u, v);
				float z1du = zdu(u, v);
				float x1dv = xdv(u, v);
				float y1dv = ydv(u, v);
				float z1dv = zdv(u, v);
				Vector n1 = Vector(x1dv, y1dv, z1dv) % Vector(x1du, y1du, z1du);

				float x2 = x(u + u_step, v);
				float y2 = y(u + u_step, v);
				float z2 = z(u + u_step, v);
				float x2du = xdu(u + u_step, v);
				float y2du = ydu(u + u_step, v);
				float z2du = zdu(u + u_step, v);
				float x2dv = xdv(u + u_step, v);
				float y2dv = ydv(u + u_step, v);
				float z2dv = zdv(u + u_step, v);
				Vector n2 = Vector(x2dv, y2dv, z2dv) % Vector(x2du, y2du, z2du);

				float x3 = x(u + u_step, v + v_step);
				float y3 = y(u + u_step, v + v_step);
				float z3 = z(u + u_step, v + v_step);
				float x3du = xdu(u + u_step, v + v_step);
				float y3du = ydu(u + u_step, v + v_step);
				float z3du = zdu(u + u_step, v + v_step);
				float x3dv = xdv(u + u_step, v + v_step);
				float y3dv = ydv(u + u_step, v + v_step);
				float z3dv = zdv(u + u_step, v + v_step);
				Vector n3 = Vector(x3dv, y3dv, z3dv) % Vector(x3du, y3du, z3du);

				float x4 = x(u, v + v_step);
				float y4 = y(u, v + v_step);
				float z4 = z(u, v + v_step);
				float x4du = xdu(u, v + v_step);
				float y4du = ydu(u, v + v_step);
				float z4du = zdu(u, v + v_step);
				float x4dv = xdv(u, v + v_step);
				float y4dv = ydv(u, v + v_step);
				float z4dv = zdv(u, v + v_step);
				Vector n4 = Vector(x4dv, y4dv, z4dv) % Vector(x4du, y4du, z4du);

				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(x1, y1, z1);
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(x2, y2, z2);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(x3, y3, z3);

				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(x1, y1, z1);
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(x3, y3, z3);
				glNormal3f(n4.x, n4.y, n4.z);
				glVertex3f(x4, y4, z4);

				glEnd();

			}
		}
	}
};

struct Kerek: public Object
{
	Torus *gumi;
	Henger **kullok;
	int kulloszam;
};

struct BringaVaz: public Object
{

};

struct Kormanymu: public Object
{
	Kerek *kerek;
	Henger *balvilla;
	Henger *jobbvilla;
	Henger *villateteje;
	Henger *villanyak;
	Henger *stucni;
	Henger *kormany;

};

struct Ember: public Object
{

};

struct Bringa: public Object
{
	Kerek *hatsokerek;
	Kormanymu *kormany;
	Ember *rider;
	BringaVaz *vaz;

	void draw()
	{

	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

struct Scene
{
	Camera *camera;

	Object* objects[100];
	int numObjects;

	Light *lights[8];
	int numLights;

	Scene()
	{
		camera = NULL;
		numObjects = 0;
		numLights = 0;
	}

	~Scene()
	{
		if (camera)
			delete camera;
		for (int i = 0; i < numObjects; ++i)
		{
			delete objects[i];
		}
		for (int i = 0; i < numLights; ++i)
		{
			delete lights[i];
		}
	}

	void build()
	{
		camera = new Camera(Vector(0, 0, 1.5), Vector(0, 0, 0), Vector(0, 1, 0),
				90.0f, 1.0f, 0.1f, 10.0f);
		Torus *t = new Torus(0.1, 0.5, 80, 80);
		objects[numObjects++] = t;

		Henger * h = new Henger(0.1, 1.2, 20, 20);
		objects[numObjects++] = h;

		Color diffuse(1, 1, 1);
		Color ambient(0.2, 0.2, 0.2);
		Color specular(1, 1, 1);
		Light *l0 = new Light(GL_LIGHT0, Vector(0, 0, 2), diffuse, ambient,
				specular);
		lights[numLights++] = l0;
	}

	void render()
	{
		camera->setOGL();
		for (int i = 0; i < numLights; ++i)
		{
			lights[i]->setOGL();
		}
		for (int i = 0; i < numObjects; ++i)
		{
			objects[i]->draw();
		}
		Torus t(0.05, 0.1, 30, 30);
		glPushMatrix();
//		glLoadIdentity();
		glTranslatef(lights[0]->pos.x, lights[0]->pos.y, lights[0]->pos.z);
		t.draw();
		glPopMatrix();
	}
};

Scene scene;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization()
{
	glViewport(0, 0, screenWidth, screenHeight);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	scene.build();
}
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
// ..
	scene.render();
	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		scene.camera->eye.x -= 1;
		break;
	case 'd':
		scene.camera->eye.x += 1;
		break;
	case 'k':
		FOK -= 1;
		break;
	case 'l':
		FOK += 1;
		break;
	case 's':
		glShadeModel(GL_SMOOTH);
		break;
	case 'f':
		glShadeModel(GL_FLAT);
		break;
	default:
		break;
	}

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y)
{

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay(); 					// Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle()
{
	long time = glutGet(GLUT_ELAPSED_TIME);	// program inditasa ota eltelt ido
//	FOK = fmod(time / 25, 360.0f);
	scene.lights[0]->pos.x = cos(fmod((time / 10) * 0.01, 360.0f));
//	scene.lights[0]->pos.y = 2 * cos(fmod((time / 10) * 0.01, 360.0f)) * sin(fmod((time / 10) * 0.05, 360.0f));
	scene.lights[0]->pos.z = sin(fmod((time / 10) * 0.01, 360.0f));
	glutPostRedisplay();

}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv)
{
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600); // Alkalmazas ablak kezdeti merete 600x600 pixel
	glutInitWindowPosition(100, 100); // Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat"); // Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);	// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();				// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}
