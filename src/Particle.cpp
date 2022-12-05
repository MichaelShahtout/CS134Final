//Add shaders!!!
#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position.set(0, 0, 0);
	forces.set(0, 0, 0);
	lifespan = 5;
	birthtime = 0;
	radius = .1;
	damping = .99;
	mass = 1;
	color = ofColor::aquamarine; //  This is the blue dot under the thruster. possibly make it same color as the exhaust OR as background? so we don't see it
    
    
    //Rotation
    //rotation = 10;
    //"Add an additional integrator for rotation in "Y" ??
    
}

void Particle::draw() {
	//Particles must be rendered using a shader, otherwise it will be too slow.
    // we will have to change something in the draw for the shaders.
    ofSetColor(ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255)));
	ofDrawSphere(position, radius);
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {

	// interval for this step
	//
    //Because framerate starts off at 0. need to hardcode value of 60 instead of using framerate of 0, or else it won't move!!!!! 
    float dt;
    if(ofGetFrameRate() < 1){
        dt = 1.0 / 60;
    }
    else
        dt = 1.0 / ofGetFrameRate();

	// update position based on velocity
	//
	position += (velocity * dt);

	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
    
	velocity += accel * dt;

	// add a little damping for good measure
	//
	velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
}

//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/1000.0;
}


