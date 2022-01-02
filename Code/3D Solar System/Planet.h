#include <math.h>

class Planet
{
	float radius;			//반지름
	float distance_FromSun; //태양으로부터 거리
	float revolution_orbit; //공전 주기
	float period_axis;		//자전축
	float period_rotation;	//자전 주기
	float inclination; // 공전축
	GLUquadric * Sphere;
	GLuint tex;
	
public:
	
	//지름 ,거리 ,공전궤도, 공전 주기, 자전 주기
	Planet(GLUquadric * Sphere, GLuint tex, float radius, float distance_FromSun, float revolution_orbit, float period_axis, float period_rotation,
			float inclination) 
	{
		this->radius = radius;
		this->distance_FromSun = distance_FromSun;
		this->revolution_orbit = revolution_orbit;
		this->period_rotation = period_rotation;
		this->period_axis = period_axis;
		this->Sphere = Sphere;
		this->tex = tex;
		this->inclination = inclination;
	}
	
	void DrawOrbit(float p)
	{
		glColor3ub(255, 255, 255);
		glDisable(GL_LIGHTING);
	
		glBegin(GL_LINE_STRIP);

		for (int i = 0; i < 361; i++)
		{
			glVertex3f(p * (float)sin(i * 3.1492 / 180), 0, p * (float)cos(i * 3.14 / 180));
		}
		glEnd(); 
		
		glEnable(GL_LIGHTING);
	}
	
	void Paint()
	{
		glPushMatrix();
			
			
			glRotatef(inclination, 0, 0, 1); //공전 각도
			
			DrawOrbit(distance_FromSun);
			// 공전 주기 : tic당 xx도
			glRotatef(revolution_orbit, 0.0f, 1.0f, 0.0f);
			// 태양으로부터의 거리
			glTranslatef(distance_FromSun, 0.0f, 0.0f);
			// Draw the electron
			glPushMatrix();
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tex);  //GLuint
				
				glRotatef(period_axis, 1, 0, 0);
				glPushMatrix();
					glRotatef(period_rotation, 0, 1, 0);
					glRotatef(90, 1, 0, 0);
					gluSphere(Sphere, radius, 30, 30);
					//glColor3ub(255,255, 255);
					/*glBegin(GL_LINES);
						glVertex3f(0, 0, -radius*2);  //공전 축
						glVertex3f(0, 0, radius*2);
					glEnd();*/
					
				glPopMatrix();
				
				glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		glPopMatrix();
	}
};