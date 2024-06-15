

#ifndef _CAMERA_INCLUDED_
#define _CAMERA_INCLUDED_

#ifndef _STRUCTS_INCLUDED_
#include "structs.h"
#endif


//there is only one camera at any instance, so instead of functions procedures are used here
// Camera should change in place to be the global object
float FOV = 90;
struct Camera_ {	// 
    V3 pos;          //position
    float beta; 		//horisontal angle
    float alpha;		//vertical angle, there is no roll
    float dist=1.5;
    
    // result of orientation calculation, 3 relative vectors
    V3 forw;   //from y
    V3 right; //from x
    V3 up; //from z
    
    //float w=FOV;//angles of the FOV
    //float h=FOV;
};
typedef struct Camera_ Camera;

Camera camera;

void report(){
	printf("cam: %f,%f,%f  %f %f\n", camera.pos.x, camera.pos.y, camera.pos.z, camera.alpha, camera.beta);
}


float fov = M_PI/2;
float scaling = 100;

V3 x_const = {1,0,0};
V3 y_const = {0,1,0};
V3 up_const = {0,0,1};

V3 rotateAroundAxis(V3 p, V3 axis, float angle) {
    // Normalize the axis vector
    axis = normalise(axis);
    float ux, uy, uz;
    ux = axis.x;
    uy = axis.y;
    uz = axis.z;

    // Calculate cosine and sine of the angle
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);
    // Calculate one minus cosine
    float oneMinusCosTheta = 1.0f - cosTheta;

    // Calculate the rotation matrix components
    float r00 = cosTheta + ux * ux * oneMinusCosTheta;
    float r01 = ux * uy * oneMinusCosTheta - uz * sinTheta;
    float r02 = ux * uz * oneMinusCosTheta + uy * sinTheta;

    float r10 = uy * ux * oneMinusCosTheta + uz * sinTheta;
    float r11 = cosTheta + uy * uy * oneMinusCosTheta;
    float r12 = uy * uz * oneMinusCosTheta - ux * sinTheta;

    float r20 = uz * ux * oneMinusCosTheta - uy * sinTheta;
    float r21 = uz * uy * oneMinusCosTheta + ux * sinTheta;
    float r22 = cosTheta + uz * uz * oneMinusCosTheta;
	
    float newX = r00 * p.x + r01 * p.y + r02 * p.z;
    float newY = r10 * p.x + r11 * p.y + r12 * p.z;
    float newZ = r20 * p.x + r21 * p.y + r22 * p.z;

    // return
    V3 result;
    result.x = newX;
    result.y = newY;
    result.z = newZ;
    return result;
}
void calc_orient(){ //use alpha beta to calculte orientation vectors, matices
	// horisontal rotation
	camera.forw = rotateAroundAxis(y_const, up_const, camera.beta);
	camera.right = rotateAroundAxis(x_const, up_const, camera.beta);
	
	//vertical
	camera.forw = rotateAroundAxis(camera.forw, camera.right, camera.alpha);
	camera.up = rotateAroundAxis(up_const, camera.right, camera.alpha);
}

