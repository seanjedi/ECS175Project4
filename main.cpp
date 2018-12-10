#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector> 
#include <cmath>
#include <string>
#include "Bresenham.h"

using namespace std;

////////////////////
//Global Variables//
////////////////////
float *PixelBuffer;
string inputFile;
char quit, all, write;
int windowSizeX = 800, windowSizeY = 800, style, curveCount, curveMode, currentID, resolution;
float viewXmin, viewXmax, viewYmin, viewYmax, deltaX, deltaY, Delta;
ifstream inFile;


//Vertex Struct
struct Vertex
{
	float x;
	float y;
};

//////////////////////////
//Function Defininitions//
//////////////////////////
void display();
void getSettings();
void getSettings2();
void boundBox();
void setBoundaryBox();
void makePixel(int x, int y, float* PixelBuffer, int mode);
Vertex toNDCtoPixel(float x, float y, float z);


//Define a Boundary Box struct
struct Boundary
{
	float Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
};

Boundary boundaryBox{0,0,0,0,0,0};

//Find max function
float max(float a, float b) {
	if (a > b)
		return a;
	else
		return b;
}

//Find min function
float min(float a, float b) {
	if (a < b)
		return a;
	else
		return b;
}
//Bresenham line drawing
void drawBresenham(int x1, int y1, int x2, int y2, int mode) {
	Bresenham(x1, x2, y1, y2, PixelBuffer, windowSizeX, mode);
}
///////////////////////////////////
//Polygon Object Class Definition//
///////////////////////////////////
class curveObject {
public:
	int vertexCount;
	int order, knotCount;

	vector<Vertex> vertices;
	vector<Vertex> points;
	vector<int> knots;

	//Set count of vertices
	void setPoints(int x) {
		vertexCount = x;
	}

	//Set count of edges
	void setOrder(int x) {
		order = x;
	}

	//Add a new vertex to the vertice vertex
	void addVertex(float x, float y) {
		Vertex newVertex = { x, y};
		vertices.push_back(newVertex);
	}

	//Add a new vertex at location
	void AddPoint(float x, float y, int location) {
		Vertex newVertex = { x, y };
		vertices.insert(vertices.begin() + location, newVertex);
		vertexCount++;
	}

	//Remove a point
	void removePoint(int location) {
		vertices.erase(vertices.begin() + location);
		vertexCount--;
	}

	//Modify a point
	void modifyPoint(float x, float y, int location) {
		vertices[location].x += x;
		vertices[location].y += y;
	}
	


	//Draw Bezier Curves to the Pixel Buffer
	void drawBezier() {
		Vertex temp1, temp2;
		int edge1, edge2;
		vector<Vertex> output;
		
		//Draw control points
		for (int i = 0; i < vertices.size() - 1; i++) {
			temp1 = toNDCtoPixel(vertices[i].x, vertices[i].y, 1);
			temp2 = toNDCtoPixel(vertices[i + 1].x, vertices[i + 1].y, 1);
			drawBresenham(temp1.x, temp1.y, temp2.x, temp2.y, 0);
			//Make a square of 10x10 pixels to make control points pronounced
			for (int x = temp1.x - 5; x < temp1.x + 5; x++) {
				for (int y = temp1.y - 5; y < temp1.y + 5; y++) {
					makePixel(x, y, PixelBuffer, 0);
				}
			}
			for (int x = temp2.x - 5; x < temp2.x + 5; x++) {
				for (int y = temp2.y - 5; y < temp2.y + 5; y++) {
					makePixel(x, y, PixelBuffer, 0);
				}
			}
		}

		//Apply the De Casteljon algorithm
		//Have 2 temp values that switch off each time to have the change in generations work
		//The last generation's first value is the one we need to save
		bool flip = true;
		for (double t = 0; t <= 1; t += (1 / (double)resolution)) {
			vector<Vertex> tempA(vertices);
			vector<Vertex> tempB(vertices);
			for (int j = 1; j <= vertices.size(); j++) {
				for (int i = 0; i < vertices.size() - j; i++) {
					if (flip) {
						tempA[i].x = ((1 - t) * tempB[i].x) + (t * tempB[i + 1].x);
						tempA[i].y = ((1 - t) * tempB[i].y) + (t * tempB[i + 1].y);
					}
					else {
						tempB[i].x = ((1 - t) * tempA[i].x) + (t * tempA[i + 1].x);
						tempB[i].y = ((1 - t) * tempA[i].y) + (t * tempA[i + 1].y);
					}
				}
				if (flip)
					flip = false;
				else
					flip = true;
			}
			if (flip) {
				output.push_back(tempA[0]);
			}
			else {
				output.push_back(tempB[0]);
			}
		}

		//Draw out the ouput curve
		for (int i = 0; i < output.size() - 1; i++) {
			temp1 = toNDCtoPixel(output[i].x, output[i].y, 1);
			temp2 = toNDCtoPixel(output[i + 1].x, output[i + 1].y, 1);
			drawBresenham(temp1.x, temp1.y, temp2.x, temp2.y, 1);
		}

	}

