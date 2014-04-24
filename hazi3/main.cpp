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
bool draw_normals = false;

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

enum MATERIAL
{
	PIROS, ZOLD, KEK, EZUST, FEKETE
};

void setMaterial(MATERIAL m)
{
	switch (m)
	{
	case PIROS:
	{
		float a[4] =
		{ 1, 0, 0, 1 };
		float d[4] =
		{ 1, 0, 0, 1 };
		float sp[4] =
		{ 1, 1, 1, 1 };
		float sh = 100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
	case ZOLD:
	{
		float a[4] =
		{ 0, 1, 0, 1 };
		float d[4] =
		{ 0, 1, 0, 1 };
		float sp[4] =
		{ 1, 1, 1, 1 };
		float sh = 128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
	case KEK:
	{
		float a[4] =
		{ 0, 0, 1, 1 };
		float d[4] =
		{ 0, 0, 1, 1 };
		float sp[4] =
		{ 1, 1, 1, 1 };
		float sh = 100;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
		// http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
	case EZUST:
	{
		float a[4] =
		{ 0.23125f, 0.23125f, 0.23125f, 1.0f };
		float d[4] =
		{ 0.2775f, 0.2775f, 0.2775f, 1.0f };
		float sp[4] =
		{ 0.773911f, 0.773911f, 0.773911f, 1.0f };
		float sh = 89.6f;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
	case FEKETE:
	{
		float a[4] =
		{ 0, 0, 0, 1 };
		float d[4] =
		{ 0.01, 0.01, 0.01, 1 };
		float sp[4] =
		{ 0.2, 0.2, 0.2, 1 };
		float sh = 128;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
	default:
		break;
	}
}

struct Object
{
	virtual float x(float u, float v)
	{
		return 0;
	}

	virtual float y(float u, float v)
	{
		return 0;
	}

	virtual float z(float u, float v)
	{
		return 0;
	}

	virtual float xdu(float u, float v)
	{
		return 0;
	}

	virtual float ydu(float u, float v)
	{
		return 0;
	}

	virtual float zdu(float u, float v)
	{
		return 0;
	}

	virtual float xdv(float u, float v)
	{
		return 0;
	}

	virtual float ydv(float u, float v)
	{
		return 0;
	}

	virtual float zdv(float u, float v)
	{
		return 0;
	}

	virtual void draw() = 0;

	void doDraw(float u_step, float v_step)
	{
		for (float u = 0; u < 1.0f; u += u_step)
		{
			for (float v = 0; v < 1.0f; v += v_step)
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

				float x2 = x(MIN(u + u_step, 1.0f), v);
				float y2 = y(MIN(u + u_step, 1.0f), v);
				float z2 = z(MIN(u + u_step, 1.0f), v);
				float x2du = xdu(MIN(u + u_step, 1.0f), v);
				float y2du = ydu(MIN(u + u_step, 1.0f), v);
				float z2du = zdu(MIN(u + u_step, 1.0f), v);
				float x2dv = xdv(MIN(u + u_step, 1.0f), v);
				float y2dv = ydv(MIN(u + u_step, 1.0f), v);
				float z2dv = zdv(MIN(u + u_step, 1.0f), v);
				Vector n2 = Vector(x2dv, y2dv, z2dv) % Vector(x2du, y2du, z2du);

				float x3 = x(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float y3 = y(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float z3 = z(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float x3du = xdu(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float y3du = ydu(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float z3du = zdu(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float x3dv = xdv(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float y3dv = ydv(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				float z3dv = zdv(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				Vector n3 = Vector(x3dv, y3dv, z3dv) % Vector(x3du, y3du, z3du);

				float x4 = x(u, MIN(v + v_step, 1.0f));
				float y4 = y(u, MIN(v + v_step, 1.0f));
				float z4 = z(u, MIN(v + v_step, 1.0f));
				float x4du = xdu(u, MIN(v + v_step, 1.0f));
				float y4du = ydu(u, MIN(v + v_step, 1.0f));
				float z4du = zdu(u, MIN(v + v_step, 1.0f));
				float x4dv = xdv(u, MIN(v + v_step, 1.0f));
				float y4dv = ydv(u, MIN(v + v_step, 1.0f));
				float z4dv = zdv(u, MIN(v + v_step, 1.0f));
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

				if (draw_normals)
				{
					glBegin(GL_LINES);
					glVertex3f(x1, y1, z1);
					glVertex3f(x1 + 0.1f * n1.norm().x, y1 + 0.1f * n1.norm().y,
							z1 + 0.1f * n1.norm().z);
					glVertex3f(x2, y2, z2);
					glVertex3f(x2 + 0.1f * n2.norm().x, y2 + 0.1f * n2.norm().y,
							z2 + 0.1f * n2.norm().z);
					glVertex3f(x3, y3, z3);
					glVertex3f(x3 + 0.1f * n3.norm().x, y3 + 0.1f * n3.norm().y,
							z3 + 0.1f * n3.norm().z);
					glVertex3f(x4, y4, z4);
					glVertex3f(x4 + 0.1f * n4.norm().x, y4 + 0.1f * n4.norm().y,
							z4 + 0.1f * n4.norm().z);
					glEnd();
				}

			}
		}
	}
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
//		glRotatef(FOK, 0, 1, 0);
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
		glLightf(id, GL_QUADRATIC_ATTENUATION, 0.8f);
	}
};

struct Ellipszoid: public Object
{
	Vector p0;

	float a;
	float b;
	float c;

	int sides;
	int rings;

	Ellipszoid(Vector p, float _a, float _b, float _c, float numSides,
			float numRings)
	{
		p0 = p;
		a = _a;
		b = _b;
		c = _c;
		sides = numSides;
		rings = numRings;
	}

	float x(float u, float v)
	{
		return p0.x + a * sin(2 * M_PI * u) * cos(2 * M_PI * v);
	}

	float y(float u, float v)
	{
		return p0.y + b * sin(2 * M_PI * u) * sin(2 * M_PI * v);
	}

	float z(float u, float v)
	{
		return p0.z + c * cos(2 * M_PI * u);
	}

	float xdu(float u, float v)
	{
		return a * cos(2 * M_PI * u) * -cos(2 * M_PI * v);
	}

	float ydu(float u, float v)
	{
		return b * cos(2 * M_PI * u) * -sin(2 * M_PI * v);
	}

	float zdu(float u, float v)
	{
		return sin(2 * M_PI * u);
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
		float u_step = 1.0f / sides;
		float v_step = 1.0f / rings;

		doDraw(u_step, v_step);
	}
};

struct Paraboloid: public Object
{
	float h;
	float a;
	int rings;
	int sides;

	Paraboloid(float _a, float _h, int numRings, int numSides)
	{
		a = _a;
		h = _h;
		rings = numRings;
		sides = numSides;
	}

	float x(float u, float v)
	{
		return a * sqrt(u / h) * cos(2 * M_PI * v);
	}

	float y(float u, float v)
	{
		return a * sqrt(u / h) * sin(2 * M_PI * v);
	}

	float z(float u, float v)
	{
		return u;
	}

	float xdu(float u, float v)
	{
		return a * sqrt(u / h) * cos(2 * M_PI * v) / (2 * 2 * M_PI * u);
	}

	float ydu(float u, float v)
	{
		return a * sqrt(u / h) * sin(2 * M_PI * v) / (2 * 2 * M_PI * u);
	}

	float zdu(float u, float v)
	{
		return 1;
	}

	float xdv(float u, float v)
	{
		return (-1.0f) * a * sqrt(u / h) * sin(2 * M_PI * v);
	}

	float ydv(float u, float v)
	{
		return a * sqrt(u / h) * cos(2 * M_PI * v);
	}
	float zdv(float u, float v)
	{
		return 0;
	}

	void draw()
	{
		float u_step = 1.0f / sides;
		float v_step = (h / rings);

		doDraw(u_step, v_step);
	}
};

struct Henger: public Object
{
	float r;
	float h;
	int rings;
	int sides;

	Henger(float radius, float height, int numRings, int numSides)
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

		doDraw(u_step, v_step);

		for (float u = 0; u < 1.0f; u += u_step)
		{
			glBegin(GL_TRIANGLES);
			glNormal3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(x(u, 0), y(u, 0), z(u, 0));
			glVertex3f(x(MIN(u + u_step, 1.0f), 0), y(MIN(u + u_step, 1.0f), 0),
					z(u, 0));
			glEnd();

			glBegin(GL_TRIANGLES);
			glNormal3f(0, 0, -1);
			glVertex3f(0, 0, z(u, 1));
			glVertex3f(x(u, 1), y(u, 1), z(u, 1));
			glVertex3f(x(MIN(u + u_step, 1.0f), 1), y(MIN(u + u_step, 1.0f), 1),
					z(u, 1));
			glEnd();
		}
	}

};

struct UjHenger: public Object
{
	Vector p0;
	Vector p1;
	Vector d;
	Vector dnorm;
	float r;
	int sides;

	UjHenger(Vector p, Vector q, float rad, int numSides)
	{
		p0 = p;
		p1 = q;
		d = (p - q);
		dnorm = d.norm();
		r = rad;
		sides = numSides;
	}

	float x(float u, float v)
	{
		return p0.x + r * cos(2 * M_PI * u) + d.x * v;
	}

	float y(float u, float v)
	{
		return p0.y + r * sin(2 * M_PI * u) + d.y * v;
	}

	float z(float u, float v)
	{
		return p0.z;
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
		return d.x;
	}

	float ydv(float u, float v)
	{
		return d.y;
	}

	float zdv(float u, float v)
	{
		return d.z;
	}

	void draw()
	{
		float u_step = 1.0f / sides;
		float v_step = 0.5f;

		doDraw(u_step, v_step);
	}

};

struct Teglalap: public Object
{
	Vector p0;
	Vector p1;
	Vector p2;
	Vector p3;
	int res;

	Teglalap(Vector a, Vector b, Vector c, Vector d, int resolution)
	{
		p0 = a;
		p1 = b;
		p2 = c;
		p3 = d;
		res = resolution;
	}

	float x(float u, float v)
	{
		return p0.x + u * (p1 - p0).x + v * (p3 - p0).x;
	}

	float y(float u, float v)
	{
		return p0.y + u * (p1 - p0).y + v * (p3 - p0).y;
	}

	float z(float u, float v)
	{
		return p0.z + u * (p1 - p0).z + v * (p3 - p0).z;
	}

	float xdu(float u, float v)
	{
		return (p1 - p0).x;
	}

	float ydu(float u, float v)
	{
		return (p1 - p0).y;
	}

	float zdu(float u, float v)
	{
		return (p1 - p0).z;
	}

	float xdv(float u, float v)
	{
		return (p3 - p0).x;
	}

	float ydv(float u, float v)
	{
		return (p3 - p0).y;
	}

	float zdv(float u, float v)
	{
		return (p3 - p0).z;
	}

	void draw()
	{
		float u_step = 1.0f / res;
		float v_step = 1.0f / res;

		doDraw(u_step, v_step);
	}

};

struct Teglatest: public Object
{
	Vector p0;
	Vector right;
	Vector up;
	Vector back;
	float a;
	float b;
	float c;
	int res;

	Teglatest(Vector _p0, Vector r, Vector u, float wa, float wb, float wc,
			int resolution)
	{
		p0 = _p0;
		right = r.norm();
		up = u.norm();
		back = (up % right).norm();
		a = wa;
		b = wb;
		c = wc;
		res = resolution;
	}

	void draw()
	{
		Teglalap f(p0, p0 + up * a, p0 + up * a + right * b, p0 + right * b,
				res);
		Teglalap u(f.p1, f.p1 + back * c, f.p1 + back * c + right * b, f.p2,
				res);
		Teglalap r(f.p2, u.p2, f.p3 + back * c, f.p3, res);
		Teglalap d(f.p3, r.p2, f.p0 + back * c, f.p0, res);
		Teglalap l(f.p0, d.p2, u.p1, f.p1, res);
		Teglalap b(u.p1, l.p1, d.p1, r.p1, res);
		f.draw();
		u.draw();
		r.draw();
		d.draw();
		l.draw();
		b.draw();

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

		doDraw(u_step, v_step);
	}
};

struct Kerek: public Object
{
	Torus *gumi;
	Henger *kullok[100];
	Henger *kerekagy;
	int kulloszam;

	Kerek(float kerekR, float gumiR, int kulloSzam)
	{
		kulloszam = kulloSzam > 100 ? 100 : kulloSzam;
		gumi = new Torus(gumiR, kerekR, 20, 30);
		kerekagy = new Henger(0.8 * gumiR, gumiR, 10, 10);
		for (int i = 0; i < kulloszam; ++i)
		{
			kullok[i] = new Henger(0.02, kerekR, 10, 10);
		}
	}

	void draw()
	{
		setMaterial(EZUST);
		for (int i = 0; i < kulloszam; ++i)
		{
			glPushMatrix();
			glRotatef(90, 1, 0, 0);
			float rot = (360.0f / kulloszam) * i;
			glRotatef(rot, 0, 1, 0);
			kullok[i]->draw();
			glPopMatrix();
		}
		glPushMatrix();
		glTranslatef(0, 0, kerekagy->h * 0.5f);
		kerekagy->draw();
		glPopMatrix();
		setMaterial(FEKETE);
		gumi->draw();
	}

	~Kerek()
	{
		delete gumi;
		for (int i = 0; i < kulloszam; ++i)
		{
			delete kullok[i];
		}
	}
};

struct BringaVaz: public Object
{
	Henger *jobbfont;
	Henger *jobblent;
	Henger *kozep;
	Henger *balfont;
	Henger *ballent;
	Henger *pedalcso;

	float csoR;
	float h;
	float w;

	BringaVaz(float height, float width, float cso_radius)
	{
		csoR = cso_radius;
		h = height;
		w = width;
		jobbfont = new Henger(csoR, w * 0.6, 2, 10);
		kozep = new Henger(csoR, h, 2, 10);
		jobblent = new Henger(csoR, sqrt(pow(w * 0.6, 2) + pow(h, 2)), 2, 10);
		pedalcso = new Henger(csoR, csoR * 8, 2, 10);
		ballent = new Henger(csoR, w * 0.4, 10, 10);
		balfont = new Henger(csoR, sqrt(pow(w * 0.4, 2) + pow(h, 2)), 2, 10);
	}

	~BringaVaz()
	{
		delete jobbfont;
		delete jobblent;
		delete kozep;
		delete balfont;
		delete ballent;
		delete pedalcso;
	}

	void draw()
	{
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(w * -0.1f, 0, 0);
		kozep->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * -0.1f, h - csoR, 0);
		glRotatef(270, 0, 1, 0);
		jobbfont->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * -0.1f, 0, 0);
		glRotatef(270, 0, 1, 0);
		float c = acos(MIN(h / jobblent->h, jobblent->h / h));
		glRotatef(90 - c * (180.0 / M_PI), 1, 0, 0);
		jobblent->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * -0.1f, 0, 4 * csoR);
		pedalcso->draw();
		glTranslatef(0, h - csoR, 0);
		pedalcso->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * -0.1f, 0, 3 * csoR);
		glRotatef(90, 0, 1, 0);
		ballent->draw();
		glTranslatef(6 * csoR, 0, 0);
		ballent->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * -0.1f - ballent->h, 0, 3 * csoR);
		glRotatef(90, 0, 1, 0);
		c = atan(MIN(h / ballent->h, ballent->h / h));
		glRotatef(90 + c * (180.0 / M_PI), 1, 0, 0);
		balfont->draw();
		glTranslatef(6 * csoR, 0, 0);
		balfont->draw();
		glPopMatrix();
	}

};

struct Kormanymu: public Object
{
	Kerek *kerek;
	Henger *balvilla;
	Henger *jobbvilla;
	Henger *villateteje;
	Henger *villanyak;
	Henger *kormany;

	Kormanymu(float h, float kerekR, float gumiR, int kullok)
	{
		kerek = new Kerek(kerekR, gumiR, kullok);
		balvilla = new Henger(gumiR, h * 0.7f, 5, 10);
		jobbvilla = new Henger(gumiR, h * 0.7f, 5, 10);
		villateteje = new Henger(gumiR, kerek->gumi->inR * 4.0f + 2 * gumiR, 5,
				10);
		villanyak = new Henger(gumiR, h * 0.25f, 5, 10);
		kormany = new Henger(gumiR, h * 0.7f, 5, 10);
	}

	~Kormanymu()
	{
		delete kerek;
		delete balvilla;
		delete jobbvilla;
		delete villateteje;
		delete villanyak;
		delete kormany;
	}

	void draw()
	{
		glPushMatrix();
//		glRotatef(FOK * -1, 0, 0, 1);
		kerek->draw();
		glPopMatrix();

		setMaterial(PIROS);
		glPushMatrix();
		glTranslatef(0, (-1.0f) * kerek->kerekagy->r, kerek->gumi->inR * 2.0f);
		glRotatef(90, 1, 0, 0);
		balvilla->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, (-1.0f) * kerek->kerekagy->r, kerek->gumi->inR * -2.0f);
		glRotatef(90, 1, 0, 0);
		jobbvilla->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, jobbvilla->h, villateteje->h * 0.5f);
		villateteje->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, jobbvilla->h, 0);
		glRotatef(90, 1, 0, 0);
		villanyak->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, jobbvilla->h + villanyak->h, kormany->h * 0.5f);
		kormany->draw();
		glPopMatrix();
	}

};

struct Ember: public Object
{
	Ellipszoid *fej;
	Teglatest *torzs;
	Teglatest *ballab_font;
	Teglatest *ballab_lent;
	Teglatest *ballabfej;
	Teglatest *jobblab_font;
	Teglatest *jobblab_lent;
	Teglatest *jobblabfej;
	Teglatest *balkez_font;
	Teglatest *balkez_lent;
	Teglatest *balkezfej;
	Teglatest *jobbkez_font;
	Teglatest *jobbkez_lent;
	Teglatest *jobbkezfej;

	Ember(Vector fejpos, Vector fejmeret, Vector torzsmeret, float labhossz,
			float labmeret)
	{
		fej = new Ellipszoid(fejpos, fejmeret.x, fejmeret.y, fejmeret.z,
				MAX(MAX(fejmeret.x, fejmeret.y) * 50, 20.0f),
				MAX(fejmeret.z * 20, 20.0f));
		torzs = NULL;
		ballab_font = NULL;
		ballab_lent = NULL;
		ballabfej = NULL;
		jobblab_font = NULL;
		jobblab_lent = NULL;
		jobblabfej = NULL;
		balkez_font = NULL;
		balkez_lent = NULL;
		balkezfej = NULL;
		jobbkez_font = NULL;
		jobbkez_lent = NULL;
		jobbkezfej = NULL;
	}

	~Ember()
	{
		delete fej;
		delete torzs;
		delete ballab_font;
		delete ballab_lent;
		delete ballabfej;
		delete jobblab_font;
		delete jobblab_lent;
		delete jobblabfej;
		delete balkez_font;
		delete balkez_lent;
		delete balkezfej;
		delete jobbkez_font;
		delete jobbkez_lent;
		delete jobbkezfej;
	}

	void draw()
	{
		fej->draw();
	}
};

struct Bringa: public Object
{
	Kerek *hatsokerek;
	Kormanymu *kormany;
	Ember *rider;
	BringaVaz *vaz;

	float h;
	float w;

	Bringa(float height, float width, float kerekR, float gumiR, int kulloSzam)
	{
		h = height;
		w = width;
		hatsokerek = new Kerek(kerekR, gumiR, kulloSzam);
		kormany = new Kormanymu(h, kerekR, gumiR, kulloSzam);
		rider = NULL;
		vaz = new BringaVaz(h * 0.85f, w * 0.9f, gumiR);
	}

	~Bringa()
	{
		delete hatsokerek;
		delete kormany;
		delete rider;
		delete vaz;
	}

	void draw()
	{
		glPushMatrix();
		glTranslatef((-1.0f) * w * 0.45f, 0, 0);
		hatsokerek->draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(w * 0.45f, 0, 0);
		glRotatef(FOK, 0, 1, 0);
		kormany->draw();
		glPopMatrix();

		glPushMatrix();
		setMaterial(PIROS);
		vaz->draw();
		glPopMatrix();
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
		camera = new Camera(Vector(0, 0, 4), Vector(0, 0, 0), Vector(0, 1, 0),
				50.0f, 1.0f, 0.1f, 10.0f);

		Color diffuse(0.5, 0.5, 0.5);
		Color ambient(0, 0, 0);
		Color specular(5, 5, 5);
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
//		Kerek k(0.3, 0.02, 15);
//		Henger h(0.3, 1, 20, 40);
//		Ellipszoid light(0.1, 0.1, 0.1, 30, 30);
//		Ellipszoid e(0.4, 0.5, 0.7, 100, 100);
//		Paraboloid p(10, 20, 400, 40);
//
//		glPushMatrix();
//		glTranslatef(0, -1 + k.gumi->outR + k.gumi->inR, 0);
//		glRotatef(FOK, 0, 0, 1);
//		k.draw();
//		glPopMatrix();
//		float d1[4] =
//		{ 0.4, 0.4, 0, 1 };
//		glMaterialfv(GL_FRONT, GL_DIFFUSE, d1);
//		d1[0] = 0;
//		d1[1] = 0;
//		d1[2] = 0;
//		d1[3] = 1;
//		glMaterialfv(GL_FRONT, GL_SPECULAR, d1);
//		glMaterialf(GL_FRONT, GL_SHININESS, 0.4);
//
//		glPushMatrix();
//		glTranslatef(0, -1, 0);
//		glRotatef(270, 1, 0, 0);
//		float d2[4] =
//		{ 0.4, 0.4, 0.1, 1 };
//		glMaterialfv(GL_FRONT, GL_DIFFUSE, d2);
//		glMaterialfv(GL_FRONT, GL_AMBIENT, d2);
//		glMaterialfv(GL_FRONT, GL_SPECULAR, d2);
//		glMaterialf(GL_FRONT, GL_SHININESS, 0.4);
//		p.draw();
//		glPopMatrix();
//
//		glPushMatrix();
//		glTranslatef(lights[0]->pos.x, lights[0]->pos.y, lights[0]->pos.z);
//		light.draw();
//		glPopMatrix();
//		glPushMatrix();
//		glTranslatef(0, 0, -1);
////		glRotatef(-90, 1, 0, 0);
//		e.draw();
//		glPopMatrix();
//		glPushMatrix();
//		glTranslatef(0, 0.2, -0.5);
//		glRotatef(90, 0, 1, 0);
//		float d[4] =
//		{ 0.4, 0.1, 0.1, 1 };
//		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
//		d[0] = 1;
//		glMaterialfv(GL_FRONT, GL_SPECULAR, d);
//		glMaterialf(GL_FRONT, GL_SHININESS, 0.8);
//		h.draw();
//		glPopMatrix();

		Bringa b(1.2, 2.5, 0.6, 0.04, 10);
		Ember e(Vector(0.5, 2, 0), Vector(0.2, 0.2, 0.2), Vector(1, 1, 1), 1, 1);
		glPushMatrix();
		glTranslatef(0, -0.6, -2);
		glRotatef(FOK - 30, 0, 1, 0);
		b.draw();
		setMaterial(ZOLD);
		e.draw();
		glPopMatrix();

//		setMaterial(PIROS);
//		Teglatest t(Vector(0, 0, 0), Vector(1, 0, 0), Vector(0, 1, 0), 1,
//				1, 1, 5);
//		Teglalap t1(Vector(0, 0, 0), Vector(0, 1, 0), Vector(1, 1, 0),
//				Vector(1, 0, 0), 40);
//		glPushMatrix();
//		glRotatef(FOK, 0, 1, 0);
//		t.draw();
////		t1.draw();
//		glPopMatrix();
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
	glClearColor(0, 0.2, 0.6, 1);
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
	case 'o':
		scene.camera->fov += 5.0f;
		break;
	case 'p':
		scene.camera->fov -= 5.0f;
		break;
	case 'n':
		draw_normals = !draw_normals;
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
//	FOK = fmod(time / 10.0f, 360.0f);
//	scene.lights[0]->pos.x = cos(fmod((time / 10) * 0.01, 360.0f));
//	scene.lights[0]->pos.y = 0.5 * sin(fmod((time / 10) * 0.05, 360.0f));
//	scene.lights[0]->pos.z = sin(fmod((time / 10) * 0.01, 360.0f));
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
