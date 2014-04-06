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

float epsilon = 0.0000001;
float dv_szorzo = 0.001;
int DMAX = 10;

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
		r = g = b = 0.03;
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

	// http://filmicgames.com/archives/75
	Color toneMapped() const
	{
		float y = 0.21f * r + 0.72f * g + 0.07f * b;
		float y_ = y / (y + 1);
		return (*this * (y_ / y)).saturated();
	}

	bool operator==(const Color& c) const
	{
		return (r == c.r && g == c.g && b == c.b);
	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

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

	bool in_air;

	Ray(Vector _p0 = Vector(0, 0, 0), Vector _dv = Vector(0, 0 - 1),
			bool inair = true)
	{
		p0 = _p0;
		dv = _dv.norm();
		in_air = inair;
	}

	Ray(const Ray& r)
	{
		p0 = r.p0;
		dv = r.dv;
		in_air = r.in_air;
	}
};

// Az alabbi struktura felepitese reszben innen szarmazik:
// https://wiki.sch.bme.hu/Sz%C3%A1m%C3%ADt%C3%B3g%C3%A9pes_grafika_h%C3%A1zi_feladat_tutorial#Hogyan_k.C3.B6vess.C3.BCk_a_sugarakat.3F
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
		nv = n.norm();
		valid = _valid;
		objectIndex = objInd;
	}
};

struct LightSource
{

	Color color;
	Color intensity;
	Vector p0;
	Vector dv;

	LightSource(Color c = Color(1, 1, 1),
			Color intens = Color(1.0f, 1.0f, 1.0f),
			Vector _p0 = Vector(0, 0, 0), Vector _dv = Vector(0, 0, -1))
	{
		color = c;
		intensity = intens;
		p0 = _p0;
		dv = _dv.norm();
	}

	Color getColor(Vector at) const
	{
		Color ret = color * intensity;
		ret = ret / (p0 - at).Length();
		return ret;

	}
};

struct Material
{
	virtual Color getColor(Intersection inter, const LightSource* lights,
			int numLights, int recursion_level = 0) = 0;
	virtual ~Material()
	{
	}
	;
};

struct Object
{
	Material *material;
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
				retColor += objects[i.objectIndex]->material->getColor(i,
						lights, numLights, iter);
			}
		}
		return retColor.saturated();
	}

	void render()
	{
		glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, pixels);
	}
} scene;

struct DiffuseMaterial: public Material
{
	Color kd;
	DiffuseMaterial(const Color& k)
	{
		kd = k;
	}

	Color getColor(Intersection inter, const LightSource* lights, int numLights,
			int recursion_level)
	{
		Color retColor;

		for (int i = 0; i < numLights; ++i)
		{
			Vector d = (lights[i].p0 - inter.p0).norm();
			Ray shadowRay(inter.p0 + (d * dv_szorzo), d);
			Intersection shadowHit = scene.intersectAll(shadowRay);
			if (!shadowHit.valid
					|| (shadowRay.p0 - shadowHit.p0).Length()
							> (shadowRay.p0 - lights[i].p0).Length())
			{
//					Vector V = (-1.0f) * itrs.r.dv.norm();
				Vector Ll = shadowRay.dv.norm();
//					Vector Hl = (V + Ll).norm();
				float cost = Ll * inter.nv.norm();
				retColor = retColor
						+ (lights[i].getColor(inter.p0) * kd * MAX(cost, 0.0f));
			}

		}
		return retColor.saturated();
	}
};

struct ReflectiveMaterial: public Material
{
	Color F0;

	ReflectiveMaterial(Color n, Color k)
	{
		F0 = ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
	}

	Color F(float costheta)
	{
		return F0 + (Color(1, 1, 1) - F0) * pow(1 - costheta, 5);
	}

	Ray reflect(Intersection inter)
	{
		Ray reflected;
		reflected.dv = inter.r.dv - 2.0f * inter.nv * (inter.nv * inter.r.dv);
		reflected.p0 = inter.p0 + reflected.dv.norm() * dv_szorzo;
		return reflected;
	}

	Color getColor(Intersection inter, const LightSource* lights, int numLights,
			int recursion_level)
	{
		if (inter.r.dv * inter.nv > 0.0f)
		{
			inter.nv = (-1.0f) * inter.nv;
		}
		Ray reflected_ray = reflect(inter);
		Color traced = (scene.trace(reflected_ray, recursion_level + 1));
		Color f = F((-1) * (inter.r.dv * inter.nv));
		return (f * traced).saturated();
	}
};