void move(Direction dir){
	calc_orient();
	float ms = 0.05; //ms = movement speed
	switch (dir) {
    case LEFT:
        camera.pos += ms * (- camera.right);
        break;
    case RIGHT:
        camera.pos += ms * ( camera.right);
        break;
    case UP:
        camera.pos += ms * ( up_const );
        break;
    case DOWN:
        camera.pos += ms * ( -up_const );
        break;
    case FORWARD:
        camera.pos += ms * ( camera.forw);
        break;
    case BACKWARD:
        camera.pos += ms * (- camera.forw);
        break;
    default:
        
        break;
	}
}
void turn(Direction dir){
	
	float rs = 0.03;

	switch (dir) {
    	case LEFT:
        	camera.beta += rs;
        	break;
    	case RIGHT:
        	camera.beta -= rs;
        	break;
    	case UP:
        	camera.alpha += rs;
        	break;
    	case DOWN:
        	camera.alpha -= rs;
        	break;
    	case FORWARD:
        
        	break;
    	case BACKWARD:
        
        	break;
    	default:
        
        	break;
	}
	if(camera.alpha > M_PI/2 ) {
		camera.alpha = M_PI/2;
	}
	if(camera.alpha < -M_PI/2 ) {
		camera.alpha = -M_PI/2;
	}
	if(camera.beta > M_PI*2 ) {
		camera.beta -= M_PI*2;
	}
	if(camera.beta < 0 ) {
		camera.beta += M_PI*2;
	}
	calc_orient();
}
void rot(Direction dir){
	float rs = 0.03;
	switch (dir) {
    	case LEFT:
        	camera.beta += rs;
        	break;
    	case RIGHT:
        	camera.beta -= rs;
        	break;
    	case UP:
        	camera.alpha += rs;
        	break;
    	case DOWN:
        	camera.alpha -= rs;
        	break;
    	case FORWARD:
        	//camera.dist -= 1;
        	break;
    	case BACKWARD:
        	//camera.dist += 1;
        	break;
    	default:
        
        	break;
	}
	
	
	if(camera.alpha > M_PI/2 ) {
		camera.alpha = M_PI/2;
	}
	if(camera.alpha < -M_PI/2 ) {
		camera.alpha = -M_PI/2;
	}
	if(camera.beta > M_PI*2 ) {
		camera.beta -= M_PI*2;
	}
	if(camera.beta < 0 ) {
		camera.beta += M_PI*2;
	}
	
	camera.pos = camera.dist*((V3){0,-1,0});
	calc_orient();
	camera.pos = rotateAroundAxis(camera.pos, up_const, camera.beta);
	camera.pos = rotateAroundAxis(camera.pos, camera.right, camera.alpha);
	
}

Pixel project_point(const Point& P){
	calc_orient();
	//depending on the current mode find coordinates on the drawing plane
	Pixel result;
	V3 relative = P.pos - camera.pos;
	
	float coord1, coord2;
	int c1,c2;
	if(Pmode){ //spherical
		float phi_hor = relative ^ camera.right;
		float phi_vert = relative ^ camera.up;
	
		//float coord1 = - phi_hor * (180/M_PI) * (DRAW_WIDTH / FOV) + DRAW_WIDTH/2;
		//float coord2 = - phi_vert * (180/M_PI) * (DRAW_WIDTH / FOV) + DRAW_WIDTH/2;
		float alpha = (180-FOV)/2;
		coord1 = (M_PI - phi_hor) * (180/M_PI) - alpha ;
		coord2 = (phi_vert) * (180/M_PI) - alpha ;//  + DRAW_WIDTH/2; //causes freeze
		
		//here is the magic + DRAW_WIDTH/2; replace with 400 or 200 and it somehow starts working!
		c1 = static_cast<int>(coord1);
		c2 = static_cast<int>(coord2) + 200;
		c1 = c1 * 3;
		c2 = c2 * 3;
		
	} else { //flat works
		coord1 = (relative*camera.right) / len(camera.right);
		coord2 = (relative*camera.up) / len(camera.up);
		
		coord1 *= scaling ;
		coord2 *= scaling ;
		
		c1 = static_cast<int>(coord1) + DRAW_WIDTH/2; //  + DRAW_WIDTH/2;
		c2 = static_cast<int>(coord2) + DRAW_WIDTH/2;

	}

	result.x = c1;
	result.y = c2;
	//result.color = P.color; //##
	return result;
}

