//reader
#ifndef _READER_INCLUDED_
#define _READER_INCLUDED_


void read_config(){
	std::ifstream configFile("config.txt");
    if (!configFile) {
        std::cerr << "Error: Unable to open config.txt" << std::endl;
        return;
    }
    
    camera.pos = {0,-1.5,0};
	camera.beta = 0.1; // grows towards left
	camera.alpha = 0.1; // grows towards up
	camera.dist = 1.5;

    std::string line;
    int lineCount = 0;
    int colorr,colorg,colorb;
    while (std::getline(configFile, line)) {
        if (lineCount % 2 == 1) {
        	std::stringstream ss(line);
            switch (lineCount) {
                case 1:
                    ss >> Pmode;
                    break;
                case 3:
                    ss >> Dmode;
                    break;
                case 5:
                    ss >> camera.pos.x >> camera.pos.y >> camera.pos.z;
                    break;
                case 7:
                    ss >> light.pos.x >> light.pos.y >> light.pos.z;
                    break;
                case 9:
                	
                    ss >> colorr >> colorg >> colorb;
                    light.color = RGB(colorb,colorg,colorr);
                    break;
                case 11:
                    
                    ss >> colorr >> colorg >> colorb;
                    main_color = RGB(colorb,colorg,colorr);
                    break;
                case 13:
                    ss >> colorr >> colorg >> colorb;
                    diffusion = RGB(colorb,colorg,colorr);
                    break;
                case 15:
                    ss >> colorr >> colorg >> colorb;
                    mirror = RGB(colorb,colorg,colorr);
                    break;
                case 17:
                    ss >> colorr >> colorg >> colorb;
                    ambience = RGB(colorb,colorg,colorr);
                    break;
                case 19:
                    ss >> mirror_sharp;
                    break;
                case 21:
                    ss >> amb_coef;
                    break;
            }
        }
        lineCount++;
    }
    //configFile.close(); //problem line, without it better
    printf("Config is read\n");   
}

std::string filepath1 = "files/cube.vtk";
std::string filepath2 = "files/sphere_octo1.vtk";
std::string filepath3 = "files/sphere_octo3.vtk";
std::string filepath4 = "files/sphere_octo5.vtk";

void calculate_normals(){
	for (Point& point : allpoints) {
		point.normal = normalise(point.pos);
	};
	printf("normals calculated");
}

V3 calculateReflection(const V3& light, const V3& normal) {
	//light defines path towards light
    float dotProduct = (-light)*normal;
    V3 reflection = (-light) - ((2 * dotProduct) * normal);
    return reflection;
}
int limitTo0To255(float value) {
    if (value < 0.0f) {
        return 0;
    } else if (value > 255.0f) {
        return 255;
    } else {
        // Convert the value to an int
        return static_cast<int>(value);
    }
}
COLORREF extract_color(Point point){
	
	V3 v = normalise(camera.pos - point.pos);
	V3 n = point.normal;
	V3 l = normalise(light.pos - point.pos);
	V3 r = calculateReflection(l,n);
	
	int lred,lgreen,lblue;
	
	ExtractRGBComponents(light.color, &lred, &lgreen, &lblue);
	float lredf=lred/255.0, lgreenf=lgreen/255.0, lbluef=lblue/255.0;
	
	int mr,mg,mb;
	ExtractRGBComponents(mirror, &mr, &mg, &mb);
	float mrf=mr/255.0, mgf=mg/255.0, mbf=mb/255.0;
	
	int dr,dg,db;
	ExtractRGBComponents(diffusion, &dr, &dg, &db);
	float drf=dr/255.0, dgf=dg/255.0, dbf=db/255.0;
	
	int ar,ag,ab;
	ExtractRGBComponents(ambience, &ar, &ag, &ab);
	float arf=ar/255.0, agf=ag/255.0, abf=ab/255.0;
	
	
	float mircoef = pow(v*r,mirror_sharp);
	float Isr = lredf*mrf*mircoef;
	float Isg = lgreenf*mgf*mircoef;
	float Isb = lbluef*mbf*mircoef;
	
	if(v*r < 0 or l*n<0){
		Isr = 0; Isg = 0; Isb = 0; 
	}
	
	float difcoef = l*n;
	if (difcoef<0) difcoef = 0;
	float Idr = lredf*drf*difcoef;
	float Idg = lgreenf*dgf*difcoef;
	float Idb = lbluef*dbf*difcoef;
	
	float Iar = arf*drf;
	float Iag = agf*dgf;
	float Iab = abf*dbf;
	
	float red = Isr + Idr + Iar;
	float green = Isg + Idg + Iag;
	float blue = Isb + Idb + Iab;
	
	 int redcolor = limitTo0To255(red*255);
	  int greencolor = limitTo0To255(green*255);
	   int bluecolor = limitTo0To255(blue*255);
	   
	return RGB(redcolor, greencolor, bluecolor);
	
}
void calculate_colors(){
	for (Point& point : allpoints) {
		point.color = extract_color(point);
	};
	printf("colors calculated");
}