struct RefractiveMaterial: public ReflectiveMaterial
{
	float n;
	float k;

	RefractiveMaterial(float _n, float _k) :
			ReflectiveMaterial(Color(_n, _n, _n), Color(_k, _k, _k))
	{
		n = _n;
		k = _k;
	}

	bool refract(Intersection inter, Ray *ret)
	{
		float cn = (inter.r.in_air ? n : 1.0f / n);
		float cosa = inter.nv * inter.r.dv;
		float discrim = 1 - ((1 - cosa * cosa) / (n * n));
		if (discrim < 0)
		{
			return false;
		}
		ret->dv =
				(inter.r.dv / cn + inter.nv * (cosa / cn - sqrt(discrim))).norm();
		return true;

	}

	Color getColor(Intersection inter, const LightSource* lights, int numLights,
			int recursion_level)
	{
		if (inter.r.dv * inter.nv > 0.0f)
		{
			inter.nv = (-1.0f) * inter.nv;
		}
		Ray reflected_ray = reflect(inter);
		Color traced = (scene.trace(reflected_ray, recursion_level + 1));
		Color reflected_color = (F((-1) * (inter.r.dv * inter.nv))) * traced;
		Color refracted_color;
		Ray refracted_ray;
		if (refract(inter, &refracted_ray))
		{
			refracted_ray.p0 = inter.p0 + refracted_ray.dv * dv_szorzo;
			Color traced = scene.trace(refracted_ray, recursion_level + 1);
			refracted_color = (Color(1, 1, 1)
					- F((-1) * (inter.r.dv * inter.nv))) * traced;
		}
		return (refracted_color + reflected_color).saturated();
	}
};

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

	Uljanov(Material* a, Vector _p1, Vector _p2, double _c)
	{
		p1 = _p1;
		p2 = _p2;
		c = _c;
		material = a;
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
			return Intersection(ray, ray.p0 + t * ray.dv, n, true);
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
	double k;

	Czermanik(Material* a, Egyenes _e1, Egyenes _e2, double _c)
	{
		e1 = _e1;
		e2 = _e2;
		k = _c;
		material = a;
	}

	Vector normal(const Vector& p)
	{
		float l1 = e1.dv.x;
		float m1 = e1.dv.y;
		float n1 = e1.dv.z;

		float l2 = e2.dv.x;
		float m2 = e2.dv.y;
		float n2 = e2.dv.z;

		float x1 = e1.p0.x;
		float y1 = e1.p0.y;
		float z1 = e1.p0.z;

		float x2 = e2.p0.x;
		float y2 = e2.p0.y;
		float z2 = e2.p0.z;

		float x = p.x;
		float y = p.y;
		float z = p.z;

		float nx = 2 * m1 * ((x - x1) - l1 - y + y1)
				+ 2 * n1 * (l1 * (z1 - z) + n1 * (x - x1))
				+ 2 * m2 * ((x - x2) - l2 - y + y2)
				+ 2 * n2 * (l2 * (z2 - z) + n2 * (x - x2));

		float ny = 2
				* (l1 + m1 * (n1 * z - n1 * z1 - x + x1)
						+ (n2 * n2 + 2) * (y - y2))
				+ 2
						* (l2 + m2 * (n2 * z - n2 * z2 - x + x2)
								+ (n2 * n2 + 2) * (y - y2));

		float nz = 2 * l1 * (l1 * (z - z1) + n1 * (x1 - x))
				+ 2 * m2 * (m2 * (z - z2) + n2 * (y - y2))
				+ 2 * l2 * (l2 * (z - z2) + n2 * (x2 - x))
				+ 2 * m2 * (m2 * (z - z2) + n2 * (y - y2));

//		return (p - e1.p0).norm();

		return Vector(nx, ny, nz).norm();
	}

	Intersection intersect(const Ray& ray)
	{
		// Pont és egyenes távolsága képlet, Bronstejn-Szemangyejev Matematikai Zsebkönyv
		// 5. kiadás, Műszaki könyvkiadó Budapest, 1982. 280. oldal
		float l1 = e1.dv.x;
		float m1 = e1.dv.y;
		float n1 = e1.dv.z;

		float l2 = e2.dv.x;
		float m2 = e2.dv.y;
		float n2 = e2.dv.z;

		float d = ray.dv.x;
		float e = ray.dv.y;
		float f = ray.dv.z;

		float p = ray.p0.x;
		float q = ray.p0.y;
		float r = ray.p0.z;

		float x1 = e1.p0.x;
		float y1 = e1.p0.y;
		float z1 = e1.p0.z;

		float x2 = e2.p0.x;
		float y2 = e2.p0.y;
		float z2 = e2.p0.z;
		float szorzo1 = 1 / (l1 * l1 + m1 * m1 + n1 * n1);
		float szorzo2 = 1 / (l2 * l2 + m2 * m2 + n2 * n2);

		// MAGIC STARTS HERE
		float a = (szorzo1
				* (d * d * m1 * m1 + d * d * n1 * n1 - 2.0f * d * f * l1 * n1
						- 2.0f * e * d * l1 * m1 + f * f * l1 * l1
						+ f * f * m1 * m1 - 2.0f * e * f * m1 * n1
						+ e * e * l1 * l1 + e * e * n1 * n1))
				+ (szorzo2
						* (d * d * m2 * m2 + d * d * n2 * n2
								- 2.0f * d * f * l2 * n2
								- 2.0f * e * d * l2 * m2 + f * f * l2 * l2
								+ f * f * m2 * m2 - 2.0f * e * f * m2 * n2
								+ e * e * l2 * l2 + e * e * n2 * n2));

		float b = (szorzo1 * 2.0f
				* (d * l1 * m1 * (y1 - q) - d * l1 * n1 * r + d * l1 * n1 * z1
						+ (d * m1 * m1 + d * n1 * n1) * (p - x1)
						+ f * l1 * l1 * (r - z1) - f * l1 * n1 * p
						+ f * l1 * n1 * x1 + f * m1 * m1 * z1 - f * m1 * n1 * q
						+ f * m1 * n1 * y1 + e * l1 * l1 * (q - y1)
						- e * l1 * m1 * p - e * m1 * n1 * r + e * m1 * n1 * z1
						+ e * n1 * n1 * q - e * n1 * n1 * y1))
				+ (szorzo2 * 2.0f
						* (d * l2 * m2 * (y2 - q) - d * l2 * n2 * r
								+ d * l2 * n2 * z2
								+ (d * m2 * m2 + d * n2 * n2) * (p - x2)
								+ f * l2 * l2 * (r - z2) - f * l2 * n2 * p
								+ f * l2 * n2 * x2 + f * m2 * m2 * z2
								- f * m2 * n2 * q + f * m2 * n2 * y2
								+ e * l2 * l2 * (q - y2) - e * l2 * m2 * p
								- e * m2 * n2 * r + e * m2 * n2 * z2
								+ e * n2 * n2 * q - e * n2 * n2 * y2));

		float c = szorzo1
				* (l1 * l1 * q * q - 2 * l1 * l1 * q * y1 + l1 * l1 * r * r
						- 2 * l1 * l1 * r * z1 + l1 * l1 * y1 * y1
						+ l1 * l1 * z1 * z1 - 2 * l1 * m1 * p * (q + y1)
						+ 2 * l1 * m1 * q * x1 - 2 * l1 * m1 * x1 * y1
						- 2 * l1 * n1 * p * r + 2 * l1 * n1 * p * z1 + 2 * l1
						- n1 * r * x1 - 2 * l1 * n1 * x1 * z1 + m1 * m1 * p * p
						- 2 * m1 * m1 * p * x1 + m1 * m1 * r * r
						- 2 * m1 * m1 * r * z1 + m1 - m1 * x1 * x1
						+ m1 * m1 * z1 * z1 - 2 * m1 * n1 * q * r
						+ 2 * m1 * n1 * q * z1 + 2 * m1 * n1 * r * y1
						- 2 * m1 * n1 * y1 * z1 + n1 * n1 * p * p
						- 2 * n1 * n1 * p * x1 + n1 * n1 * q * q
						- 2 * n1 * n1 * q * y1 + n1 * n1 * x1 * x1
						+ n1 * n1 * y1 * y1 - k)
				+ szorzo2
						* (l2 * l2 * q * q - 2 * l2 * l2 * q * y2
								+ l2 * l2 * r * r - 2 * l2 * l2 * r * z2
								+ l2 * l2 * y2 * y2 + l2 * l2 * z2 * z2
								- 2 * l2 * m2 * p * (q + y2)
								+ 2 * l2 * m2 * q * x2 - 2 * l2 * m2 * x2 * y2
								- 2 * l2 * n2 * p * r + 2 * l2 * n2 * p * z2
								+ 2 * l2 - n2 * r * x2 - 2 * l2 * n2 * x2 * z2
								+ m2 * m2 * p * p - 2 * m2 * m2 * p * x2
								+ m2 * m2 * r * r - 2 * m2 * m2 * r * z2 + m2
								- m2 * x2 * x2 + m2 * m2 * z2 * z2
								- 2 * m2 * n2 * q * r + 2 * m2 * n2 * q * z2
								+ 2 * m2 * n2 * r * y2 - 2 * m2 * n2 * y2 * z2
								+ n2 * n2 * p * p - 2 * n2 * n2 * p * x2
								+ n2 * n2 * q * q - 2 * n2 * n2 * q * y2
								+ n2 * n2 * x2 * x2 + n2 * n2 * y2 * y2 - k);
		// MAGIC ENDS HERE

		float discrmin = b * b - 4 * a * c;

		if (discrmin < 0)
		{
			return Intersection();
		}
		float t = -1;
		float t1 = ((-1.0 * b + sqrt(discrmin)) / (2.0 * a));
		float t2 = ((-1.0 * b - sqrt(discrmin)) / (2.0 * a));
		if ((t2 > t1 || t2 < epsilon) && t1 > epsilon)
			t = t1;
		if ((t1 > t2 || t1 < epsilon) && t2 > epsilon)
			t = t2;

		if (t > epsilon)
		{
			Vector n = normal(ray.dv * t + ray.p0);
			return Intersection(ray, ray.p0 + t * ray.dv, n, true);
		}
		else
		{
			return Intersection();
		}

		return Intersection();
	}
};

