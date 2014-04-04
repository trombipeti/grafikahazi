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

double epsilon = 0.001;
int DMAX = 6;

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

	Color operator-=(const Color& c)
	{
		return *this = *this - c;
	}

	Color saturated() const
	{
		return Color(MAX(MIN(r, 1.0f), 0.0f), MAX(MIN(g, 1.0f), 0.0f),
				MAX(MIN(b, 1.0f), 0.0f));
	}

	bool operator==(const Color& c) const
	{
		return (r == c.r && g == c.g && b == c.b);
	}

	bool operator<(const Color& c)
	{
		return (r < c.r && g < c.g && b < c.b);
	}

	bool operator>(const Color& c)
	{
		return (r > c.r && g > c.g && b > c.b);
	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

//Vector normals[100000][2];
//int normal_nums = 0;

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

	Ray(Vector _p0 = Vector(0, 0, 0), Vector _dv = Vector(0, 0 - 1)) :
			p0(_p0), dv(_dv.norm())
	{
	}

	Ray(const Ray& r)
	{
		p0 = r.p0;
		dv = r.dv;
	}
};

struct Intersection
{
	Ray r;
	Vector p0;
	Vector nv;
	bool valid;
	int objectIndex;
	Intersection(Ray _r = Ray(), Vector p = Vector(0, 0, 0),
			Vector n = Vector(0, 0, 0), bool _valid = false, int objInd = -1)
	{
		r = _r;
		p0 = p;
		nv = n;
		valid = _valid;
		objectIndex = objInd;
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
			int numLights, int recursion_level = 0) = 0;
	virtual ~Anyag()
	{
	}
	;
};

struct Object
{
	Anyag *anyag;
	virtual Intersection intersect(const Ray& ray) = 0;
	virtual Vector normal(const Vector& p) = 0;
	virtual ~Object()
	{
	}
	;
};

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
		for (int i = 0; i < numObj; ++i)
		{
			delete objects[i];
		}
	}

	void addObject(Object* o)
	{
		objects[numObj++] = o;
	}

	void addLight(const LightSource& l)
	{
		lights[numLights++] = l;
	}

	Intersection intersectAll(const Ray& r)
	{
		float closest_dist = 0;
		Intersection theClosest(r);

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
			Intersection i = intersectAll(r);
			if (i.objectIndex != -1)
			{
				retColor += objects[i.objectIndex]->anyag->getColor(i, lights,
						numLights, iter);
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
		return retColor.saturated();
	}

	void draw()
	{
		glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, pixels);
	}
} scene;

struct DiffuzAnyag: public Anyag
{
	Color kd;
	DiffuzAnyag(const Color& k)
	{
		kd = k;
	}

	Color getColor(Intersection itrs, const LightSource* lights, int numLights,
			int recursion_level)
	{
		Color retColor;

		for (int i = 0; i < numLights; ++i)
		{
			switch (lights[i].type)
			{
			case LightSource::AMBIENS:
				retColor += kd * lights[i].color;
				break;
			case LightSource::PONT:
			{
				Vector d = (lights[i].p0 - itrs.p0).norm();
				Ray shadowRay(itrs.p0 + (d * epsilon), d);
				Intersection shadowHit = scene.intersectAll(shadowRay);
				if (!shadowHit.valid
						|| (shadowRay.p0 - shadowHit.p0).Length()
								> (shadowRay.p0 - lights[i].p0).Length())
				{
//					Vector V = (-1.0f) * itrs.r.dv.norm();
					Vector Ll = shadowRay.dv.norm();
//					Vector Hl = (V + Ll).norm();
					float cost = Ll * itrs.nv.norm();
					retColor = retColor
							+ (lights[i].color
									* (1 / (itrs.p0 - lights[i].p0).Length())
									* kd * MAX(cost, 0.0f));
				}
			}
				break;
			default:
				break;
			}
		}
		return retColor.saturated();
	}
} Zold(Color(0.1, 0.4, 0.05)), Feher(Color(1, 0.7, 1));

struct ReflektivAnyag: public Anyag
{
	Color F0;

	ReflektivAnyag(Color n, Color k)
	{
		F0 = ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
	}

	Color F(float costheta)
	{
		return F0 + (Color(1, 1, 1) - F0) * pow(1 - costheta, 5);
	}

	Color getColor(Intersection inter, const LightSource* lights, int numLights,
			int recursion_level)
	{
		Ray reflected_ray;
		reflected_ray.dv = inter.r.dv
				- 2.0f * inter.nv * (inter.nv * inter.r.dv);
		reflected_ray.p0 = inter.p0 + (inter.r.dv * epsilon );
		Color traced = (scene.trace(reflected_ray, recursion_level + 1));
//		traced = Color(0.5,0.5,0.5);
		Color f = F((-1) * (inter.r.dv * inter.nv));
		return (f * traced).saturated();
	}
} Arany(Color(0.17, 0.35, 1.5), Color(3.1, 2.7, 1.9)),

