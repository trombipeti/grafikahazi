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

//#include <stdio.h>
float FOK = 0;
float P = 0;

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

#define PUSH_MX		(mxstack.push(ModelView))
#define POP_MX		(ModelView = mxstack.pop())

float doround(float a)
{
	if (fabs(a) < 0.0001)
	{
		return 0.0f;
	}
	if (a < -1.0f + 0.0001)
	{
		return -1.0f;
	}
	if (a > 1.0f - 0.0001)
	{
		return 1.0f;
	}
	return a;
}

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector
{
	float x, y, z, w;

	Vector()
	{
		x = y = z = w = 0;
	}
	Vector(float x0, float y0, float z0 = 0, float w0 = 1)
	{
		x = x0;
		y = y0;
		z = z0;
		w = w0;
	}
	Vector operator*(float a) const
	{
		return Vector(x * a, y * a, z * a, w * a);
	}
	Vector operator/(float a) const
	{
		return Vector(x / a, y / a, z / a, w / a);
	}
	Vector operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vector operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	float operator*(const Vector& v) const  	// dot product
	{
		return (x * v.x + y * v.y + z * v.z + w * v.w);
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

	Vector round() const
	{
		Vector ret;
		ret.x = doround(x);
		ret.y = doround(y);
		ret.z = doround(z);
		return ret;
	}
};

Vector operator*(float f, const Vector& v)
{
	return Vector(v.x * f, v.y * f, v.z * f, v.w * f);
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
	PIROS, ZOLD, KEK, EZUST, FEKETE, FEHER, BOR
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
		{ 0, 0, 0, 1 };
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
	case FEHER:
	{
		float a[4] =
		{ 0.5, 0.5, 0.5, 1 };
		float d[4] =
		{ 1, 1, 1, 1 };
		float sp[4] =
		{ 1, 1, 1, 1 };
		float sh = 0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		glMaterialf(GL_FRONT, GL_SHININESS, sh);
		break;
	}
	case BOR:
	{
		float a[4] =
		{ 0.86, 0.71, 0.625, 1 };
		float d[4] =
		{ 1, 1, 1, 1 };
		float sp[4] =
		{ 1, 1, 1, 1 };
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

struct Matrix
{
	float mx[4 * 4];

	float inv[4 * 4];

	Matrix()
	{
		loadIdentity();
	}

	void loadIdentity()
	{
		for (int i = 0; i < 4 * 4; ++i)
		{
			if (i % 5 == 0)
			{
				mx[i] = 1;
				inv[i] = 1;
			}
			else
			{
				mx[i] = 0;
				inv[i] = 0;
			}
		}
	}

	static void copy_mx4(float to[], float from[])
	{
		for (int i = 0; i < 4 * 4; ++i)
		{
			*(to + i) = *(from + i);
		}
	}

	void translate(float px, float py, float pz, bool comp_inv = true)
	{
		Matrix tr;
		tr.mx[12] = px;
		tr.mx[13] = py;
		tr.mx[14] = pz;
		if (comp_inv)
		{
			Matrix i;
			copy_mx4(i.mx, inv);
			*this = tr * (*this);
			i.translate(-px, -py, -pz, false);
			copy_mx4(inv, i.mx);
		}
		else
		{
			*this = (*this) * tr;
		}
	}

	void rotate(float angle, float x, float y, float z, bool comp_inv = true)
	{
		Matrix tr;
		// [0,u2sw ×u2w×w ×u]
		float fok = angle * (M_PI / 180.0f);
		Vector w = sin(fok * 0.5f) * (Vector(x, y, z).norm());
		Vector u0 = Vector(1, 0, 0);
		Vector u0_rot = u0 + 2 * cos(fok * 0.5f) * (w % u0)
				+ (2 * w) % (w % u0);
		tr.mx[0] = u0_rot.x;
		tr.mx[1] = u0_rot.y;
		tr.mx[2] = u0_rot.z;

		Vector u1 = Vector(0, 1, 0);
		Vector u1_rot = u1 + 2 * cos(fok * 0.5f) * (w % u1)
				+ (2 * w) % (w % u1);
		tr.mx[4] = u1_rot.x;
		tr.mx[5] = u1_rot.y;
		tr.mx[6] = u1_rot.z;

		Vector u2 = Vector(0, 0, 1);
		Vector u2_rot = u2 + 2 * cos(fok * 0.5f) * (w % u2)
				+ (2 * w) % (w % u2);
		tr.mx[8] = u2_rot.x;
		tr.mx[9] = u2_rot.y;
		tr.mx[10] = u2_rot.z;

		if (comp_inv)
		{
			Matrix i;
			copy_mx4(i.mx, inv);
			*this = tr * (*this);
			i.rotate(360.0f - angle, x, y, z, false);
			copy_mx4(inv, i.mx);
		}
		else
		{
			*this = (*this) * tr;
		}

	}

	void scale(float sx, float sy, float sz, bool comp_inv = true)
	{
		Matrix tr;
		tr.mx[0] = sx;
		tr.mx[5] = sy;
		tr.mx[10] = sz;

		if (comp_inv)
		{
			Matrix i;
			copy_mx4(i.mx, inv);
			*this = tr * (*this);
			i.scale(1.0f / sx, 1.0f / sy, 1.0f / sz, false);
			copy_mx4(inv, i.mx);
		}
		else
		{
			*this = (*this) * tr;
		}
	}

	Matrix inverz()
	{
		Matrix ret;
		copy_mx4(ret.mx, inv);
		return ret;
	}

	Matrix transzponalt()
	{
		Matrix ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret.mx[i * 4 + j] = mx[i + j * 4];
			}
		}
		return ret;
	}

	Matrix invTrans()
	{
		return inverz().transzponalt();
	}

	Matrix operator*(float a)
	{
		Matrix ret;
		for (int i = 0; i < 4 * 4; ++i)
		{
			ret.mx[i] = mx[i] * a;
		}
		return ret;
	}

	Matrix operator/(float a)
	{
		return *this * (1.0f / a);
	}

	Matrix operator*(const Matrix& m)
	{
		Matrix ret;
		for (int i = 0; i < 4 * 4; ++i)
		{
			float f = 0;
			for (int j = 0; j < 4; ++j)
			{
				int jobb = j * 4 + i % 4;
				int bal = (i / 4) * 4 + j;
				f += mx[bal] * m.mx[jobb];
			}
			ret.mx[i] = f;
		}
		return ret;
	}

};

Matrix ModelView;

struct MatrixStack
{
	Matrix stack[32];
	int top;

	MatrixStack()
	{
		top = 0;
	}

	void push(Matrix m)
	{
		stack[top++] = m;
	}

	Matrix pop()
	{
		return stack[--top];
	}
};

MatrixStack mxstack;

Vector operator*(const Vector& v, const Matrix& m)
{
	Vector ret;

	Matrix ize;
	ize.mx[0] = v.x;
	ize.mx[1] = v.y;
	ize.mx[2] = v.z;
	ize.mx[3] = v.w;

	ize = ize * m;

	ret.x = ize.mx[0];
	ret.y = ize.mx[1];
	ret.z = ize.mx[2];
	ret.w = ize.mx[3];

	return ret;
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

	virtual Vector point(float u, float v, bool transform = true)
	{
		Vector ret = Vector(x(u, v), y(u, v), z(u, v));
		if (transform)
		{
			ret = ret * ModelView;
		}
		return ret;
	}

	virtual Vector normal(float u, float v, bool transform = true)
	{
		Vector ret = (Vector(xdv(u, v), ydv(u, v), zdv(u, v))
				% Vector(xdu(u, v), ydu(u, v), zdu(u, v)));
		if (transform)
		{
			ret = ret * ModelView.invTrans();
		}
		return ret.norm();
	}

	virtual void draw() = 0;

	void doDraw(float u_step, float v_step)
	{
		for (float u = 0; u <= 1.0f; u += u_step)
		{
			for (float v = 0; v <= 1.0f; v += v_step)
			{
				glBegin(GL_TRIANGLES);

				Vector p0 = point(u, v);
				Vector n0 = normal(u, v);
				Vector p1 = point(MIN(u + u_step, 1.0f), v);
				Vector n1 = normal(MIN(u + u_step, 1.0f), v);
				Vector p2 = point(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				Vector n2 = normal(MIN(u + u_step, 1.0f),
						MIN(v + v_step, 1.0f));
				Vector p3 = point(u, MIN(v + v_step, 1.0f));
				Vector n3 = normal(u, MIN(v + v_step, 1.0f));

				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);

				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(p1.x, p1.y, p1.z);

				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);

				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);

				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);

				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(p3.x, p3.y, p3.z);

				glEnd();

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
		{ pos.x, pos.y, pos.z, 1.0f };
		float i_d[4] =
		{ Id.r, Id.g, Id.b, 0 };
		float i_a[4] =
		{ Ia.r, Ia.g, Ia.b, 0 };
		float i_s[4] =
		{ Is.r, Is.g, Is.b, 0 };
		glLightfv(id, GL_POSITION, p);
		glLightfv(id, GL_DIFFUSE, i_d);
		glLightfv(id, GL_AMBIENT, i_a);
		glLightfv(id, GL_SPECULAR, i_s);

		glLightf(id, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(id, GL_QUADRATIC_ATTENUATION, 0.1f);
	}
};

//Light light0(GL_LIGHT0,Vector(0,2,2),)

struct Ellipszoid: public Object
{
	float a;
	float b;
	float c;

	int sides;
	int rings;

	Ellipszoid(float _a, float _b, float _c, float numSides, float numRings)
	{
		a = _a;
		b = _b;
		c = _c;
		sides = numSides;
		rings = numRings;
	}

	float x(float u, float v)
	{
		return a * sin(2 * M_PI * u) * cos(2 * M_PI * v);
	}

	float y(float u, float v)
	{
		return b * sin(2 * M_PI * u) * sin(2 * M_PI * v);
	}

	float z(float u, float v)
	{
		return c * cos(2 * M_PI * u);
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
		return 1.0f / a * sqrt(u / h) * cos(2 * M_PI * v);
	}

	float y(float u, float v)
	{
		return u * h;
	}

	float z(float u, float v)
	{
		return (1.0f / a) * sqrt(u / h) * sin(2 * M_PI * v);
	}

	float xdu(float u, float v)
	{
		float aa = (-1.0f / a) * sqrt((u + 0.00001) / h) * cos(2 * M_PI * v);
		float ret = aa / (2 * 2 * M_PI * (u + 0.00001));
		return ret;
	}

	float ydu(float u, float v)
	{
		return -1;
	}

	float zdu(float u, float v)
	{
		return -a * sqrt((u + 0.00001) / h) * sin(2 * M_PI * v)
				/ (2 * 2 * M_PI * (u + 0.00001));
	}

	float xdv(float u, float v)
	{
		return (-1.0f) * a * sqrt(u / h) * sin(2 * M_PI * v);
	}

	float ydv(float u, float v)
	{
		return 0;
	}
	float zdv(float u, float v)
	{
		return (1.0f / a) * sqrt(u / h) * cos(2 * M_PI * v);
	}

	void draw()
	{

		float u_step = (h / rings) * 0.0000001;
		float v_step = 1.0f / sides;
		for (float u = 0; u < 1.0f; u += u_step)
		{
			u_step = u_step * 2;
			for (float v = 0; v < 1.0f; v += v_step)
			{
				Vector p0 = point(u, v);
				Vector n0 = normal(u, v);
				Vector p1 = point(MIN(u + u_step, 1.0f), v);
				Vector n1 = normal(MIN(u + u_step, 1.0f), v);
				Vector p2 = point(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				Vector n2 = normal(MIN(u + u_step, 1.0f),
						MIN(v + v_step, 1.0f));
				Vector p3 = point(u, MIN(v + v_step, 1.0f));
				Vector n3 = normal(u, MIN(v + v_step, 1.0f));

				glBegin(GL_TRIANGLES);
				glTexCoord2f(u, v);
				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);

				glTexCoord2f(MIN(u + u_step, 1.0f), v);
				glNormal3f(n1.x, n1.y, n1.z);
				glVertex3f(p1.x, p1.y, p1.z);

				glTexCoord2f(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);

				glTexCoord2f(u, v);
				glNormal3f(n0.x, n0.y, n0.z);
				glVertex3f(p0.x, p0.y, p0.z);

				glTexCoord2f(MIN(u + u_step, 1.0f), MIN(v + v_step, 1.0f));
				glNormal3f(n2.x, n2.y, n2.z);
				glVertex3f(p2.x, p2.y, p2.z);

				glTexCoord2f(u, MIN(v + v_step, 1.0f));
				glNormal3f(n3.x, n3.y, n3.z);
				glVertex3f(p3.x, p3.y, p3.z);
				glEnd();
//				v += 0.01f;
			}
		}

	}

	float intersect(Vector ray_p, Vector ray_dv)
	{
		float t = -1;
		ray_dv = ray_dv.norm();

		float A = a * (ray_dv.x * ray_dv.x + ray_dv.y * ray_dv.y);
		float B = 2 * a * (ray_p.x * ray_dv.x + ray_p.y * ray_dv.y) - ray_dv.z;
		float C = a * (ray_p.x * ray_p.x + ray_p.y * ray_p.y) - ray_p.z;

		float discrim = B * B - 4 * A * C;
		if (discrim < 0)
		{
			return -1;
		}

		float t1 = ((-1.0 * B + sqrt(discrim)) / (2.0 * A));
		float t2 = ((-1.0 * B - sqrt(discrim)) / (2.0 * A));
		if ((t2 > t1 || t2 < 0.0001) && t1 > 0.0001)
			t = t1;
		if ((t1 > t2 || t1 < 0.0001) && t2 > 0.0001)
			t = t2;

		if (t > 0.0001)
		{
			return t;
		}

		return -1;
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
			Vector n0(0, 0, 1);
			n0 = n0 * ModelView.invTrans();
			glNormal3f(n0.x, n0.y, n0.z);
			Vector p0 = Vector(0, 0, 0) * ModelView;
			Vector p1 = Vector(x(u, 0), y(u, 0), z(u, 0)) * ModelView;
			Vector p2 = Vector(x(MIN(MIN(u + u_step, 1.0f), 1.0f), 0),
					y(MIN(MIN(u + u_step, 1.0f), 1.0f), 0), z(u, 0))
					* ModelView;
			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();

			glBegin(GL_TRIANGLES);
			n0 = Vector(0, 0, -1) * ModelView.invTrans();
			glNormal3f(n0.x, n0.y, n0.z);
			p0 = Vector(0, 0, z(u, 1)) * ModelView;
			p1 = Vector(x(u, 1), y(u, 1), z(u, 1)) * ModelView;
			p2 = Vector(x(MIN(MIN(u + u_step, 1.0f), 1.0f), 1),
					y(MIN(MIN(u + u_step, 1.0f), 1.0f), 1), z(u, 1))
					* ModelView;
			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();
		}
	}

};