struct Floor: public Object
{
	Vector p0;
	Vector nv;
	Floor(Material *a = NULL, Vector p = Vector(0, 0, 0),
			Vector n = Vector(0, 1, 0))
	{
		material = a;
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
		Vector p = lookat
				+ (float) ((2.0f * (x + 0.5f)) / (screenWidth - 1.0f)) * right
				+ (float) ((2.0f * (y + 0.5f)) / (screenHeight - 1.0f)) * up;
		Ray r(eye, (p - eye).norm());
		Color c = scene.trace(r, 0);
		scene.pixels[x + y * screenWidth] = c;
	}
};

ReflectiveMaterial Arany(Color(0.17, 0.35, 1.5), Color(3.1, 2.7, 1.9));

ReflectiveMaterial Ezust(Color(0.14, 0.16, 0.13), Color(4.1, 2.3, 3.1));

RefractiveMaterial Uveg(1.5, 0);

DiffuseMaterial Zold(Color(0.1, 0.4, 0.05));
DiffuseMaterial VilagosZold(Color(0.1, 0.15, 0.1));
DiffuseMaterial Feher(Color(0.8, 0.8, 0.8));
DiffuseMaterial Fekete(Color(0.1, 0.1, 0.1));

Floor *diffuseFloor = new Floor(&Zold, Vector(0, 0.5, 0), Vector(0, 1, 0));

