// int round = (float +0.5) 
#include <math.h>
#include <iostream>
using namespace std;

template <typename T>
struct point{
T x,y;};

class shape
{
public:
float x,y;
float theta; // Location 
virtual void drawOutline(unsigned char * image,int width,int height)=0;
virtual bool isValidLocation(unsigned char * image,int width,int height)=0;
virtual void setLocation(float x, float y, float theta){this->x=x;this->y=y;this->theta=theta;}
shape(float x,float y, float theta){this->x=x;this->y=y;this->theta=(theta/180.0)*M_PI;}
shape(){}
};

class Line:public shape{
float L;
float N;// Number of sample points
	public:
point<int> start;
point<int> end;
Line(){};
Line(float x, float y,float theta, float L):shape(x,y,theta){
this->L=L; this->N=L*5; 
start.x=x;
start.y=y; 
end.x=L*cos(this->theta)+start.x+0.5;
end.y=L*sin(this->theta)+start.y+0.5;
}

void Set(float x, float y, float theta, int L){

this->x=x; 
this->y=y; 
this->theta=(theta/180.0)*M_PI;
this->L=L; 
this->N=L*5;

start.x=x;
start.y=y;
end.x=L*cos(this->theta)+start.x+0.5;
end.y=L*sin(this->theta)+start.y+0.5;}

virtual bool isValidLocation(unsigned char * image,int width,int height){
float x_increment = L*cos(theta)/N;
float y_increment = L*sin(theta)/N;
int x_plot = x;
int y_plot = y;
for(int i=0; i<N;i++){
	x_plot=x_increment*i+x+0.5;
	y_plot=y_increment*i+y+0.5;
	if (image[y_plot*width+x_plot]==0){return 0;}
}
return 1;

}
void drawOutline(unsigned char * image,int width,int height){
float x_increment = L*cos(theta)/N;
float y_increment = L*sin(theta)/N;
int x_plot = x;
int y_plot = y;
for(int i=0; i<N;i++){
	x_plot=x_increment*i+x+0.5;
	y_plot=y_increment*i+y+0.5;
	image[y_plot*width+x_plot]=0;
}
	
}
};




class Arc:shape{
float arc_angle;
float radius;
float N;
	public:
Arc(){};
Arc(int x, int y,float theta, float radius,float arc_angle):shape(x,y,theta){
this->arc_angle=arc_angle; this->radius=radius;this->N=arc_angle*radius*5;
}
void Set(int x, int y, float theta, float radius, float arc_angle){
	this->x=x; this->y=y; 
	this->theta=(theta/180.0)*M_PI;
	this->radius=radius;
	this->arc_angle=(arc_angle/180.0)*M_PI;
	this->N=arc_angle*radius*5; }
virtual bool isValidLocation(unsigned char * image,int width,int height){
float radian_increment = arc_angle/N;
float radians = this->theta;
int x_plot,y_plot; 
for(int i=0; i<N;i++){
x_plot = x+radius*cos(radians+radian_increment*i) +0.5;
y_plot = y+radius*sin(radians+radian_increment*i)+0.5;
	if (image[y_plot*width+x_plot]==0){return 0;}
}
return 1;
}



void drawOutline(unsigned char * image,int width,int height){
float radian_increment = arc_angle/N;
float radians = this->theta;
int x_plot,y_plot; 
for(int i=0; i<N;i++){
x_plot = x+radius*cos(radians+radian_increment*i) +0.5;
y_plot = y+radius*sin(radians+radian_increment*i)+0.5;
	image[y_plot*width+x_plot]=0;
}

}
};
class rhombus: public shape
{
float corner_angle;
float width,height;
Line a,b,c,d;

public:
rhombus(){}
rhombus(int x, int y,float theta, int width, int height, float corner_angle):shape(x,y,theta){
this->theta=theta;
this->corner_angle=corner_angle;
this->width=width;
this->height=height;
a.Set(x,y,theta,width);
b.Set(a.end.x,a.end.y,this->theta+this->corner_angle,height);
c.Set(b.end.x,b.end.y,theta+180,width);
d.Set(c.end.x,c.end.y,this->theta+this->corner_angle+180,height);

}
void Set(int x, int y,float theta, int width, int height, float corner_angle){

this->theta=theta;
this->corner_angle=corner_angle;
this->width=width;
this->height=height;
a.Set(x,y,theta,width);
b.Set(a.end.x,a.end.y,this->theta+this->corner_angle,height);
c.Set(b.end.x,b.end.y,theta+180,width);
d.Set(c.end.x,c.end.y,this->theta+this->corner_angle+180,height);

}
void setParams(float width,float height,float corner_angle){
this->corner_angle=corner_angle;
this->width=width;
this->height=height;

}
void Init(){
a.Set(x,y,theta,width);
b.Set(a.end.x,a.end.y,this->theta+this->corner_angle,height);
c.Set(b.end.x,b.end.y,theta+180,width);
d.Set(c.end.x,c.end.y,this->theta+this->corner_angle+180,height);

}
virtual	void drawOutline(unsigned char * image,int width,int height){
Init();
a.drawOutline(image,width,height);
b.drawOutline(image,width,height);
c.drawOutline(image,width,height);
d.drawOutline(image,width,height);

}
virtual bool isValidLocation(unsigned char * image,int width,int height){
Init();
return a.isValidLocation(image,width,height) && 
       b.isValidLocation(image,width,height) && 
       c.isValidLocation(image,width,height) && 
       d.isValidLocation(image,width,height);
}
};
class lens:public shape
{
	Arc A;
	Arc B;
	int radius;
	float angle;
	int V;
	point<float> v_unit;
	float separation;
public:	
lens(){}
lens(int x, int y,float theta,float radius, float angle) :shape(x,y,theta){
this->radius=radius;
this->theta=theta;
this->angle=angle;
separation=2*radius*cos(angle/2);
v_unit.x=cos(theta+angle/2.0);
v_unit.y=sin(theta+angle/2.0);
A.Set(x,y,theta,angle,radius);
B.Set(x+v_unit.x*separation,y+v_unit.y*separation,theta+180,angle,radius);}
void Set(int x, int y,float theta,float radius, float angle){

this->radius=radius;
this->theta=theta;
this->angle=angle;


} 
void Init(){
separation=2*radius*cos(((angle/180.0)*M_PI)/2.0);
v_unit.x=cos((theta/180.0)*M_PI+((angle/180.0)*M_PI)/2.0);
v_unit.y=sin((theta/180.0)*M_PI+((angle/180.0)*M_PI)/2.0);
A.Set(x,y,theta,radius,angle);
B.Set(x+v_unit.x*separation,y+v_unit.y*separation,theta+180,radius,angle);



}
  void setParams(float radius,float arc_angle){
	this->radius=radius;
	this->angle=arc_angle;


  }
	void drawOutline(unsigned char * image,int width,int height){
	Init();
	A.drawOutline(image,width,height);
	B.drawOutline(image,width,height);
	}
	bool isValidLocation(unsigned char * image,int width,int height){
    Init();	
	return (A.isValidLocation(image,width,height)&&B.isValidLocation(image,width,height) );
	
	}
};

