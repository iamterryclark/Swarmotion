//
//  Particle.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#include "ofMain.h"
#include "ofxMaxim.h"

class Particle {
public:
    ofVec2f pos;
    ofVec2f vel;
    ofVec2f personalBest;
    double fitness = 0.0;
    double pBestFitness = 0.0;
    double gBestFitness = numeric_limits<float>::min();
    maxiOsc osc;
    
    Particle(){
        pos = ofVec2f(0,0);
        vel = ofVec2f(0,0);
        personalBest = vel;
    }
    
    double getDistance(ofVec2f globalBestPos){
        return ofDistSquared(pos.x, pos.y, globalBestPos.x, globalBestPos.y);
    }
    
    ofVec2f getDistance2vec(ofVec2f globalBestPos){
        double xDist = pos.x - globalBestPos.x;
        double yDist = pos.x - globalBestPos.y;
        return ofVec2f(xDist, yDist);
    }
    
    double output(double param1, double param2){
//        switch(type){
//            case 0:
            return osc.sinewave(param1*param2);
//            break;
//        }
    }
    
    
};