Uljanov *kisUljanov = new Uljanov(&Ezust, Vector(0.2, 0.9, 0),
		Vector(0.3, 1.1, 0), 0.4);

Uljanov *uvegUljanov = new Uljanov(&Uveg, Vector(-0.05, 0.72, 1.3),
		Vector(-0.05, 0.72, 1.3), 0.1);

Uljanov *nagyUljanov = new Uljanov(&Arany, Vector(-1, 4, 60), Vector(-1, 4, 60),
		7800);

Czermanik *metszoCzermanik = new Czermanik(&Uveg,
		Egyenes(Vector(0, 1, 0), Vector(0, -1, 1)),
		Egyenes(Vector(0, 1, 2), Vector(-1, 0, 0)), 1);

Czermanik *kiteroCzermanik = new Czermanik(&Uveg,
		Egyenes(Vector(0, 1, -10), Vector(0, 1, 0)),
		Egyenes(Vector(0, 1, -30), Vector(-1, 0, 0)), 1);

Camera camera(Vector(-1, 5, 19), Vector(-1.2, 0.3, 1), Vector(0, 1, 0));

LightSource lightP1(Color(1.0f, 1.0f, 1.0f), Color(10, 10, 10),
		Vector(1.4, 1, 10), Vector(0, -1, -0.2));

