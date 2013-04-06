// -----------------------------------
// Version: Hierarchical Display lists
// Antonio Ramires Fernandes
// www.lighthouse3d.com
// -----------------------------------
#include <GL/glew.h>
#include <GL/glut.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "PerformanceTimer.h"

struct Edge
{
	unsigned int *indice;
	float *vertex_ptr;
};

struct Triangle
{
	float *vertex_ptr;
	unsigned int *indice;
};


Edge vertex_ptr;
Edge edge_ptr;
Triangle triangle_ptr;
unsigned int number_elements;
 

GLuint vboVertex;
GLuint vboList;

 
char title[30];

float width = 1024;
float height = 768;
float m_Ortho[]  ={0,1,0,1,-1,1};
float edge_length = 0.1;

static PerformanceTimer timer;

GLuint DLpoint;
GLuint DLedge;
GLuint DLtriangle;

GLuint createPoint();
GLuint createEdge();
GLuint createTriangle();

FILE* outputfile;

unsigned int test_mode;
unsigned int test_type;
unsigned int point_type;
unsigned int edge_type;
unsigned int triangle_type;


void CalcFPS()
{
	static int timeinit=0;
	static int frame=0, frame2=0;
	static PerformanceTimer pTimer;
	if(!timeinit)
	{
		frame=0;
		pTimer.StartTimer();	
		timeinit=1;
	}
	frame++;
	double dif = pTimer.GetTimeElapsed();
	double fps = (dif)?(double)frame/dif:-1.0;
	if(dif>2.0 && frame >10) 
	{
		pTimer.StartTimer();
		frame  = 0;
	}
	sprintf(title,"FPS:%4.2f",fps);
	glutSetWindowTitle(title);
}