	//Draw B-Spline Curves to the Pixel Buffer
	void drawSpline() {
		Vertex temp1, temp2;

		//Draw control points
		for (int i = 0; i < vertexCount - 1; i++) {
			temp1 = toNDCtoPixel(vertices[i].x, vertices[i].y, 1);
			temp2 = toNDCtoPixel(vertices[i + 1].x, vertices[i + 1].y, 1);
			drawBresenham(temp1.x, temp1.y, temp2.x, temp2.y, 0);
			//Make a square of 10x10 pixels to make control points pronounced
			for (int x = temp1.x - 5; x < temp1.x + 5; x++) {
				for (int y = temp1.y - 5; y < temp1.y + 5; y++) {
					makePixel(x, y, PixelBuffer, 0);
				}
			}
		}

		// Apply the De Boor algorithm
		//Have 2 temp values that switch off each time to have the change in generations work
		//The last generation's first value is the one we need to save
		bool flip = true;
		for (double t = 0; t <= 1; t += (1 / resolution)) {

		}
	}

	//Function to find a new boundary for each polyhedra
	Boundary getBoundary() {
		Boundary instance{0,0,0,0,0,0};
		for (int i = 0; i < vertexCount; i++) {
			instance.Xmax = max(instance.Xmax, vertices[i].x);
			instance.Ymax = max(instance.Ymax, vertices[i].y);
			instance.Xmin = min(instance.Xmin, vertices[i].x);
			instance.Ymin = min(instance.Ymin, vertices[i].y);
		}
		return instance;
	}

	//Write Polygon Buffer to output file!
	void writeBack(std::ofstream& file) {
		file << vertexCount << endl;
		for (int i = 0; i < vertexCount; i++) {
			file << vertices[i].x << " " << vertices[i].y << endl;
		}
		file << order << endl;
		for (int i = 0; i < knotCount; i++) {
			file << knots[i] << endl;
		}
	}

};

/////////////////////////////////
//Polygon Vector Initialization//
/////////////////////////////////
vector<curveObject> curves;

///////////////////////////
//Reset Pixel Buffer to 0//
///////////////////////////
void setPixelBuffer(float* PixelBuffer) {
	for (int i = 0; i < windowSizeX; i++) {
		for (int j = 0; j < windowSizeY; j++) {
			PixelBuffer[((windowSizeX * j) + i) * 3 + 1] = 0;
			PixelBuffer[((windowSizeX * j) + i) * 3] = 0;
		}
	}
}



//////////////////////
//WriteBack Function//
//////////////////////
void writeBack(string inputFile) {
	//Write back to open file!
	ofstream myfile(inputFile);
	if (myfile.is_open()) {
		myfile << curveCount << endl;
		for (int i = 0; i < curveCount; i++) {
			myfile << endl;
			curves[i].writeBack(myfile);
		}
	}
	else {
		cout << "Error! Unable to open file!";
		exit(-1);
	}
	myfile.close();
}