class capsule:public shape
{
	Arc A, B;
	Line C,D;
	int width;
	int height;
public:
    capsule(){};
	capsule(float x, float y, float theta,int width,int height){
		this->x=x;
		this->y=y;
		this->theta=theta;
		this->width=width;
		this->height=height;
		C.Set(x,y,(theta/180.0)*M_PI,width);
		D.Set(
		     	x+width*cos(  ((theta/180.0) *M_PI))-height*sin(((theta/180.0)*M_PI)),
		 	y+width*sin((theta/180.0)*M_PI)+height*cos(((theta/180.0)*M_PI)),
			theta+180,width);
		A.Set(
                         x + width*cos((theta/180.0)*M_PI) - height/2*sin((theta/180.0)*M_PI),
                         y + width*sin((theta/180.0)*M_PI) + height/2*cos((theta/180.0)*M_PI),
                         theta-90,height/2.0,180);
		B.Set(
                         x - height/2*sin((theta/180.0)*M_PI),
                         y + height/2*cos((theta/180.0)*M_PI),
                         theta+90,height/2.0,180);

	}
void Init(){
C.Set(x,y,theta,width);
		D.Set(
			x+width*cos(  ((theta/180.0) *M_PI))-height*sin(((theta/180.0)*M_PI)),
			y+width*sin((theta/180.0)*M_PI)+height*cos(((theta/180.0)*M_PI)),
			theta+180,width);

		A.Set( 
                        x + width*cos((theta/180.0)*M_PI) - height/2*sin((theta/180.0)*M_PI), 
                        y + width*sin((theta/180.0)*M_PI) + height/2*cos((theta/180.0)*M_PI),
                        theta-90,height/2.0,180);
		B.Set(
                        x - height/2*sin((theta/180.0)*M_PI),   
                        y + height/2*cos((theta/180.0)*M_PI),
                        theta+90,height/2.0,180);


}
void setParams(float width,float height){
		this->width=width;
		this->height=height;
}
	void drawOutline(unsigned char * image,int width,int height){
		Init();
	A.drawOutline(image,width,height);
	B.drawOutline(image,width,height);
	C.drawOutline(image,width,height);
	D.drawOutline(image,width,height);
	
	
	}
	bool isValidLocation(unsigned char * image,int width,int height){
		Init();
return (A.isValidLocation(image,width,height)&&
        B.isValidLocation(image,width,height)&& 
        C.isValidLocation(image,width,height)&&
        D.isValidLocation(image,width,height));
	
	}
};