void changeSize(int w, int h)	{
	// Set the viewport to be the entire window
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

 
void initVBO()
{
	unsigned int num_vertex;
	unsigned int num_list;
	float* vertexPtr;
	unsigned int* indicePtr;

	if(test_type ==1)
	{
		num_vertex = number_elements*3*sizeof(float);
		num_list = number_elements*sizeof(unsigned int);
		vertexPtr = vertex_ptr.vertex_ptr;
		indicePtr = vertex_ptr.indice;
	}
	else if(test_type ==2)
	{
		num_vertex = number_elements*6*sizeof(float);
		num_list = number_elements*2*sizeof(unsigned int);
		vertexPtr = edge_ptr.vertex_ptr;
		indicePtr = edge_ptr.indice;
	}
	else if(test_type ==3)
	{
		num_vertex = number_elements*9*sizeof(float);
		num_list = number_elements*3*sizeof(unsigned int);
		vertexPtr = triangle_ptr.vertex_ptr;
		indicePtr = triangle_ptr.indice;
	}

	
	glGenBuffers(1, &vboVertex); 
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glBufferData(GL_ARRAY_BUFFER, num_vertex, vertexPtr, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vboList); // Generate buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboList); // Bind the element array buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_list, indicePtr, GL_STATIC_DRAW);
}

void deleteVBO()
{
	glDeleteBuffers(1, &vboVertex );
	glDeleteBuffers(1, &vboList );
}

void generateData()
{
	srand (time(NULL));

	if(test_type ==1)
	{
		vertex_ptr.vertex_ptr = new float[3*number_elements];
		vertex_ptr.indice = new unsigned int[number_elements];
		for(unsigned int i=0;i<number_elements;i++)
		{
			vertex_ptr.vertex_ptr[3*i] = (float)rand()/(float)RAND_MAX;
			vertex_ptr.vertex_ptr[3*i+1] = (float)rand()/(float)RAND_MAX;
			vertex_ptr.vertex_ptr[3*i+2] = 0.0f;
			vertex_ptr.indice[i] = i;
			 
		}
	}
	else if(test_type ==2)
	{
		edge_ptr.vertex_ptr =  new float[6*number_elements];
		edge_ptr.indice = new unsigned int[2*number_elements];
		for(unsigned int i=0;i<number_elements;i++)
		{
			edge_ptr.vertex_ptr[3*i] = 0.5-edge_length*0.5; 
			edge_ptr.vertex_ptr[3*i+1] = (float)rand()/(float)RAND_MAX;
			edge_ptr.vertex_ptr[3*i+2] = 0.0f;
			edge_ptr.vertex_ptr[3*i +3*number_elements] = 0.5+edge_length*0.5; 
			edge_ptr.vertex_ptr[3*i+1 + 3*number_elements] = (float)rand()/(float)RAND_MAX;
			edge_ptr.vertex_ptr[3*i+2 +3*number_elements] = 0.0f;

			edge_ptr.indice[2*i] = rand()%number_elements;
			edge_ptr.indice[2*i+1] = rand()%number_elements + number_elements;
		}
	}
	else if(test_type ==3)
	{
		triangle_ptr.indice = new unsigned int[3*number_elements];
		triangle_ptr.vertex_ptr = new float[9*number_elements];

		for(unsigned int i=0;i<number_elements;i++)
		{
			triangle_ptr.vertex_ptr[9*i] = (float)rand()/(float)RAND_MAX; 
			triangle_ptr.vertex_ptr[9*i+1] = (float)rand()/(float)RAND_MAX;
			triangle_ptr.vertex_ptr[9*i+2] = 0.0f;
			triangle_ptr.vertex_ptr[9*i+3] = (float)rand()/(float)RAND_MAX; 
			triangle_ptr.vertex_ptr[9*i+4] = (float)rand()/(float)RAND_MAX;
			triangle_ptr.vertex_ptr[9*i+5] = 0.0f;
			triangle_ptr.vertex_ptr[9*i+6] = 0.0f;
			triangle_ptr.vertex_ptr[9*i+7] = 0.0f;
			triangle_ptr.vertex_ptr[9*i+8] = 0.0f;

			triangle_ptr.indice[3*i] = rand()%number_elements;
			triangle_ptr.indice[3*i+1] = rand()%number_elements + number_elements;
			triangle_ptr.indice[3*i+2] = rand()%number_elements + number_elements*2;
		}
	}
}

void freeData()
{
	printf("exit and destory the data");
	delete [] vertex_ptr.vertex_ptr;
	delete [] vertex_ptr.indice;
	delete [] edge_ptr.indice;
	delete [] edge_ptr.vertex_ptr;
	delete [] triangle_ptr.indice;
	delete [] triangle_ptr.vertex_ptr;

}

GLuint createPoint()
{
	GLuint vertexDL;
	vertexDL = glGenLists(1);
	glNewList(vertexDL,GL_COMPILE);
	glPointSize(10.0f);
	glColor3f( 0.95f, 0.207, 0.031f );

	if(point_type == 2)
	{
		glEnable( GL_POINT_SMOOTH );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glPointSize( 6.0 );
	}

	glBegin( GL_POINTS );
	for(unsigned int i=0;i<number_elements;i++)
		glVertex3f(vertex_ptr.vertex_ptr[3*i],vertex_ptr.vertex_ptr[3*i+1],vertex_ptr.vertex_ptr[3*i+2]);
	glEnd();
	glEndList();
	return vertexDL;
}

GLuint createEdge()
{
	GLuint edgeDL;
	edgeDL = glGenLists(1);

	float vertex1[3];
	float vertex2[3];

	glNewList(edgeDL,GL_COMPILE);
	glColor3f( 0.95f, 0.207, 0.031f );

	if(edge_type == 1)
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
 		glEnable(GL_NORMALIZE);
	}

	glBegin( GL_LINES );
	for(unsigned int i=0;i<number_elements;i++)
	{
		vertex1[0] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i] ];
		vertex1[1] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i] +1 ];
		vertex1[2] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i] +2];

		vertex2[0] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i+1] ];
		vertex2[1] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i+1] +1];
		vertex2[2] = edge_ptr.vertex_ptr[ 3*edge_ptr.indice[2*i+1] +2];

		glVertex3f(vertex1[0],vertex1[1],vertex1[2]);
		glVertex3f(vertex2[0],vertex2[1],vertex2[2]);
	}
	glEnd();
	glEndList();

	return edgeDL;
}

GLuint createTriangle()
{
	GLuint triangleDL;
	triangleDL = glGenLists(1);

	float vertex1[3];
	float vertex2[3];
	float vertex3[3];

	glNewList(triangleDL,GL_COMPILE);
	glColor3f( 0.95f, 0.207, 0.031f );

	if(triangle_type == 1)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	glBegin( GL_TRIANGLES  );
	for(unsigned int i=0;i<number_elements;i++)
	{
		vertex1[0] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i] ];
		vertex1[1] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i] +1 ];
		vertex1[2] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i] +2];

		vertex2[0] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+1] ];
		vertex2[1] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+1] +1];
		vertex2[2] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+1] +2];

		vertex3[0] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+2] ];
		vertex3[1] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+2] +1];
		vertex3[2] = triangle_ptr.vertex_ptr[ 3*triangle_ptr.indice[3*i+2] +2];

		glVertex3f(vertex1[0],vertex1[1],vertex1[2]);
		glVertex3f(vertex2[0],vertex2[1],vertex2[2]);
		glVertex3f(vertex3[0],vertex3[1],vertex3[2]);
	}
	glEnd();
	glEndList();

	return triangleDL;
}