struct Teglalap: public Object
{
	float a, b;
	int res;

	Teglalap(float wa, float wb, int resolution)
	{
		a = wa;
		b = wb;
		res = resolution;
	}

	float x(float u, float v)
	{
		return u * a;
	}

	float y(float u, float v)
	{
		return 0;
	}

	float z(float u, float v)
	{
		return v * b;
	}

	float xdu(float u, float v)
	{
		return 1;
	}

	float ydu(float u, float v)
	{
		return 0;
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
		return 1;
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
	float a;
	float b;
	float c;
	int res;

	Teglatest(float wa, float wb, float wc, int resolution)
	{
		a = wa;
		b = wb;
		c = wc;
		res = resolution;
	}

	void draw()
	{
		Teglalap B(a, c, res);
		Teglalap D(a, b, res);
		Teglalap L(c, b, res);

		PUSH_MX;
		ModelView.translate(a, 0, 0);
		ModelView.rotate(180, 0, 0, 1);
		D.draw();
		POP_MX;

		PUSH_MX;
		ModelView.rotate(90, 0, 0, 1);
		L.draw();
		POP_MX;

		PUSH_MX;
		ModelView.rotate(270, 1, 0, 0);
		B.draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, c, 0);
		D.draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(a, c, 0);
		ModelView.rotate(270, 0, 0, 1);
		L.draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, c, b);
		ModelView.rotate(90, 1, 0, 0);
		B.draw();
		POP_MX;

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
			PUSH_MX;
			ModelView.rotate(90, 1, 0, 0);
			float rot = (360.0f / kulloszam) * i;
			ModelView.rotate(rot, 0, 1, 0);
			kullok[i]->draw();
			POP_MX;
		}
		PUSH_MX;
		ModelView.translate(0, 0, kerekagy->h * 0.5f);
		kerekagy->draw();
		POP_MX;
		setMaterial(FEKETE);
		float sp[4] =
		{ 0, 0, 0, 1 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
		gumi->draw();
	}

	~Kerek()
	{
		delete gumi;
		for (int i = 0; i < kulloszam; ++i)
		{
			delete kullok[i];
		}
		delete kerekagy;
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
		ballent = new Henger(csoR, w * 0.4, 2, 10);
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
		PUSH_MX;
		ModelView.rotate(90, 1, 0, 0);
		ModelView.translate(w * -0.1f, 0, 0);
		kozep->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * -0.1f, h - csoR, 0);
		ModelView.rotate(270, 0, 1, 0);
		jobbfont->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * -0.1f, 0, 0);
		ModelView.rotate(270, 0, 1, 0);
		float c = acos(MIN(h / jobblent->h, jobblent->h / h));
		ModelView.rotate(90 - c * (180.0 / M_PI), 1, 0, 0);
		jobblent->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * -0.1f, 0, 4 * csoR);
		pedalcso->draw();
		ModelView.translate(0, h - csoR, 0);
		pedalcso->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * -0.1f, 0, 3 * csoR);
		ModelView.rotate(90, 0, 1, 0);
		ballent->draw();
		ModelView.translate(6 * csoR, 0, 0);
		ballent->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * -0.1f - ballent->h, 0, 3 * csoR);
		ModelView.rotate(90, 0, 1, 0);
		c = atan(MIN(h / ballent->h, ballent->h / h));
		ModelView.rotate(90 + c * (180.0 / M_PI), 1, 0, 0);
		balfont->draw();
		ModelView.translate(6 * csoR, 0, 0);
		balfont->draw();
		POP_MX;
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
		PUSH_MX;
