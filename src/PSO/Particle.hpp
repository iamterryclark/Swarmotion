//
//  Particle.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#include "ofMain.h"
#include "ofxMaxim.h"

class Particle : public SIPoint {
private:
    vector<double> vel;
    vector<double> pBest;
    double pBestFitness = 0.0;
    double gBestFitness = numeric_limits<float>::min();

public:
    Particle(int _dims) : SIPoint(_dims) {
        for (int d = 0; d < _dims; d++){
            vel.push_back(0);
        }
        pBest = vel;
    }
    
    vector<double> getVel(){
        return vel;
    }
    
    void setVel(vector<double> _newVel){
        vel = _newVel;
    }
    
    void setVel(int i, double _newVel){
        vel[i] = _newVel;
    }
    
    void setPBest(vector<double> _newPBest){
        pBest = _newPBest;
    }
    
    vector<double> getPBest(){
        return pBest;
    }
    
    void setPBestFitness(double _newPBestFitness){
        pBestFitness = _newPBestFitness;
    }
    
    double getPBestFitness(){
        return pBestFitness;
    }
    
    void setGBestFitness(double _newGBestFitness){
        gBestFitness = _newGBestFitness;
    }
    
    double getGBestFitness(){
        return gBestFitness;
    }
};
