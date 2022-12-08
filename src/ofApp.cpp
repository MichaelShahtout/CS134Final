
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
    
    ofDisableArbTex(); // disable rectangular textures
    //Load textures
    if(!ofLoadImage(particleTex, "images/dot.png")) {
        cout << "Particle Texture file: images/dots.png not found " << endl;
        ofExit();
    }
    //Load the Shader
#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/shader");
#else
    shader.load("shaders/shader");
#endif
	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/mars-low-5x-v2.obj");
	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = false;

	//  Create Octree for testing.
	//
	
    
    float t1 = ofGetElapsedTimeMillis(); // Start time
    octree.create(mars.getMesh(0), 20);
    float t2 = ofGetElapsedTimeMillis(); // End time
    //cout << "Time to Create Octree: " << t2 - t1 << " millisec" << endl; // subtracted time is the creation time
	
	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));
    
    //Setup lander so we don't have to drag it in every time
    
    if (lander.loadModel("geo/lander.obj")) {
        bLanderLoaded = true;
        lander.setScaleNormalization(false);
        lander.setPosition(0, 25, 0); // just made the lander 25 pixels in the air.
        cout << "number of meshes: " << lander.getNumMeshes() << endl;
        bboxList.clear();
        for (int i = 0; i < lander.getMeshCount(); i++) {
            bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
        }

            glm::vec3 min = lander.getSceneMin();
            glm::vec3 max = lander.getSceneMax();

            // set up bounding box for lander while we are at it
            landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
    } else{
        cout << "Error loading model: geo/lander.obj" << endl;
        ofExit(0);
    }
    
    //Forces
    gravityForce = new GravityForce(ofVec3f(0,-10,0));
    radialForce = new ImpulseRadialForce(1000.0);
    turbForce = new TurbulenceForce(ofVec3f(-2,0), ofVec3f(2,0)); // used for the movement
    radialForce->setHeight(10);
    thruster = new ThrusterForce();
    
    exhaust.sys->addForce(gravityForce); // want the particles to always flow down
   // exhaust.sys->addForce(new GravityForce(ofVec3f(0,-10,0)));
    exhaust.setGroupSize(25);
    exhaust.setLifespan(1.5);
    exhaust.setVelocity(ofVec3f(0,2,0));
    exhaust.setOneShot(true);
    exhaust.setParticleRadius(0.1);
    //exhaust.setEmitterType(RadialEmitter);
    exhaust.setEmitterType(DiscEmitter);
    //exhaust->setRate();
    
   
    //explosion. haven't tested this yet, just basic idea
     explosion = new ParticleEmitter();
     explosion->setGroupSize(25);
     explosion->setLifespan(10);
     explosion->setOneShot(true);
     explosion->setParticleRadius(1);
     explosion->sys->addForce(new TurbulenceForce(glm::vec3(-300, -300, 0), glm::vec3(300, 300, 0)));
     explosion->setEmitterType(RadialEmitter);
     
    
    //add particle for movement. so basically the lander follows the particle for movement
    particle.lifespan = 9999999; // never dies! because we need this for movement
    particle.position = ofVec3f(0,25,0); // this is the lander position. so just made it 25 above the starting point
    gravityRotation = new GravityForce( ofVec3f(0,10,0));
    particle.color = ofColor::black; 
    
    //Movement
    gravityForce = new GravityForce(ofVec3f(0,-0.1,0)); // change the gravity so its slowly falling down
    movement = new ParticleSystem();
    movement->addForce(turbForce);
    movement->addForce(gravityForce);
    movement->addForce(thruster); // thruster movement wasd-qe
    movement->add(particle); // The particle is the driving force for the movement.
    
    
    
    //Sound  - BLANK FOR NOW
    /*
    if(!backgroundSound.load("")){
        cout << "Can't open background sound " << endl;
        ofExit(0);
    }
     
    if(!explosionSound.load("")){
        cout << "Can't open explosion sound file" << endl;
        ofExit(0);
    }
    if(!thrusterSound.load("")){
        cout << "Can't open thruster sound file" << endl;
        ofExit(0);
    }
     */
    
    //Lights -- this is just copy and paste from our lab 8. will change it once we see how the model fits lighting 
    /*
     ofEnableDepthTest();
     ofEnableLighting();

     // Setup 3 - Light System
     //
     keyLight.setup();
     keyLight.enable();
     keyLight.setAreaLight(1, 1);
     keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
     keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
     keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
     
     keyLight.rotate(45, ofVec3f(0, 1, 0));
     keyLight.rotate(-45, ofVec3f(1, 0, 0));
     keyLight.setPosition(5, 5, 5);

     fillLight.setup();
     fillLight.enable();
     fillLight.setSpotlight();
     fillLight.setScale(.05);
     fillLight.setSpotlightCutOff(15);
     fillLight.setAttenuation(2, .001, .001);
     fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
     fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
     fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
     fillLight.rotate(-10, ofVec3f(1, 0, 0));
     fillLight.rotate(-45, ofVec3f(0, 1, 0));
     fillLight.setPosition(-5, 5, 5);

     rimLight.setup();
     rimLight.enable();
     rimLight.setSpotlight();
     rimLight.setScale(.05);
     rimLight.setSpotlightCutOff(30);
     rimLight.setAttenuation(.2, .001, .001);
     rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
     rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
     rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
     rimLight.rotate(180, ofVec3f(0, 1, 0));
     rimLight.setPosition(0, 5, -7);
     */
     
    
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadVbo() {
    if (exhaust.sys->particles.size() < 1) return;

    vector<ofVec3f> sizes;
    vector<ofVec3f> points;
    for (int i = 0; i < exhaust.sys->particles.size(); i++) {
        points.push_back(exhaust.sys->particles[i].position);
        sizes.push_back(ofVec3f(10)); // particle radius
    }
    // upload the data to the vbo
    //
    int total = (int)points.size();
    vbo.clear();
    vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
    vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
    exhaust.update();
    exhaust.setPosition(lander.getPosition()); // exhaust follows lander
    
    movement->update();
    
    //Move the lander based on the particle ( if statement to make sure particle exists)
    if(!(movement->particles.empty())){
        //cout << "Moved" << endl;
        
        lander.setPosition(movement->particles[0].position.x, movement->particles[0].position.y, movement->particles[0].position.z);
    }
    //cameras
    if(trackingCamera){
        cam.setPosition(lander.getPosition().x + 25, lander.getPosition().y, lander.getPosition().z);
        cam.lookAt(lander.getPosition());
    }
    
    //Leaving blank for now till we find a cool pos with the model
    if(onBoardCamera){
        ;
    }
    if(topCamera){
        ;
    }
    
    //Rotation
    if(bRotation){
        //This is implemented via Forces, so the longer you hold it, the faster it spins! 
        particle.angularForce = 10;
        float angle = particle.rotation; // this is the rotation angle we will turn with
        particle.integrateAngular(); // function for angular motion
        //cout << ofRadToDeg(rotationHold) << endl; // used as a check
        
        int numRotation = lander.getNumRotations(); // we need this so we can add another rotation on top of the already applied rotation
        lander.setRotation(numRotation,angle,0,1,0); // rotate along Y axis
  
    }
    
    //Gravity needs to be an adjustable force 
    
    
    
    
	
}
//--------------------------------------------------------------
void ofApp::draw() {
    loadVbo();
	ofBackground(ofColor::black);

	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);

	cam.begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					//ofRotate(-90, 1, 0, 0); // dont need this
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
                ofNoFill(); // DOn't want the fat rectangle around the box
				Octree::drawBox(bounds);

				// draw colliding boxes - This is the terrain to rover box collision
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}

	ofPopMatrix();
	cam.end();
    
    glDepthMask(GL_FALSE);
    ofSetColor(255,100,90);
    //Makes everything glowy
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    //Begin drawing in the camera
    shader.begin();
    cam.begin();
    //Draw particle Emitter here
    exhaust.draw(); // draw then pop.
    particleTex.bind();
    vbo.draw(GL_POINTS, 0, (int)exhaust.sys->particles.size());
    particleTex.unbind();
    //end drawing in the camera
    cam.end();
    shader.end();
    ofDisablePointSprites();
    ofDisableBlendMode();
    ofEnableAlphaBlending();
    //Set back depth mask
    glDepthMask(GL_TRUE); 
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 10,15); // top left

    
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {
	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 'p': // This was orginally 's'
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'z': // This was orginally 'w'
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
    //3D space has 6 movements
    case 'w': // up
            //Play the sound
            thrusterSound.setLoop(true);
            thrusterSound.play();
            exhaust.start(); // going up will trigger the exhaust
            thruster->setVelocity(ofVec3f(0,2,0));
        break;
    case 'a': // left
        thruster->setVelocity(ofVec3f(-2,0,0));
        break;
    case 's': // down
        thruster->setVelocity(ofVec3f(0,-2,0));
        break;
    case 'd': // right
        thruster->setVelocity(ofVec3f(2,0,0));
        break;
    case 'q': // forward
        thruster->setVelocity(ofVec3f(0,0,2));
        break;
    case 'e': // backwards
        thruster->setVelocity(ofVec3f(0,0,-2));
        break;
    case 'y': // Rotation
            bRotation = true;
            break;
    case '1': // camera position will be wherever the lander is
            cam.setPosition(lander.getPosition());
            break;
    case '2':
            if (trackingCamera)
                trackingCamera = false;
            else
                trackingCamera = true;
            break;
    case '3':
            if (onBoardCamera)
                onBoardCamera = false;
            else
                onBoardCamera = true;
            break;
    case '4':
                if (topCamera)
                    topCamera = false;
                else
                    topCamera = true;
                break;
    
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
    case 'w': // up
            thrusterSound.stop();
        exhaust.start(); // going up will trigger the exhaust
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 'a': // left
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 's': // down
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 'd': // right
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 'q': // forward
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 'e': // backwards
        thruster->setVelocity(ofVec3f(0,0,0));
        break;
    case 'y':
            bRotation = false;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

    float t1 = ofGetElapsedTimeMicros();
    //cout << "T1 " << t1 << endl;
    pointSelected = octree.intersect(ray, octree.root, selectedNode);
    float t2 = ofGetElapsedTimeMicros();
    //cout << "T2 " << t2  << endl;
    //cout << "Time to Select: " << t2 - t1 << " microseconds" << endl; // THIS IS COMMENTED OUT FOR THE VIDEO.
	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points.at(0));
	}
    
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}*/


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