//		ModelView.rotate(FOK * -1, 0, 0, 1);
		kerek->draw();
		POP_MX;

		setMaterial(PIROS);
		PUSH_MX;
		ModelView.translate(0, (-1.0f) * kerek->kerekagy->r,
				kerek->gumi->inR * 2.0f);
		ModelView.rotate(90, 1, 0, 0);
		balvilla->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, (-1.0f) * kerek->kerekagy->r,
				kerek->gumi->inR * -2.0f);
		ModelView.rotate(90, 1, 0, 0);
		jobbvilla->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, jobbvilla->h, villateteje->h * 0.5f);
		villateteje->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, jobbvilla->h, 0);
		ModelView.rotate(90, 1, 0, 0);
		villanyak->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(0, jobbvilla->h + villanyak->h, kormany->h * 0.5f);
		kormany->draw();
		POP_MX;
	}

};

struct Ember: public Object
{
	Ellipszoid *fej;
	Teglatest *torzs;
	Teglatest *lab;
	Teglatest *labfej;
	Teglatest *kez;

	Ember(Vector fejmeret, Vector torzsmeret, float labhossz)
	{
		fej = new Ellipszoid(fejmeret.x, fejmeret.y, fejmeret.z,
				MAX(MAX(fejmeret.x, fejmeret.y) * 50, 20.0f),
				MAX(fejmeret.z * 20, 20.0f));
		torzs = new Teglatest(torzsmeret.x, torzsmeret.y, torzsmeret.z, 10);

		kez = new Teglatest(torzsmeret.x * 0.95f, fejmeret.x, fejmeret.x, 10);
		lab = new Teglatest(torzsmeret.z, torzsmeret.y * 0.3f, labhossz, 10);
		labfej = NULL;
	}

