#pragma once
//  Kevin M. Smith - CS 134 SJSU
// Added a thruster force

#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void addForce(ParticleForce *);
	void remove(int);
	void update();
	void setLifespan(float);
	void reset();
	int removeNear(const ofVec3f & point, float dist);
	void draw();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
};



// Some convenient built-in forces
//
class GravityForce: public ParticleForce {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(Particle *);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	void updateForce(Particle *);
};

class ImpulseRadialForce : public ParticleForce {
	float magnitude;
    float height;
public:
	ImpulseRadialForce(float magnitude);
	void updateForce(Particle *);
    ImpulseRadialForce setHeight(float new_height) { height = new_height;} // to control the clamping in the "Y" values via slider
};
class ThrusterForce : public ParticleForce {
    float magnitude = 1.0;
    ofVec3f thruster;
    
public:
    ThrusterForce(){thruster = ofVec3f(0,0,0);} // start with 0 thrust force
    void updateForce(Particle *);
    void add(ofVec3f movement) { thruster += movement; } // This is used to move the lander
    void setVelocity(ofVec3f velocity) { thruster = velocity;} // To set a thrust speed
};