/////////////////
//Main Function//
/////////////////
int main(int argc, char *argv[])
{
	//allocate new pixel buffer, need initialization!!
	getSettings();
	setBoundaryBox();
	PixelBuffer = new float[windowSizeX * windowSizeY * 3];
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	//set window size to windowSizeX by windowSizeX
	glutInitWindowSize(windowSizeX, windowSizeY);
	//set window position
	glutInitWindowPosition(100, 100);

	//create and set main window title
	int MainWindow = glutCreateWindow("Hello Graphics!!");
	glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
	//sets display function
	while (1) {
		display();
		cout << "Would you like to write to a file? (y/n)\nChooice: ";
		cin >> write;
		if (write == 'y') {
			cout << "Specify ouput File: ";
			cin >> inputFile;
			writeBack(inputFile);
		}

		cout << "Would you like to quit? (y/n)\nChooice: ";
		cin >> quit;
		if (quit == 'y') {
			exit(0);
		}
		getSettings2();
	}
	
	//glutDisplayFunc(display);

	glutMainLoop();//main display loop, will display until terminate
	return 0;
}



///////////////////////
//Make Pixel Function//
///////////////////////

void makePixel(int x, int y, float* PixelBuffer, int mode)
{
	//Make sure it is within range
	if (x > 0 && x < windowSizeX && y > 0 && y < windowSizeY) {
		if (mode == 0) {
			PixelBuffer[((windowSizeX * y) + x) * 3] = 1;
		}
		else {
			PixelBuffer[((windowSizeX * y) + x) * 3 + 1] = 1;
		}
	}
		
}

/////////////////////////////////////////////////////////////////////////////
//main display loop, this function will be called again and again by OpenGL//
/////////////////////////////////////////////////////////////////////////////
void display(){

	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	setPixelBuffer(PixelBuffer);

	if (curveMode == 0) {
		for (int i = 0; i < curveCount; i++)
			curves[i].drawBezier();
	}
	else {
		for (int i = 0; i < curveCount; i++)
			curves[i].drawSpline();
	}

	
	//draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(windowSizeX, windowSizeY, GL_RGB, GL_FLOAT, PixelBuffer);

	//window refresh
	glFlush();
}

//////////////////////////////
//Handles File Input//////////
//Handles Initial User Input//
//////////////////////////////
void getSettings(){
	string space;
	int vertexCount, order, point1, point2;
	float x, y;
	//Ask how many curves they want
	cout << "How many curves do you want: ";
	cin >> curveCount;
	curves.resize(curveCount);
	for (int i = 0; i < curveCount; i++) {
		cout << "curve " << i + 1 << endl;
		cout << "  How many Control points do you want: ";
		cin >> vertexCount;
		curves[i].setPoints(vertexCount);
		for (int j = 0; j < vertexCount; j++) {
			cout << "  Point " << j << endl;
			cout << "    X: ";
			cin >> x;
			cout << "    Y: ";
			cin >> y;
			curves[i].addVertex(x, y);
		}
		cout << "  Order(k value): ";
		cin >> order;
		curves[i].setOrder(order);
	}
	//Go to polyhedra Menu
	getSettings2();

}