LightSource lightP2(Color(1.0f, 1.0f, 1.0f), Color(10, 10, 10),
		Vector(-1.4, 1, 10), Vector(0.4, 1, 0.2));

LightSource lightP3(Color(1.0f, 1.0f, 1.0f), Color(40, 10, 50),
		Vector(1.4, 5, 10), Vector(0, 2, 0));

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization()
{
	glViewport(0, 0, screenWidth, screenHeight);

	scene.addObject(diffuseFloor);
	scene.addObject(kisUljanov);
//	scene.addObject(uvegUljanov);
	scene.addObject(metszoCzermanik);
//	scene.addObject(kiteroCzermanik);
	scene.addObject(nagyUljanov);
	scene.addLight(lightP1);
	scene.addLight(lightP2);
	scene.addLight(lightP3);
	camera.takePicture();

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

//	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, scene.pixels);
	scene.render();

	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y)
{
	static bool up = true;
	switch (key)
	{
	case 'l':
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
		break;
	case 'h':
		diffuseFloor->p0.y -= 0.5;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'g':
		diffuseFloor->p0.y += 0.5;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'w':
		camera.eye.y += 1;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 's':
		camera.eye.y -= 1;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'a':
		camera.eye.x -= 1;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'd':
		camera.eye.x += 1;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'c':
		camera.eye.z -= 10;
		camera.lookat.z -= 10;
		camera.lookat.y -= 0.5;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'x':
		camera.eye.z += 10;
		camera.lookat.z += 10;
		camera.lookat.y += 0.5;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '1':
		scene.lights[0].color =
				scene.lights[0].color.r > 0 ? Color(0, 0, 0) : Color(1, 1, 1);
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '2':
		scene.lights[1].color =
				scene.lights[1].color.r > 0 ? Color(0, 0, 0) : Color(1, 1, 1);
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '3':
		scene.lights[2].color =
				scene.lights[2].color.r > 0 ? Color(0, 0, 0) : Color(1, 1, 1);
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'n':
		kisUljanov->p1.x -= 0.1;
		kisUljanov->p2.x += 0.1;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'm':
		kisUljanov->p1.x += 0.1;
		kisUljanov->p2.x -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case ',':
		nagyUljanov->p1.y -= 1;
		nagyUljanov->p2.y -= 1;
		//std::cout << nagyUljanov->p2.y << std::endl;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '.':
		nagyUljanov->p1.y += 1;
		nagyUljanov->p2.y += 1;
		//std::cout << nagyUljanov->p2.y << std::endl;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '-':
		kisUljanov->p1.z -= 0.1;
		kisUljanov->p2.z += 0.1;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case '+':
		kisUljanov->p1.z += 0.1;
		kisUljanov->p2.z -= 0.1;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'r':
		nagyUljanov->c *= 2.0f;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'e':
		nagyUljanov->c *= 0.5f;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'q':
		kisUljanov->c *= 0.5f;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'o':
		dv_szorzo /= 10;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'p':
		dv_szorzo *= 10;
		camera.takePicture();
		glutPostRedisplay();
		break;
	case 'z':
		camera.lookat.z += 2;
		camera.right = ((camera.lookat - camera.eye) % camera.up).norm();
		camera.takePicture();
		glutPostRedisplay();
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
//	static int a = 0;
	if (first)
	{
		glutPostRedisplay();
		first = false;
	}
//	long time = glutGet(GLUT_ELAPSED_TIME);
//	if (a++ > 0)
//	{
//		a = 0;
//		uvegUljanov->p1.z = 1.3 - 5.0f * sin(time / 100000.0f);
//		uvegUljanov->p2.z = 1.3 - 5.0f * sin(time / 100000.0f);
//		uvegUljanov->p1.x = -0.05 + 0.5f * sin(time / 50000.0f);
//		uvegUljanov->p2.x = -0.05 + 0.5f * sin(time / 50000.0f);
//		kisUljanov->p1.y = 0.9 + 0.6 + sinf(time / 2500.0f);
//		kisUljanov->p2.y = 1.1 + 0.6 + sinf(time / 2500.0f);
//		camera.takePicture();
//		glutPostRedisplay();
//	}
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

	onInitialization(); 	// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay); 		// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop(); 				// Esemenykezelo hurok

	return 0;
}