void renderScene(void) {
	
	glViewport (0, 0, width, height );
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho( m_Ortho[0], m_Ortho[1], m_Ortho[2], m_Ortho[3], m_Ortho[4], m_Ortho[5] );
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(test_mode == 1)
	{
		glPointSize(5.0f); 
		glColor4f(0.0,0.0,1.0,1.0 );
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
		glVertexPointer(3,GL_FLOAT,0,0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboList);
	
		if(test_type ==1)
		{
			if(point_type == 2)
			{
				glEnable( GL_POINT_SMOOTH );
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glPointSize( 6.0 );
			}
			glDrawElements(GL_POINTS, number_elements, GL_UNSIGNED_INT, NULL);
		}
		else if(test_type ==2)
		{
			if(edge_type == 1)
			{
				glEnable(GL_LINE_SMOOTH);
				glEnable(GL_BLEND);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
				glDepthFunc(GL_LEQUAL);
				glEnable(GL_DEPTH_TEST);
 				glEnable(GL_NORMALIZE);
			}
			glDrawElements(GL_LINES, number_elements*2, GL_UNSIGNED_INT, NULL);
		}
		else if(test_type ==3)
		{
			if(triangle_type == 1)
			{
				glPolygonMode(GL_FRONT, GL_LINE);
				glPolygonMode(GL_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
			}
			glDrawElements(GL_TRIANGLES, number_elements*3, GL_UNSIGNED_INT, NULL);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
	}


	else if(test_mode ==2)
	{
		if(test_type == 1)
			glCallList(DLpoint);
		else if(test_type == 2)
			glCallList(DLedge);
		else if(test_type == 3)
			glCallList(DLtriangle);
	}
	
	CalcFPS(); 
	glutSwapBuffers();
}




/*use can specify the different rendering mode and types*/
void InitParameter()
{
	printf("Test Mode: 1:VBO Test 2:Display List\n");
	scanf("%d",&test_mode);
	printf("Test Type: 1:Points 2:Edges 3:Triangles\n");
	scanf("%d",&test_type);
	printf("The number of Points( Edges or Triangles)\n");
	scanf("%d",&number_elements);
	
	assert((test_mode==1)||(test_mode==2));
	assert((test_type==1)||(test_type==2)||(test_type==3));
	assert(number_elements>0);

	if(test_type ==1)
	{
		printf("Points are rendered as 1: rectangle 2:circle \n");
		scanf("%d",&point_type);
	}
	else if(test_type ==2)
	{
		printf("The edges are smoothed or not? 1:smoothed 2:not smoothed\n");
		scanf("%d",&edge_type);
		printf("The length of the edges: range:(0-1)\n");
		scanf("%f",&edge_length);
	}
	else if(test_type ==3)
	{
		printf("The triangle mode: 1:wireframe 2:filled\n");
		scanf("%d",&triangle_type);
	}
}


void InitGL()
{
	glClearColor( 1.0,1.0,1.0,1.0 );
	glClearDepth(1.0);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf( "%s\n", glewGetErrorString(err) );

	if ( GLEW_ARB_vertex_buffer_object )
		printf( "VBO extersion is supported!\n");
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		printf("Ready for GLSL - vertex, fragment, and geometry units.\n");
	else {
		printf("Not totally ready :( \n");
	}

	glClearColor( 1.0,1.0,1.0,1.0);
	glClearDepth(1.0);
 
	vboVertex = -1;
	vboList = -1;	

	if(test_mode == 1)
		initVBO();
	else if(test_mode == 2)
	{
		if(test_type == 1)
			DLpoint = createPoint();
		else if(test_type == 2)
			DLedge = createEdge();
		else if(test_type == 3)
			DLtriangle = createTriangle();
	}
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
	glutCreateWindow("Display Test");


	atexit (freeData);
	InitParameter();
	generateData();
	InitGL();
	

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutMainLoop();

	return(0);
}