	~Ember()
	{
		delete fej;
		delete torzs;
		delete lab;
		delete labfej;
		delete kez;
	}

	void draw()
	{
		setMaterial(KEK);
		PUSH_MX; // 1
		ModelView.rotate(20, 0, 0, 1);

		PUSH_MX; // 2
		ModelView.translate(0, 0, torzs->b * 0.49f - lab->b);
		lab->draw();

		PUSH_MX; // 3
		ModelView.translate(0, 0, -torzs->b * 0.98f + lab->b);
		lab->draw();
		POP_MX; // 2

		POP_MX; // 1

		PUSH_MX; // 2
		setMaterial(ZOLD);
		ModelView.translate(0, lab->c * 0.9, -0.5f * torzs->b);
		ModelView.rotate(10, 0, 0, 1);
		torzs->draw();

		PUSH_MX; // 3
		ModelView.translate(torzs->a - kez->b, kez->c, 0);
		ModelView.rotate(295, 0, 0, 1);
		kez->draw();

		ModelView.translate(0, 0, torzs->b - kez->b);
		kez->draw();
		POP_MX; // 2

		setMaterial(BOR);
		ModelView.translate(torzs->a + fej->a, 0, torzs->b * 0.5f);
		fej->draw();

		POP_MX; // 1

		POP_MX; // 0
	}
};