Ezust(Color(0.14, 0.16, 0.13), Color(4.1, 2.3, 3.1));

struct Egyenes
{
	Vector p0;
	Vector dv;

	Egyenes(Vector p = Vector(0, 0, 0), Vector d = Vector(0, 0, -1))
	{
		p0 = p;
		dv = d.norm();
	}
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

	Vector normal(const Vector& p)
	{
		Vector n = 2.0f
				* Vector((p.x - p1.x) + (p.x - p2.x),
						(p.y - p1.y) + (p.y - p2.y),
						(p.z - p1.z) + (p.z - p2.z));
		return n.norm();
	}

	Intersection intersect(const Ray& ray)
	{
		float dvx = ray.dv.x;
		float dvy = ray.dv.y;
		float dvz = ray.dv.z;
		float p0x = ray.p0.x;
		float p0y = ray.p0.y;
		float p0z = ray.p0.z;
		float x0 = p1.x;
		float y0 = p1.y;
		float z0 = p1.z;
		float x1 = p2.x;
		float y1 = p2.y;
		float z1 = p2.z;
		// (x-x0)^2 + (y-y0)^2 + (z-z0)^2 + (x-x1)^2 + (y-y1)^2 + (z-z1)^2 - c = 0
		float a = 2.0f * (dvx * dvx + dvy * dvy + dvz * dvz);
		float b = 2 * dvx * (p0x - x0) + 2 * dvy * (p0y - y0)
				+ 2 * dvz * (p0z - z0)
				+ (2 * dvx * (p0x - x1) + 2 * dvy * (p0y - y1)
						+ 2 * dvz * (p0z - z1));
		float _c = x0 * x0 + y0 * y0 + z0 * z0 + p0x * p0x + p0y * p0y
				+ p0z * p0z - 2 * (x0 * p0x + y0 * p0y + z0 * p0z) + x1 * x1
				+ y1 * y1 + z1 * z1 + p0x * p0x + p0y * p0y + p0z * p0z
				- 2 * (x1 * p0x + y1 * p0y + z1 * p0z) - c;
		float d = b * b - 4 * a * _c;
		if (d < 0)
		{
			return Intersection();
		}
		float t = -1;
		float t1 = ((-1.0 * b + sqrt(d)) / (2.0 * a));
		float t2 = ((-1.0 * b - sqrt(d)) / (2.0 * a));
		if ((t2 > t1 || t2 < epsilon) && t1 > epsilon)
			t = t1;
		if ((t1 > t2 || t1 < epsilon) && t2 > epsilon)
			t = t2;

		if (t > epsilon)
		{
			Vector n = normal(ray.dv * t + ray.p0);
			float rayd = pow((p1 - ray.p0).Length(), 2)
					+ pow((p2 - ray.p0).Length(), 2);
			if (rayd < c)
			{
//				n = n * (-1.0f);
			}
			return Intersection(ray, ray.p0 + t * ray.dv, n, true);
		}
		else if (t == 0)
		{
			Vector n = normal(ray.dv * t + ray.p0);
			return Intersection(ray, ray.p0, n, true);
		}
		else
		{
			return Intersection();
		}
	}
};

struct Czermanik: public Object
{
	Egyenes e1;
	Egyenes e2;
	double c;

	Czermanik(Anyag* a, Egyenes _e1, Egyenes _e2, double _c)
	{
		e1 = _e1;
		e2 = _e2;
		c = _c;
		anyag = a;
	}

	Vector normal(const Vector& p)
	{
		return (p - e1.p0).norm();
	}

	Intersection intersect(const Ray& ray)
	{

		return Intersection();
	}
};
//Czermanik *metszoCzermanik = new Czermanik(&Uveg,
//		Egyenes(Vector(1, 1, -2), Vector(0, 1, 0)),
//		Egyenes(Vector(1, 1, -2), Vector(0.5, 0.5, -1)), 4.0);

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

	Vector normal(const Vector& p)
	{
		return nv;
	}

	Intersection intersect(const Ray& r)
	{
		float r_t = (p0 - r.p0) * nv / (r.dv * nv);
		if (!isnan(r_t) && r_t >= 0)
		{
			Vector metszes = r.p0 + r_t * r.dv;
			return Intersection(r, metszes, nv, true);
		}
		return Intersection();
	}
	~Floor()
	{

	}
};

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
		up = u.norm();
		right = ((lookat - eye) % up).norm();
	}

	void takePicture()
	{
#pragma omp parallel for
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
		Vector p = lookat + (float) ((2.0f * x) / (screenWidth - 1.0f)) * right
				+ (float) ((2.0f * y) / (screenHeight - 1.0f)) * up;
		Ray r(eye, (p - eye).norm());
		Color c = scene.trace(r, 0);
		scene.pixels[x + y * screenWidth] = c;
	}
};