void getSettings2() {
	int choice = 4;
	//Ash which transformation you want
	cout << "What would you like to do next?\n1) Display Curves\n2) Add Control Point\n3) Delete Control Point\n4) Modify Control Point\nChoose:";
	cin >> choice;
	while (choice < 1 || choice > 5) {
		cout << "Wrong Choice, Please choose a possible Action!\nChoose:";
		cin >> choice;
	}

	//Ask which polyhedra you want to manipulate!
	if(choice != 1) {
		cout << "Which Curve would you like to manipulate? (id's 1 to " << curveCount << ")\nID:";
		cin >> currentID;
		while (currentID <= 0 || currentID > curveCount) {
			cout << "Wrong ID, Please choose a possible ID!\nID:";
			cin >> currentID;
		}
	}
	//Add Control Point
	if (choice == 2) {
		int loc;
		float x, y;
		cout << "Where would you like to add the point? (from values 0 to " << curves[currentID - 1].vertexCount << ")\nLocation:";
		cin >> loc;
		while (loc < 0 || loc > curves[currentID - 1].vertexCount) {
			cout << "Wrong Choice, Please choose a possible Location!\nLocation:";
			cin >> loc;
		}
		cout << "X: ";
		cin >> x;
		cout << "Y: ";
		cin >> y;
		curves[currentID - 1].AddPoint(x, y, loc-1);
		setBoundaryBox();
	}
	//Delete Control point
	else if (choice == 3) {
		int loc;
		cout << "Which point would you like to delete? (from values 1 to " << curves[currentID - 1].vertexCount << ")\nLocation:";
		cin >> loc;
		while (loc < 1 || loc > curves[currentID - 1].vertexCount) {
			cout << "Wrong Choice, Please choose a possible Location!\nLocation:";
			cin >> loc;
		}
		curves[currentID - 1].removePoint(loc-1);
		setBoundaryBox();
	}
	//Modify Control Point
	else if (choice == 4) {
		int loc;
		float x, y;
		cout << "Which point would you like to modify? (from values 1 to " << curves[currentID - 1].vertexCount << ")\nLocation:";
		cin >> loc;
		while (loc < 1 || loc > curves[currentID - 1].vertexCount) {
			cout << "Wrong Choice, Please choose a possible Location!\nLocation:";
			cin >> loc;
		}
		cout << "Please choose a new X and Y value: " << endl;
		cout << "  X: ";
		cin >> x;
		cout << "  Y: ";
		cin >> y;
		curves[currentID - 1].modifyPoint(x, y, loc-1);
		setBoundaryBox();
	}

	//Ask for resolution
	cout << "What Resolution would you like to use\nchoice: ";
	cin >> resolution;
	while (resolution < 0 ) {
		cout << "Wrong Choice, Please choose a possible value!\nChoose:";
		cin >> resolution;
	}

	//Ask for method
	cout << "Would you like to use 1)Bezier or 2)B-Splines\nchoice: ";
	cin >> curveMode;
	while (curveMode < 1 || curveMode > 2) {
		cout << "Wrong Choice, Please choose a possible Action!\nChoose:";
		cin >> curveMode;
	}
	//Did this decrement because I wanted the value to be either 0 or 1
	curveMode--;
	return;
}

/////////////////////////////////////////////////////////////////////////////////
//Sets Screen to divide it by 4 differnt quadrants, and adds which one is which//
/////////////////////////////////////////////////////////////////////////////////

//Set a new boundary box if values are outside the current range!
void setBoundaryBox() {
	Boundary temp;
	for (int i = 0; i < curveCount; i++) {
		//Get the boundaries of the new values
		temp = curves[i].getBoundary();
		//Check if the new values cause a new boundary, if so change boundary Box size
		//Add or subtract 10% to have some space between the screen edges
		if (max(boundaryBox.Xmax, temp.Xmax) > boundaryBox.Xmax) {
			boundaryBox.Xmax = temp.Xmax;
		}
		if (max(boundaryBox.Xmax, temp.Ymax) > boundaryBox.Ymax) {
			boundaryBox.Ymax = temp.Ymax;
		}
		if (min(boundaryBox.Xmin, temp.Xmin) < boundaryBox.Xmin) {
			boundaryBox.Xmin = temp.Xmin;
		}
		if (min(boundaryBox.Ymin, temp.Ymin) < boundaryBox.Ymin) {
			boundaryBox.Ymin = temp.Ymin;
		}
	}
	//Create new values for deltas
	deltaX = boundaryBox.Xmax - boundaryBox.Xmin;
	deltaY = boundaryBox.Ymax - boundaryBox.Ymin;
	//Take max of the deltas
	Delta = max(deltaX, deltaY);
}

//Turn world to NDC then to Pixel
Vertex toNDCtoPixel(float x, float y, float z) {
	Vertex point{ 0,0 };
	float xNDC, yNDC;

	xNDC = (x - boundaryBox.Xmin) / Delta;
	yNDC = (y - boundaryBox.Ymin) / Delta;
	point.x = int(xNDC * (windowSizeX - 30));
	point.y = int(yNDC * (windowSizeY - 30));
	point.y += 10;
	point.x += 10;
	return point;
}