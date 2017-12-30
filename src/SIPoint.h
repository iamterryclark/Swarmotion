//
//  SIPoint.h
//  Swarmotion
//
//  Created by Terry Clark on 20/12/2017.
//
#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"

class SIPoint {
private:
    vector<double> pos;
    double fitness;
    int type, dims;
public:
    maxiOsc osc, osc2, phaser;
    maxiFilter filter, filter2;
    
    SIPoint(int _dims) : dims(_dims) {
        for (int d = 0; d < dims; d++){
            pos.push_back(0);
        }
        fitness = 0.1;
        type = 0;
    }
    
    void setDimensions(int _dims){
        dims = _dims;
    }
    
    int getDimensions(){
        return dims;
    }
    
    void setPos(vector<double> _newPos){
        pos = _newPos;
    }
    
    void setPos(int i, double _newPos){
        pos[i] = _newPos;
    }
    
    vector<double> getPos(){
        return pos;
    }
    
    void setFitness(double _newFitness){
        fitness = _newFitness;
    }
    
    double getFitness(){
        return fitness;
    }
    
    double getDistance(vector<double> point){
        return ofDistSquared(pos[0], pos[1], point[0], point[1]);
    }
    
    ofVec2f getDistance2Vec(vector<double> point){
        double xDist = pos[0] - point[0];
        double yDist = pos[1] - point[1];
        return ofVec2f(xDist, yDist);
    }

    double output(double param1, double param2, double param3){
        switch(type){
            case 0:
                return osc.sinewave((param1*param2)*param3);
            break;
        }
        return 0;
    }
};

