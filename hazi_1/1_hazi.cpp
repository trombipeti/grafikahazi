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
// Nev    : Trombitás Péter
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

    Vector( )
    {
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0)
    {
        x = x0;
        y = y0;
        z = z0;
    }
    Vector operator*(float a)
    {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v)
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v)
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v)   	// dot product
    {
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v)   	// cross product
    {
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    bool operator==(const Vector& v)
    {
        return (x == v.x && y == v.y && z == v.z);
    }
};

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

//-----------------------------------------------
// Kontroll pont
//-----------------------------------------------
struct ControlPoint
{
    Vector cp;
    Vector v;
    Vector a;

    long t;

    ControlPoint() {};

    ControlPoint(Vector _cp, long _t) : cp(_cp), t(_t) {}

    ControlPoint(Vector _cp, Vector _v, Vector _a) :
        cp(_cp), v(_v), a(_a) {}
};

struct RussianSpline
{
    ControlPoint cps[100];
    int numCtrlPts;

    RussianSpline()
    {
        numCtrlPts = 0;
    }

    void addControlPoint(Vector cp, long _t)
    {
        if(numCtrlPts < 100)
        {
            cps[numCtrlPts++] = ControlPoint(cp,_t);
        }
    }



    void drawCtrlPts()
    {
        glColor3f(1.0, 1.0, 1.0);
        for(int i = 0; i<numCtrlPts; ++i)
        {
            glBegin(GL_TRIANGLE_FAN);
            for(float angle = 0.0f; angle < 2*M_PI; angle += 0.2f)
            {
                float x = cps[i].cp.x + sin(angle);
                float y = cps[i].cp.y + cos(angle);
                glVertex2f(x,y);
            }
            glEnd();
        }
    }
};


typedef enum
{
    ADDING_POINTS,      // Kontrollpontok hozzáadása
    SETTING_UP_VECTORS, // Kezdeti gyorsulás és sebesség beállítása
    VECTORS_SETUP,      // Készen van minden, lehet nyomni a space-t
    ANIMATING           // Körbenézegetés
} PROGSTATE;

PROGSTATE prog_state = ADDING_POINTS;

typedef enum
{
    NONE,
    B1CLK,
    B2CLK,
    B3CLK
} BCLK;

BCLK clickType = NONE;
long lastClickTime = 0;
Vector lastMousePos;

bool startVectorSetup = false;
bool accelSetup = false;

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

RussianSpline spline;

float px, py;
float phi;
float sx, sy;

double wl, wr, wb, wt;

float normCoordX(float x)
{
    return x*(2.0f/screenWidth) - 1.0f;
}

float normCoordY(float y)
{
    return (screenHeight - y)*(2.0f/screenHeight) - 1.0f;
}


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( )
{
    glViewport(0, 0, screenWidth, screenHeight);

    px = 0.0f;
    py = 0.0f;
    phi = 0.0f;
    sx = 1.0f;
    sy = 1.0f;

    wl = 100;
    wr = 200;
    wb = 300;
    wt = 400;

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(wl,wr,wb,wt);

}


// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( )
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // ..


    // ...
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(wl,wr,wb,wt);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(px,py,0.0f);
    glRotatef(phi,0.0f,0.0f,1.0f);
    glScalef(sx,sy,1.0f);

    spline.drawCtrlPts();


    glutSwapBuffers();     				// Buffercsere: rajzolas vege

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
        if(prog_state == VECTORS_SETUP || prog_state == ANIMATING)
        {
            prog_state = ANIMATING;
            // Animáció indítása...
        }
    }

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y)
{

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)    // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
    {
        long tsec = glutGet(GLUT_ELAPSED_TIME);

        Vector curMousePos( normCoordX(x) * ( (wr - wl) / 2.0 ) + (wr+wl)/2.0,
                            normCoordY(y) * ( (wt - wb) / 2.0 ) + (wt+wb)/2.0);
        if(tsec - lastClickTime <= 300 && clickType < B3CLK && curMousePos == lastMousePos)
        {
            clickType = (BCLK)((int)clickType + 1);
        }
        else
        {
            clickType = B1CLK;
        }
        lastClickTime = tsec;
        lastMousePos = curMousePos;
        std::cout << "CLK: " << clickType << ", state: " << prog_state << std::endl;

        switch(clickType)
        {
        case B1CLK:
            if(prog_state == ADDING_POINTS)
            {
                spline.addControlPoint(lastMousePos, tsec);
            }
            break;
        case B2CLK:
            prog_state = SETTING_UP_VECTORS;

            startVectorSetup = true;
            if(accelSetup)
            {
                prog_state = VECTORS_SETUP;
            }
            break;
        case B3CLK:
            prog_state = SETTING_UP_VECTORS;

            accelSetup = true;
            if(startVectorSetup)
            {
                prog_state = VECTORS_SETUP;
            }
            break;
        default:
            break;
        }

        glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
    }
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( )
{
    long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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

