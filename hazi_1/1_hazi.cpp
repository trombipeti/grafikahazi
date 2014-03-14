//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization f√ºggvenyt kiveve, a lefoglalt adat korrekt felszabaditasa nelk√ºl
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

#include <iostream>

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector
{
    float x, y, z;

    Vector(float v = 0)
    {
        x = y = z = v;
    }

    Vector(float x0, float y0, float z0 = 0)
    {
        x = x0;
        y = y0;
        z = z0;
    }

    Vector operator/(float a)
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

    Vector operator*(const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }

    Vector operator/(const Vector& v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    Vector operator%(const Vector& v) const     // cross product
    {
        return Vector(y * v.z - z * v.y,
                      z * v.x - x * v.z,
                      x * v.y - y * v.x);
    }
    float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    bool operator==(const Vector& v) const
    {
        return (x == v.x && y == v.y && z == v.z);
    }

    bool operator!=(const Vector& v) const
    {
        return !(*this == v);
    }
};

Vector operator*(float f, const Vector& v)
{
    return (Vector)v * Vector(f,f,f);
}

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color
{
    float r, g, b;

    Color( )
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

struct RussianSpline
{
    Vector cps[100];
    int numCtrlPts;
    Vector all_v[0];
    int known_v[100];
    Vector all_a[100];
    int known_a[100];

    RussianSpline()
    {
        numCtrlPts = 0;
        for(int i = 0; i<100;++i)
        {
            known_a[i] = 0;
        }
    }

    void setA0(const Vector& a)
    {
        all_a[0] = a;
        for(int i = 0; i<100;++i)
        {
            known_a[i] = 0;
        }
        known_a[0] = 1;
    }

    void setV0(const Vector& v)
    {
        all_v[0] = v;
        for(int i = 0;i<100;++i)
        {
            known_v[i] = 0;
        }
        known_v[0] = 1;
        
    }

    void addControlPoint(const Vector& cp)
    {
        if(numCtrlPts < 100)
        {
            cps[numCtrlPts++] = cp;
        }
    }

    float t(int i)
    {
        return cps[i].z - cps[0].z;
    }

    Vector _a2(int i)
    {
        return 0.5f * a(i);
    }

    Vector _a3(int i)
    {
         return (
                 -4 * ( (p(i) - p(i+1)) / pow(t(i+1) - t(i), 3) ) -
                 ((v(i+1) + 3*v(i))     / pow(t(i+1) - t(i), 2))      -
                 ( a(i)                 /    (t(i+1) - t(i))  )
                 );
    }

    Vector _a4(int i)
    {
        return (
                (3 * ( (p(i) - p(i+1) ) /       pow(t(i+1) - t(i), 4) )) +
                (   ( v(i+1) + 2*v(i) ) /       pow(t(i+1) - t(i), 3) )  +
                                ( a(i)  / ( 2 * pow(t(i+1) - t(i), 2)))
                );
    }

    Vector a(int i)
    {
        std::cout << "Getting a: " << i << std::endl;
        if(known_a[i] == 1 || i == 0)
        {
            return all_a[i];
        }
        else
        {
            Vector a2 = _a2(i-1);

            Vector a3 = _a3(i-1);
            Vector a4 = _a4(i-1);

            Vector ret = 12*a4*pow(t(i) - t(i-1),2) + 6*a3*(t(i) - t(i-1)) + 2*a2;

            all_a[i] = ret;
            known_a[i] = 1;
            return ret;
        }
    }

    Vector v(int i)
    {
        std::cout << "Getting v: " << i << std::endl;
	if(known_v[i] == 1 || i == 0)
        {
            return all_v[i];
        }
        else
        {
            Vector elozo = (p(i) - p(i-1)) / (t(i) - t(i-1));
            Vector kovi  = (p(i+1) - p(i)) / (t(i+1) - t(i));
           
            Vector ret = (elozo + kovi) * 0.5f;
            all_v[i] = ret;
            known_v[i] = 1;
            return ret;
        }
    }

    const Vector& p(int i)
    {
        return cps[i];
    }

    int index_for_t(float tm)
    {
        for(int i = 0;i < numCtrlPts - 1; ++i)
        {
            if(t(i+1) > tm)
            {
                return i;
            }
        }
        return numCtrlPts - 1;
    }


    Vector r(float _t)
    {
        int i = index_for_t(_t);
        if(i == numCtrlPts - 1)
        {
            return cps[numCtrlPts - 1];
        }
        Vector _a[5];
        _a[0] = p(i);
        _a[1] = v(i);
        _a[2] = _a2(i);
        _a[3] = _a3(i);
        _a[4] = _a4(i);

        Vector r;
        for(int j = 0; j<5; ++j)
        {
            r = r + _a[j] * pow(_t - t(i), j);
        }
        return r;

    }

    void draw()
    {
        if(numCtrlPts == 0) return;
        std::cout << "RussianSplie::draw()" << std::endl;
        if(known_v[0] == 1)
        {
            std::cout << ">>>> Sebesseg" << std::endl;
            glColor3f(0.0,1.0,0.0);
            glBegin(GL_TRIANGLE_FAN);
            for(float angle = 0.0f; angle < 2*M_PI; angle += 0.2f)
            {
                float x = (cps[0] + all_v[0]).x + sin(angle);
                float y = (cps[0] + all_v[0]).y + cos(angle);
                glVertex2f(x,y);
            }
            glEnd();
            glBegin(GL_LINE_STRIP);
                glVertex2f((cps[0] + all_v[0]).x, (cps[0] + all_v[0]).y);
                glVertex2f(cps[0].x, cps[0].y);
            glEnd();
            std::cout << "Sebesseg <<<<" << std::endl;
        }
       
        if(known_a[0] == 1)
        {
            std::cout << ">>>> Gyorsulas" << std::endl;
            glColor3f(0.0,0.0,1.0);
            glBegin(GL_TRIANGLE_FAN);
            for(float angle = 0.0f; angle < 2*M_PI; angle += 0.2f)
            {
                float x = (cps[0] + all_a[0]).x + sin(angle);
                float y = (cps[0] + all_a[0]).y + cos(angle);
                glVertex2f(x,y);
            }
            glEnd();
            glBegin(GL_LINE_STRIP);
                glVertex2f((cps[0] + all_a[0]).x, (cps[0] + all_a[0]).y);
                glVertex2f(cps[0].x, cps[0].y);
            glEnd();
            std::cout << "Gyorsulas <<<<" << std::endl;
        }

        std::cout << ">>>> CP rajz" << std::endl;
        glColor3f((50.0f/255.0f), (50.0f/255.0f), (50.0f/255.0f));
        for(int i = 0; i<numCtrlPts; ++i)
        {
            glBegin(GL_TRIANGLE_FAN);
            for(float angle = 0.0f; angle < 2*M_PI; angle += 0.1f)
            {
                float x = cps[i].x + sin(angle);
                float y = cps[i].y + cos(angle);
                glVertex2f(x,y);
            }
            glEnd();
        }
        std::cout << "CP rajz <<<<" << std::endl;
        if(numCtrlPts > 1)
        {
            std::cout << "Gˆrbe rajzol·sa" << std::endl;
            glColor3f((M_PI*M_E)/10.0f, 1.0f - M_E/20.0f, 0.0f);
            const float res = (t(numCtrlPts - 1) - t(0)) / 2000.0f;
            glBegin(GL_LINE_STRIP);
            for(float _time = t(0); _time <= t(numCtrlPts - 1) ; _time += res)
            {
                Vector v = r(_time);
                glVertex2f(v.x, v.y);
            }
            glEnd();
        }
    }
};

typedef enum
{
    NONE,
    B1CLK,
    B2CLK,
    B3CLK,
    BMOV
} BCLK;

BCLK clickType = NONE;
long lastClickTime = -1;
Vector lastCPPos;
Vector lastMousePos;

const int screenWidth = 600;	// alkalmazas ablak felbontasa
const int screenHeight = 600;

RussianSpline spline;

bool dont_add = false;

Vector pv;
Vector pt;
Vector pt_0;
Vector pa;
float _mu = - (100.0/9.0)*2;
float phi;
float sx, sy;

double wl, wr, wb, wt;
#define WL          100
#define WR          200
#define WB          300
#define WT          400

#define MAX(a,b) ((a) > (b) ? (a) : (b))

long eltol_start = 0;
long anim_start  = 0;
long anim_end = 0;

float normCoordX(float x)
{
    return x*(2.0f/screenWidth) - 1.0f;
}

float normCoordY(float y)
{
    return (screenHeight - y)*(2.0f/screenHeight) - 1.0f;
}


void updateMouseState()
{
    if(lastClickTime < 0 || dont_add) return;


    long tmsec = glutGet(GLUT_ELAPSED_TIME);
    if(tmsec - lastClickTime > 300 && clickType == B1CLK)
    {
        spline.addControlPoint(lastCPPos);
        clickType = NONE;
    }
    else if(tmsec - lastClickTime > 300 && clickType == B2CLK )
    {
        spline.setV0(Vector(lastCPPos.x, lastCPPos.y) - spline.cps[0]);
        clickType = NONE;
    }
    else if(clickType == B3CLK )
    {
        spline.setA0(Vector(lastCPPos.x, lastCPPos.y) - spline.cps[0]);
        clickType = NONE;
    }
}

float screenCoordX(float x)
{
    return normCoordX(x) * ( (wr - wl) / 2.0 ) + (wr+wl)/2.0 - pt.x;
}

float screenCoordY(float y)
{
    return normCoordY(y) * ( (wt - wb) / 2.0 ) + (wt+wb)/2.0 - pt.y;
}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( )
{
    glViewport(0, 0, screenWidth, screenHeight);

    pt = Vector(0,0,0);
    phi = 0.0f;
    sx = 1.0f;
    sy = 1.0f;

    wl = WL;
    wr = WR;
    wb = WB;
    wt = WT;

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(wl,wr,wb,wt);

}

long prev_draw_time = 0;

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg

void onDisplay( )
{
    long time = glutGet(GLUT_ELAPSED_TIME);
    std::cout << time << ", rajzolok" << std::endl;
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // ..
    if(pv != Vector(0,0,0))
    {
        float dtime = (time - eltol_start);
        float maxt = MAX(fabs(pv.x/pa.x), fabs(pv.y/pa.y)) * 1000.0f;
        if(dtime > maxt)
        {
            pt = pt_0 + (maxt/1000.0)*pv + (0.5f*pa/1000.0)*(maxt/1000.0)*(maxt/1000.0);
            pv = Vector(0,0,0);
        }
        else
        {
            pt = pt_0 + (dtime/1000.0)*pv + (0.5f*pa/1000.0)*(dtime/1000.0)*(dtime/1000.0);
        }

    }

    // ...
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(wl,wr,wb,wt);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(pt.x,pt.y,pt.z);
    glRotatef(phi,0.0f,0.0f,1.0f);
    glScalef(sx,sy,1.0f);

    spline.draw();
    
    std::cout << "Kirajzolva" << std::endl;
    if(anim_start > anim_end && anim_start != 0)
    {
        std::cout << "Anim·lok..." << std::endl;
        Vector r = spline.r((time - anim_start)/1000.0f);
        glColor3f(1.0f,0.0f,0.0f);
        glBegin(GL_TRIANGLE_FAN);
        for(float angle = 0.0f; angle <= 2*M_PI; angle += 0.1f)
        {
            float x = r.x + sin(angle)*2;
            float y = r.y + cos(angle)*2;
            glVertex2f(x, y);
        }
        glEnd();

        if(r == spline.cps[spline.numCtrlPts - 1] && anim_end <= anim_start)
        {
            anim_end = time;
        }
    }


    glutSwapBuffers();     				// Buffercsere: rajzolas vege
    prev_draw_time = glutGet(GLUT_ELAPSED_TIME);
    glutPostRedisplay();
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y)
{
    if (key == 'd')
    {
        glutPostRedisplay( );    // d beture rajzold ujra a kepet
    }
    else if (key == ' ')
    {
        anim_start = glutGet(GLUT_ELAPSED_TIME);
    }
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y)
{

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y)
{
    std::cout << ">>>> onMouse" << std::endl;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)    // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
    {
        long tmsec = glutGet(GLUT_ELAPSED_TIME);
	std::cout << tmsec << std::endl;
        Vector curCPPos( screenCoordX(x), screenCoordY(y), tmsec / 1000.0f);
        if      ( clickType == NONE )       clickType = B1CLK;
        else if ( clickType == B1CLK )      clickType = B2CLK;
        else if ( clickType == B2CLK )      clickType = B3CLK;



        lastClickTime = tmsec;
        lastMousePos = Vector(screenCoordX(x),screenCoordY(y));
        lastCPPos = curCPPos;
        updateMouseState();

        glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
    }
    else if(state == GLUT_UP)
    {
        if( ! (Vector(screenCoordX(x),screenCoordY(y)) == lastMousePos))
        {
            std::cout << glutGet(GLUT_ELAPSED_TIME) << std::endl;
            clickType = NONE;
        }
        dont_add = false;
    }
    std::cout << "onMouse <<<<" << std::endl;
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{
    long t = glutGet(GLUT_ELAPSED_TIME);
    if(Vector(x,y) != lastMousePos)
    {
        dont_add = true;
    }
    if(t - lastClickTime >= 500 && clickType != BMOV /*&& prog_state == VECTORS_SETUP*/)
    {
        clickType = BMOV;
        Vector curMousePos(screenCoordX(x > screenWidth ? screenWidth : x),
                           screenCoordY(y > screenHeight ? screenHeight : y));
        pv = ((lastMousePos - curMousePos)/(3.0));
//        pv.x *= -1.0f;
        pa.x = _mu * (pv.x < 0 ? -1 : 1);
        pa.y = _mu * (pv.y < 0 ? -1 : 1);
        pt_0 = pt;
        eltol_start = t;
    }
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( )
{
    std::cout << "onIdle <<<<" << std::endl;
    updateMouseState();

    //glutPostRedisplay();
   std::cout << ">>>> onIdle" << std::endl;
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv)
{
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}

