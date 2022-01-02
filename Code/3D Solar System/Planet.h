#include <math.h>

class Planet
{
	float radius;			//������
	float distance_FromSun; //�¾����κ��� �Ÿ�
	float revolution_orbit; //���� �ֱ�
	float period_axis;		//������
	float period_rotation;	//���� �ֱ�
	float inclination; // ������
	GLUquadric * Sphere;
	GLuint tex;
	
public:
	
	//���� ,�Ÿ� ,�����˵�, ���� �ֱ�, ���� �ֱ�
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
			
			
			glRotatef(inclination, 0, 0, 1); //���� ����
			
			DrawOrbit(distance_FromSun);
			// ���� �ֱ� : tic�� xx��
			glRotatef(revolution_orbit, 0.0f, 1.0f, 0.0f);
			// �¾����κ����� �Ÿ�
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
						glVertex3f(0, 0, -radius*2);  //���� ��
						glVertex3f(0, 0, radius*2);
					glEnd();*/
					
				glPopMatrix();
				
				glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		glPopMatrix();
	}
};