Floor *diffuseFloor = new Floor(&Zold, Vector(0, -1, 0), Vector(0, 1, 0));

Uljanov *kisUljanov = new Uljanov(&Ezust, Vector(1, 0, -32), Vector(1, 0, -32),
		1);

Uljanov *nagyUljanov = new Uljanov(&Arany, Vector(0, 0, -35), Vector(0, 0, -35),
		100000);

Camera camera(Vector(0, 1, 10), Vector(-1, 0, -1), Vector(0, 1, 0));

LightSource ambient(LightSource::AMBIENS, Color(0.2f, 0.2f, 0.2f));

LightSource lightP1(LightSource::PONT, Color(10.0f, 10.0f, 10.0f),
		Vector(1.4, 1, -25), Vector(0, -1, -0.2));

LightSource lightP2(LightSource::PONT, Color(10.0f, 10.0f, 10.0f),
		Vector(1.4, 1, -50), Vector(0, -1, -0.2));

LightSource lightP3(LightSource::PONT, Color(50.0f, 50.0f, 50.0f),
		Vector(5, 5, 11), Vector(0, 2, 0));

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization()
{
	glViewport(0, 0, screenWidth, screenHeight);

	scene.addObject(diffuseFloor);
//	scene.addObject(kisUljanov);
	scene.addObject(nagyUljanov);
	scene.addLight(ambient);
	scene.addLight(lightP1);
	scene.addLight(lightP2);
	scene.addLight(lightP3);
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
	static bool up = true;
	if (key == 'd')
		glutPostRedisplay(); 		// d beture rajzold ujra a kepet
	else if (key == 'u')
	{
		scene.lights[0].color += Color(0.1, 0.1, 0.1);
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'z')
	{
		scene.lights[0].color -= Color(0.1, 0.1, 0.1);
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'l')
	{
		if (up)
		{
			scene.lights[scene.numLights - 1].color = Color(0, 0, 0);
			up = false;
		}
		else
		{
			scene.lights[scene.numLights - 1].color = Color(1, 1, 1);
			up = true;
		}
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'f')
	{
		diffuseFloor->p0.z -= 0.5;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'g')
	{
		diffuseFloor->p0.z += 0.5;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'a')
	{
		scene.lights[scene.numLights - 1].p0.x -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 's')
	{
		scene.lights[scene.numLights - 1].p0.x += 0.1;
		//      std::cout << scene.lights[scene.numLights - 1].p0.x << std::endl;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'c')
	{
		camera.eye.z -= 1;
		camera.right = camera.lookat % camera.up;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'x')
	{
		camera.eye.z += 1;
		camera.right = camera.lookat % camera.up;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '1')
	{
		scene.lights[1].color =
				scene.lights[1].color.r > 0 ?
						Color(0, 0, 0) : Color(10, 10, 10);
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '2')
	{
		scene.lights[2].color =
				scene.lights[2].color.r > 0 ?
						Color(0, 0, 0) : Color(10, 10, 10);
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '3')
	{
		scene.lights[3].color =
				scene.lights[3].color.r > 0 ?
						Color(0, 0, 0) : Color(20, 20, 20);
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'n')
	{
		kisUljanov->p1.x -= 0.1;
		kisUljanov->p2.x += 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'm')
	{
		kisUljanov->p1.x += 0.1;
		kisUljanov->p2.x -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == ',')
	{
		kisUljanov->p1.y -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '.')
	{
		kisUljanov->p1.y += 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '-')
	{
		kisUljanov->p1.z -= 0.1;
		kisUljanov->p2.z += 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == '+')
	{
		kisUljanov->p1.z += 0.1;
		kisUljanov->p2.z -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'r')
	{
		nagyUljanov->c *= 2.0f;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'e')
	{
		nagyUljanov->c *= 0.5f;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'q')
	{
		kisUljanov->c *= 2.0f;
		camera.takePicture();
		glutPostRedisplay();
	}
	else if (key == 'w')
	{
		kisUljanov->c *= 0.5f;
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
		glutPostRedisplay(); // Ilyenkor rajzold ujra a kepet
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
//	long time = glutGet(GLUT_ELAPSED_TIME);
//	if ((time % 1000) / 100 >= 5)
//	{
//		kisUljanov->p1.x = ((time % 100) / 50.0f);
//		std::cout << kisUljanov->p1.x << std::endl;
//		camera.takePicture();
//		glutPostRedisplay();
//	}

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

	glMatrixMode(GL_MODELVIEW); // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION); // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization(); 			// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay); 				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop(); 				// Esemenykezelo hurok

	return 0;
}