struct Bringas: public Object
{
	Kerek *hatsokerek;
	Kormanymu *kormany;
	BringaVaz *vaz;
	Ember *rider;

	float rotation;

	float h;
	float w;

	Bringas(float height, float width, float kerekR, float gumiR, int kulloSzam)
	{
		h = height;
		w = width;
		hatsokerek = new Kerek(kerekR, gumiR, kulloSzam);
		kormany = new Kormanymu(h, kerekR, gumiR, kulloSzam);
		vaz = new BringaVaz(h * 0.85f, w * 0.9f, gumiR);
		rider = new Ember(Vector(kerekR * 0.3f, kerekR * 0.3f, kerekR * 0.3f),
				Vector(kerekR * 2.0f, height * 0.8f, kerekR * 0.4f), height);
		rotation = 0;
	}

	~Bringas()
	{
		delete hatsokerek;
		delete kormany;
		delete vaz;
		delete rider;
	}

	void drawOnParaboloid(Paraboloid *p, float u, float v)
	{
		static float prev_u = 0;
		static float prev_v = 0;
		Vector pp = p->point(u, v, false);
		Vector pn = p->normal(u, v);
		Vector axis = Vector(0, 1, 0) % pn;
		float c = Vector(0, 1, 0) * pn.round();
		if (fabs(c) > 0.999)
		{
			c = (c < 0 ? -1 : 1);
		}
		PUSH_MX;
		ModelView.loadIdentity();
		ModelView.translate(pp.x, pp.y + h * 0.5f, pp.z);
		if (!isnan(c))
		{
			ModelView.rotate(acos(c) * (180.0f / M_PI) + 180.0f, axis.x, axis.y,
					axis.z);
		}
		ModelView.rotate(rotation, 0, 1, 0);
		if (u != prev_u || v != prev_v)
		{
//			printf("Ide jöttem (%.1f, %.1f): %f, %f, %f\n", u, v, pp.x, pp.y,
//					pp.z);
			prev_u = u;
			prev_v = v;
		}
		draw();
		POP_MX;
	}