void readVtkFile(const std::string& filepath, Allpoints& allpoints, Scene& scene) {
	if (!allpoints.empty()) {
        allpoints.clear();
    }

    if (!scene.empty()) {
        scene.clear();
    }
    
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filepath << std::endl;
        return;
    }
    
    std::string line;
    bool processingPoints = false;
    bool processingFaces = false;
    
    std::vector<Point*> pointRefs;
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string keyword;
        ss >> keyword;

        // Skip comments and empty lines
        if (keyword.empty() || keyword[0] == '#') continue;

        if (keyword == "POINTS") {
            processingPoints = true;
            processingFaces = false;
            int numPoints;
            ss >> numPoints; 
            std::string type;
            ss >> type; // Type  float

            // Read points
            for (int i = 0; i < numPoints; i++) {
                if (std::getline(file, line)) {
                    std::stringstream pointStream(line);
                    Point p;
                    if (!(pointStream >> p.pos.x >> p.pos.y >> p.pos.z)) {
                    	std::cerr << "Error reading point coordinates on line " << (i + 1) << std::endl;
                    	return;
                	}
                    p.color = main_color; // object color
                    allpoints.push_back(p);
                    pointRefs.push_back(&allpoints.back()); // Store reference to point, might not be needed
                }
            }
        } else if (keyword == "POLYGONS") {
            processingPoints = false;
            processingFaces = true;
            printf("Starting to read polygons\n");
            int numPolygons;
            int numVertices;
            ss >> numPolygons >> numVertices; // Number of polygons and total vertices (ignored)

            // Read faces
            for (int i = 0; i < numPolygons; i++) {
                if (getline(file, line)) {
                    std::stringstream faceStream(line);
                    int vertexCount; // Always 3 for a triangle face
                    faceStream >> vertexCount;

                    if (vertexCount == 3) { // check if file is correct
                        Face face;
                        int indexA, indexB, indexC;
                        faceStream >> indexA >> indexB >> indexC;
						//if(debug)printf("%d %d %d", indexA, indexB, indexC);
						
                        // Set pointers to points from allpoints
						if (indexA >= 0 && indexA < pointRefs.size() &&
    						indexB >= 0 && indexB < pointRefs.size() &&
    						indexC >= 0 && indexC < pointRefs.size()) {

    						face.A = pointRefs[indexA];
    						face.B = pointRefs[indexB];
    						face.C = pointRefs[indexC];

    						// Add face to scene
    						scene.push_back(face);

    						// Get the address of the newly added face in the scene vector
    						//Face* addedFace = &scene.back(); //this useless line prevented from crashing?
    						
    						
							//if(debug)printf("Face added\n");
							
    						// Update point facePtrs vectors, now points have pointers to the added face
    						//if (face.A) addedFace->A->facePtrs.push_back(addedFace);
    						//if (face.B) addedFace->B->facePtrs.push_back(addedFace);
    						//if (face.C) addedFace->C->facePtrs.push_back(addedFace);
    						
    						//if(debug)printf("Points updated\n");
						} else {
    						std::cerr << "Index out of bounds for pointRefs vector." << std::endl;
						}
                        //if(debug)printf("Points got their pointers\n");
                    } else {
                    	printf("Line did not declare 3 vertices\n");
					}
                }
            }
        }
    }

    file.close();
    printf("Vtk is read \n");
    calculate_normals();
    calculate_colors();
}




#endif // reader_INCLUDED