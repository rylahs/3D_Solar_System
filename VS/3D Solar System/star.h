#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;

typedef struct _Position
{
	float x;
	float y;
	float z;
}Position;

class Star
{
	vector<Position> list;
public:
	Star(int amount)
	{
		int count = 0;		
		srand((unsigned)time(NULL));
		
		while(count < amount)
		{
			int sig = 0;
			Position p;
			
			p.x = rand() % 50000 - 25000;
			p.y = rand() % 50000 - 25000;
			p.z = rand() % 50000 - 25000;
			list.push_back(p);
			count++;
		}
	}

	void Draw()
	{
		int count = 0;
		
		glDisable(GL_LIGHTING);

		glPushMatrix();
		glBegin(GL_POINTS);
		glColor3f(1,1,1);
		glPointSize(3);
		
		for(count = 0; count != list.size(); count ++)
		{
			glVertex3f(list[count].x, list[count].y, list[count].z);
		}
		
		glEnd();
		
		glPopMatrix();
		
		glEnable(GL_LIGHTING);
	}
};