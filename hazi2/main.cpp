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
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
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
#include <iostream>

template<typename T>
T MAX(T a, T b)
{
	return (a > b ? a : b);
}

template<typename T>
T MIN(T a, T b)
{
	return (a < b ? a : b);
}

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
		float l = Length();
		return (*this * (1.0f / l));
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
	Color operator*(float a) const
	{
		return Color(r * a, g * a, b * a);
	}
	Color operator/(float a) const
	{
		return Color(r / a, g / a, b / a);
	}
	Color operator*(const Color& c) const
	{
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator/(const Color& c) const
	{
		return Color(r / c.r, g / c.g, b / c.b);
	}
	Color operator+(const Color& c) const
	{
		return Color(r + c.r, g + c.g, b + c.b);
	}
	Color operator-(const Color& c) const
	{
		return Color(r - c.r, g - c.g, b - c.b);
	}

	Color operator+(float a) const
	{
		return Color(r + a, g + a, b + a);
	}
	Color operator-(float a) const
	{
		return Color(r - a, g - a, b - a);
	}

	Color operator+=(const Color& c)
	{
		return *this = *this + c;
	}

	Color saturated() const
	{
		return Color(MAX(MIN(r, 1.0f), 0.0f), MAX(MIN(g, 1.0f), 0.0f),
				MAX(MIN(b, 1.0f), 0.0f));
	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

double epsilon = 0.001;
int DMAX = 5;

float normCoordX(float x)
{
	return x * (2.0f / screenWidth) - 1.0f;
}

float normCoordY(float y)
{
	return (screenHeight - y) * (2.0f / screenHeight) - 1.0f;
}

struct Ray
{
	Vector p0;
	Vector dv;

	Ray(Vector _p0, Vector _dv) :
			p0(_p0), dv(_dv)
	{
	}
};

struct Intersection
{
	Vector p0;
	Vector nv;
	bool valid;
	int objectIndex;
	Intersection(Vector p = Vector(0, 0, 0), Vector n = Vector(0, 0, 0),
			bool _valid = false, int objInd = -1) :
			p0(p), nv(n), valid(_valid), objectIndex(objInd)
	{
	}
};

struct LightSource
{
	enum Type
	{
		AMBIENS, PONT
	} type;

	Color color;
	Vector p0;
	Vector dv;

	LightSource(Type t = AMBIENS, Color c = Color(1, 1, 1),
			Vector _p0 = Vector(0, 0, 0), Vector _dv = Vector(0, 0, -1)) :
			type(t), color(c), p0(_p0), dv(_dv)
	{
	}
};

struct Anyag
{
	virtual Color getColor(Intersection inter, const LightSource* lights,
			int numLights) = 0;
	virtual ~Anyag()
	{
	}
	;
};

static bool a = true;

struct DiffuzAnyag: public Anyag
{
	Color color;
	DiffuzAnyag(const Color& c)
	{
		color = c;
	}

	Color getColor(Intersection itrs, const LightSource* lights, int numLights)
	{
		Color retColor;

		for (int i = 0; i < numLights; ++i)
		{
			switch (lights[i].type)
			{
			case LightSource::AMBIENS:
				retColor += color * lights[i].color;
				break;
			case LightSource::PONT:
			{
				float intensity = 0;
				intensity = MAX((itrs.nv.norm() * lights[i].dv.norm()), 0.0f);
				retColor = retColor
						+ ((lights[i].color
								/ pow((lights[i].p0 - itrs.p0).Length(), 2))
								* color) * intensity;
				if (a)
				{
					Vector l = lights[i].dv.norm();
					Vector it = itrs.nv.norm();
					std::cout << "l: " << l.x << "," << l.y << "," << l.z
							<< std::endl;
					std::cout << "it: " << it.x << "," << it.y << "," << it.z
							<< std::endl;
					std::cout << intensity << std::endl;
					std::cout << "retColor: " << retColor.r << "," << retColor.g
							<< "," << retColor.b << std::endl;
					std::cout << "color: " << color.r << "," << color.g << ","
							<< color.b << std::endl;
					std::cout << "lights[i].color: " << lights[i].color.r << ","
							<< lights[i].color.g << "," << lights[i].color.b
							<< std::endl;
					a = false;
				}
			}
				break;
			default:
				break;
			}
		}
		return retColor.saturated();
	}
} Zold(Color(0.1, 0.4, 0.05));

struct Uveg: public Anyag
{
	Color F0;

	Uveg(Color n, Color k)
	{
		F0 = ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
	}

	Color F(float costheta)
	{
		return F0 + (Color(1, 1, 1) - F0) * pow(1 - costheta, 5);
	}

	Color getColor(Intersection inter, const LightSource* lights, int numLights)
	{

	}
};

struct Object
{
	Anyag *anyag;
	Object(Anyag* a = NULL) :
			anyag(a)
	{
	}
	virtual Intersection intersect(const Ray& ray) = 0;
	virtual ~Object()
	{
	}
	;
};

struct Uljanov: public Object
{
	Vector p1, p2;
	double c;

	Uljanov(Anyag* a, Vector _p1, Vector _p2, double _c)
	{
		p1 = _p1;
		p2 = _p2;
		c = _c;
		anyag = a;
	}

	Intersection intersect(const Ray& ray)
	{

		return Intersection();
	}
};

struct Czermanik: public Object
{
	Vector e1_dv;
	Vector e2_dv;
	double c;

	Czermanik(Vector e1, Vector e2, double _c, Anyag* a)
	{
		e1_dv = e1;
		e2_dv = e2;
		c = _c;
		anyag = a;
	}
};

struct Floor: public Object
{
	Vector p0;
	Vector nv;
	Floor(Anyag *a = NULL, Vector p = Vector(0, 0, 0),
			Vector n = Vector(0, 1, 0))
	{
		anyag = a;
		p0 = p;
		nv = n.norm();
	}

	Intersection intersect(const Ray& r)
	{
		float r_t = (p0 - r.p0) * nv / (r.dv * nv);
		if (!isnan(r_t) && r_t >= 0)
		{
			Vector metszes = r.p0 + r_t * r.dv;
			if ((metszes - p0).Length() > 0.3)
				return Intersection(metszes, nv, true);
		}
		return Intersection();
	}
	~Floor()
	{

	}
}*diffuseFloor = new Floor(&Zold, Vector(0, -2, 0), Vector(0.5, 1, -1));

struct Scene
{
	Object* objects[100];
	int numObj;
	LightSource lights[100];
	int numLights;
	Color pixels[screenWidth * screenHeight];

	Scene()
	{
		numObj = 0;
		numLights = 0;
	}

	~Scene()
	{
//        for(int i = 0; i<numObj; ++i)
//        {
//            delete objects[i];
//        }
	}

	void addObject(Object* o)
	{
		objects[numObj++] = o;
	}

	void addLight(const LightSource& l)
	{
		lights[numLights++] = l;
	}

	Intersection findClosestIntersect(const Ray& r)
	{
		float closest_dist = 0;
		Intersection theClosest;

		for (int i = 0; i < numObj; ++i)
		{
			Intersection intr = objects[i]->intersect(r);
			if (!intr.valid)
			{
				continue;
			}
			float dst = (intr.p0 - r.p0).Length();
			if (dst < closest_dist || theClosest.objectIndex == -1)
			{
				intr.objectIndex = i;
				theClosest = intr;
				closest_dist = dst;
			}
		}

		return theClosest;

	}

	Color trace(const Ray& r, int iter)
	{
		Color retColor;
		if (iter < DMAX)
		{
			Intersection i = findClosestIntersect(r);
			if (i.objectIndex != -1)
			{
				return objects[i.objectIndex]->anyag->getColor(i, lights,
						numLights);
			}
			else
			{
				for (int i = 0; i < numLights; ++i)
				{
					if (lights[i].type == LightSource::AMBIENS)
					{
						retColor += lights[i].color;
					}
				}
			}
			iter++;
		}
		else
		{
			for (int i = 0; i < numLights; ++i)
			{
				if (lights[i].type == LightSource::AMBIENS)
				{
					retColor += lights[i].color;
				}
			}
		}
		return retColor;
	}

	void draw()
	{
		glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, pixels);
	}
} scene;

struct Camera
{
	Vector eye;
	Vector lookat;
	Vector up;
	Vector right;

	Camera(Vector theEye, Vector look, Vector u)
	{
		eye = theEye;
		lookat = look;
		up = u;
		right = lookat % up;
	}

	void takePicture()
	{
		for (int x = 0; x < screenWidth; ++x)
		{
			for (int y = 0; y < screenHeight; ++y)
			{
				getPixel(x, y);
			}
		}
	}

	void getPixel(int x, int y)
	{
		Vector p = lookat + (float) ((2 * x) / (screenWidth - 1)) * right
				+ (float) ((2 * y) / (screenHeight - 1)) * up;
		Ray r(eye, p);
		scene.pixels[x + y * screenWidth] = scene.trace(r, 0);
	}
} camera(Vector(-3, -2, 1), Vector(1, 1, -1), Vector(0, 1, 0));	//Vector(3,2,-1), Vector(0.5,1,1), Vector(0,1,0));

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization()
{
	glViewport(0, 0, screenWidth, screenHeight);

	LightSource ambient(LightSource::AMBIENS);

	LightSource lightP1(LightSource::PONT, Color(0.0f, 1.0f, 0.0f),
			Vector(0, 3, 0), Vector(0.5, 1, -1));

	scene.addObject(diffuseFloor);
	scene.addLight(ambient);
	scene.addLight(lightP1);
	camera.takePicture();

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, scene.pixels);

	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y)
{
	if (key == 'd')
		glutPostRedisplay(); 		// d beture rajzold ujra a kepet
	else if (key == 'q')
	{
		diffuseFloor->p0.z += 1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'w')
	{
		diffuseFloor->p0.z -= 1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'k')
	{
		a = true;
		scene.lights[scene.numLights - 1].dv.x -= 0.5;
		std::cout << "X: " << scene.lights[scene.numLights - 1].dv.x
				<< std::endl;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'l')
	{
		a = true;
		scene.lights[scene.numLights - 1].dv.x += 0.5;
		std::cout << "X: " << scene.lights[1].dv.x << std::endl;
		camera.takePicture();
		glutPostRedisplay();
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
	static bool first = true;
	if (first)
	{
		glutPostRedisplay();
		first = false;
	}

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