Point project_point2(const Point& P){
	calc_orient();
	//depending on the current mode find coordinates on the drawing plane
	Point result;
	V3 relative = P.pos - camera.pos;
	
	float coord1, coord2;
	float c1,c2;
	if(Pmode){ //spherical
		float phi_hor = relative ^ camera.right;
		float phi_vert = relative ^ camera.up;
	
		//float coord1 = - phi_hor * (180/M_PI) * (DRAW_WIDTH / FOV) + DRAW_WIDTH/2;
		//float coord2 = - phi_vert * (180/M_PI) * (DRAW_WIDTH / FOV) + DRAW_WIDTH/2;
		float alpha = (180-FOV)/2;
		coord1 = (M_PI - phi_hor) * (180/M_PI) - alpha ;
		coord2 = (phi_vert) * (180/M_PI) - alpha ;//  + DRAW_WIDTH/2; //causes freeze
		
		//here is the magic + DRAW_WIDTH/2; replace with 400 or 200 and it somehow starts working!
		c1 = floor(coord1);
		c2 = floor(coord2) + 200;
		c1 = c1 * 5;
		c2 = c2 * 5;
		
	} else { //flat works
		coord1 = (relative*camera.right) / len(camera.right);
		coord2 = (relative*camera.up) / len(camera.up);
		
		coord1 *= scaling *2;
		coord2 *= scaling *2;
		
		c1 = coord1 + DRAW_WIDTH/2; //  + DRAW_WIDTH/2;
		c2 = coord2 + DRAW_WIDTH/2;

	}

	result.pos.x = c1;
	result.pos.y = c2;
	result.color = P.color; //##
	return result;
}


void drawLine( Point start_float, Point end_float, COLORREF color){
	//Pixel start = convertPointToPixel(start_float);
	//Pixel end = convertPointToPixel(end_float);
	Pixel start = project_point(start_float);
	Pixel end = project_point(end_float);
	
	dda2(  start, end, color);
	
};
void calculate_distances(){
	for (Point& point : allpoints) {
		//point.dist = len(point.pos - camera.pos);
	};
	//printf("dists calculated\n");
}

void drawSegment(  Segment s, COLORREF color){ drawLine(  s.start, s.finish, color); };
void drawSegments(  Segments f, COLORREF color){
	for (const Segment& segment : f) {
		drawSegment(  segment, color);
	};
}

bool looksatme(Face face){
	V3 A = face.A->pos;
	V3 B = face.B->pos;
	V3 C = face.C->pos;
	V3 av = 1.0/3*(A+B+C);
	V3 potnorm = crossProduct(B-A,C-B);
	if(potnorm*av<0) potnorm = -potnorm;
	if(
		potnorm*((Pmode) ? camera.pos-av : camera.pos) >= 0
	) {
		return true;
	} else {
		return false;
	}
	
	
}

void drawFace(Face face){
	Contour cont1 = FaceToContour(face);
	Segments f = convertContourToSegments(cont1 );
	if(Dmode){
		for (Point& point : cont1) {
			point = project_point2(point);
		}
		if(looksatme(face)){
			fill_triangle(cont1, cont1);
		}
	}else{
		drawSegments(  f, main_color);
	}
	
	
}
void calculate_colors();
void drawScene(){
	//for each triangle call painter
	InitializeBuffer(); //clean color buffer
	resetZBuffer(); // clear depth buffer
	calculate_colors();
	//calculate_distances();
	
	Contour cont1;
	Segments f;
	if(Dmode){
		for (const Face& face : scene) {
        	cont1 = FaceToContour(face);
			for (Point& point : cont1) {
				point = project_point2(point);
			}
			if(!looksatme(face)){fill_triangle(cont1, cont1);}
    	};
    	for (const Face& face : scene) {
        	cont1 = FaceToContour(face);
			for (Point& point : cont1) {
				point = project_point2(point);
			}
			if(looksatme(face)){fill_triangle(cont1, cont1);}
    	}
	}else{
		for (const Face& face : scene) {
        	Contour cont1 = FaceToContour(face);
			Segments f = convertContourToSegments(cont1 );
			drawSegments(  f, main_color);
    	}
	}
	
}

#endif // CAMERA_INCLUDED