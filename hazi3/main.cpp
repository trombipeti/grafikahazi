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
	}
};

struct Light
{
	GLenum id;
	Vector pos;
	float Id[4];
	float Ia[4];
	float Is[4];

	void setOGL()
	{

	}
};

struct Henger: public Object
{
	float r;

};

struct Torus: public Object
{
	// A külső sugár
	float outR;
	// A körök sugara
	float inR;
	// Tesszaláció során egy-egy henger "oldalszáma"
	float sides;
	// Tesszaláció során hány hengerre osszuk fel
	float rings;

	Torus(float innerRadius, float outerRadius, float numSides, float numRings)
	{
		outR = outerRadius;
		inR = innerRadius;
		sides = numSides;
		rings = numRings;
	}

	float x(float phi, float theta)
	{
		return (outR + inR * cos(phi)) * cos(theta);
	}

	float y(float phi, float theta)
	{
		return (outR + inR * cos(phi)) * sin(theta);
	}

	float z(float phi, float theta)
	{
		return inR * sin(phi);
	}

	float xdx(float phi, float theta)
	{
		return inR * (cos(phi) * sin(theta) - sin(phi) * cos(theta));
	}

	float ydy(float phi, float theta)
	{
		return inR * (cos(theta) * cos(phi) - sin(phi) * cos(theta));
	}

	float zdz(float phi, float theta)
	{
		return cos(phi) * inR;
	}

	void draw()
	{
		float phi_step = (2 * M_PI / rings);
		float theta_step = (2 * M_PI / sides);

//		glTranslatef(0, 0, -1);
		glRotatef(FOK, 0, 1, 0);

//		glutSolidTorus(inR,outR,sides,rings);
//		return;
		glBegin(GL_QUADS);
		for (float phi = 0; phi <= 2 * M_PI; phi += phi_step)
		{
			for (float theta = 0; theta <= 2 * M_PI; theta += theta_step)
			{
				float x1 = x(phi, theta);
				float y1 = y(phi, theta);
				float z1 = z(phi, theta);
				float x1dx = xdx(phi, theta);
				float y1dy = ydy(phi, theta);
				float z1dz = zdz(phi, theta);
				glNormal3f(x1dx, y1dy, z1dz);
				glVertex3f(x1, y1, z1);

				float x2 = x(phi + phi_step, theta);
				float y2 = y(phi + phi_step, theta);
				float z2 = z(phi + phi_step, theta);
				float x2dx = xdx(phi + phi_step, theta);
				float y2dy = ydy(phi + phi_step, theta);
				float z2dz = zdz(phi + phi_step, theta);
				glNormal3f(x2dx, y2dy, z2dz);
				glVertex3f(x2, y2, z2);

				float x3 = x(phi + phi_step, theta + theta_step);
				float y3 = y(phi + phi_step, theta + theta_step);
				float z3 = z(phi + phi_step, theta + theta_step);
				float x3dx = xdx(phi + phi_step, theta + theta_step);
				float y3dy = ydy(phi + phi_step, theta + theta_step);
				float z3dz = zdz(phi + phi_step, theta + theta_step);
				glNormal3f(x3dx, y3dy, z3dz);
				glVertex3f(x3, y3, z3);

				float x4 = x(phi, theta + theta_step);
				float y4 = y(phi, theta + theta_step);
				float z4 = z(phi, theta + theta_step);
				float x4dx = xdx(phi, theta + theta_step);
				float y4dy = ydy(phi, theta + theta_step);
				float z4dz = zdz(phi, theta + theta_step);
				glNormal3f(x4dx, y4dy, z4dz);
				glVertex3f(x4, y4, z4);
			}
		}
		glEnd();
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
		Torus *t = new Torus(0.6, 1.1, 50, 50);
		objects[numObjects++] = t;
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
	scene.build();

	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	float p[4] =
	{ 0.0f, 0.0f, 0.0f, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, p);
	float c[4] =
	{ 0.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, c);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.9f);

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
//	FOK = fmod(time / 10, 360.0f);
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