	void draw()
	{
		PUSH_MX;
		ModelView.translate((-1.0f) * w * 0.45f, 0, 0);
		hatsokerek->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(w * 0.45f, 0, 0);
//		ModelView.rotate(FOK, 0, 1, 0);
		kormany->draw();
		POP_MX;

		PUSH_MX;
		setMaterial(PIROS);
		vaz->draw();
		POP_MX;

		PUSH_MX;
		ModelView.translate(-w * 0.1f, 0, 0);
		rider->draw();
		POP_MX;
	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

struct Scene
{
	Camera *camera;

	Light *l0;

	unsigned int texids;
	unsigned char image[256][256][3];

	Bringas *b1;
	Bringas *b2;
	Bringas *b3;

	Paraboloid *p;

	Scene()
	{
		camera = NULL;
		texids = 0;
		b1 = b2 = b3 = NULL;
		p = NULL;
		l0 = NULL;
	}

	~Scene()
	{
		delete camera;
		delete l0;
		delete b1;
		delete b2;
		delete b3;
		delete p;

	}

	void genTexture()
	{
		for (int x = 0; x < 256; ++x)
		{
			for (int y = 0; y < 256; ++y)
			{
				if ((x / 8) % 2 == 0 || (y / 8) % 2 == 1)
				{
					image[x][y][1] = 255;
					image[x][y][2] = cos(x / 265.0f) * 255;
				}
				else if ((y / 8) % 2 == 0 || (x / 8) % 2 == 1)
				{
					image[x][y][1] = sin(y / 265.0f) * 16;
					image[x][y][2] = sin(y / 265.0f) * 255;
				}
				image[x][y][0] = 10
						* sin(
								((y << x) ^ 0x23434) * 0.435634
										* fmod(cos(x >> 3 | 0x332), 12));
			}
		}
	}

	void build()
	{
		camera = new Camera(Vector(0, 1.3, 4), Vector(0, 0, 0), Vector(0, 1, 0),
				70.0f, 1.0f, 0.5f, 100.0f);

		Color diffuse(1, 1, 1);
		Color ambient(0.2, 0.2, 0.2);
		Color specular(5, 5, 5);
		l0 = new Light(GL_LIGHT0, Vector(0, 2, 4), diffuse, ambient, specular);

		b1 = new Bringas(0.6, 1.2, 0.3, 0.02, 10);
		b2 = new Bringas(0.6, 1.2, 0.3, 0.02, 10);
		b3 = new Bringas(0.6, 1.2, 0.3, 0.02, 10);

		p = new Paraboloid(0.01, 10, 20, 200);

		glGenTextures(1, &texids);
		glBindTexture(GL_TEXTURE_2D, texids);

		genTexture();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	void render()
	{
		camera->setOGL();
		l0->setOGL();

		PUSH_MX;
//		ModelView.rotate(340, 0, 1, 0);
		b1->rotation = 225.0f;
		b1->drawOnParaboloid(p, 0.001, 0.875);
		POP_MX;
//
		PUSH_MX;
		b2->drawOnParaboloid(p, 0.003, 0.5);
		POP_MX;

		PUSH_MX;
		b3->rotation = 300.0f;
		b3->drawOnParaboloid(p, 0.002, 0.7);
		POP_MX;

		setMaterial(FEHER);
		glEnable(GL_TEXTURE_2D);
		p->draw();
		glDisable(GL_TEXTURE_2D);
	}
};

Scene scene;
bool line = false;
// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization()
{
	glViewport(0, 0, screenWidth, screenHeight);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	Matrix m;
	float glm[16];

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m.loadIdentity();

	glPushMatrix();
	mxstack.push(m);
	glRotatef(10, 0, 0, 1);
	m.rotate(10, 0, 0, 1);

	glGetFloatv(GL_MODELVIEW_MATRIX, glm);
	glPushMatrix();
	mxstack.push(m);

	glTranslatef(10, 2, 0);
	m.translate(10, 2, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm);

	glPopMatrix();
	m = mxstack.pop();
	glGetFloatv(GL_MODELVIEW_MATRIX, glm);

	glPopMatrix();
	m = mxstack.pop();
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
	case 'w':
		scene.p->a *= 2;
		break;
	case 's':
		scene.p->a /= 2;
		break;
	case 'k':
		P += 0.001;
		break;
	case 'l':
		P -= 0.001;
		break;
	case 'o':
		scene.camera->fov += 5.0f;
		break;
	case 'p':
		scene.camera->fov -= 5.0f;
		break;
	case 'f':
		line = !line;
		if (line)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'h':
		scene.camera->lookat.y -= 0.5;
		break;
	case 'z':
		scene.camera->lookat.y += 0.5;
		break;
	case 'g':
		scene.camera->eye.y -= 0.5;
		break;
	case 't':
		scene.camera->eye.y += 0.5;
		break;
	case 'u':
		FOK += 1;
		break;
	case 'i':
		FOK -= 1;
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
		glutPostRedisplay(); 			// Ilyenkor rajzold ujra a kepet
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
//	scene.l0->pos.x = 2 * cos(fmod((time / 10) * 0.01, 360.0f));
//	scene.l0->pos.y = 3 + sin(fmod((time / 10) * 0.05, 360.0f));
//	scene.l0->pos.z = 2 * sin(fmod((time / 10) * 0.01, 360.0f));
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

	onInitialization();		// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);			// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}
