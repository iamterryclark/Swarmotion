//
//  DFO.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Fly.hpp"

class DFO{
private:
public:
    vector<Fly *> swarm;
    int bestIndex = 0;
    double bestFitness = std::numeric_limits<float>::max();;
    double sound;
    
    ofParameterGroup parameters;
    ofParameter<float> dt;
    ofParameter<int> topology;
    
    DFO(int population){
        for (int i = 0 ; i < population; i++){
            swarm.push_back(new Fly(2));
            swarm[i]->setPos(0, ofRandom(120));
            swarm[i]->setPos(1, ofRandom(90));
        }
        
        parameters.setName("DFO Params");
        parameters.add(dt.set("Disturbance", 0.01, 0.0, 0.1));
        parameters.add(topology.set("Topology", 0, 0, 2));
    }
    
    void find(ofPixels &searchSpace){
        
        for (auto & fly : swarm){
            //Set the fitness of each fly based on image colour
            ofColor ssData = searchSpace.getColor( fly->getPos()[0], fly->getPos()[1] );
            ofVec2f velXY = ofVec2f(ssData.r, ssData.g);
            
            fly->setFitness( highestVel( velXY ) );
            
            //cout << fly.getFitness() << endl;
        }
        
        bestFitness = std::numeric_limits<float>::max();
        
        int index = 0;
        for (auto & fly : swarm){
            //Find the best fly in the swarm
            if (fly->getFitness() <= bestFitness){
                bestFitness = fly->getFitness();
                bestIndex = index;
                //cout << fly.getFitness() << endl;
            }
            index++;
        }
        
        index = 0;
        //Find Neighbours to each current fly
        int leftN = 0;
        int rightN = 0;
        
        for (auto & fly : swarm){
            
            switch(topology){
                case 0://Ring Topology
                    
                    //Create a ring of flies so that the last in the array is neighboured to the first fly
                    leftN = index - 1;
                    rightN = index + 1;
                    
                    if (index == 0)
                        leftN = (int)(swarm.size() - 1);
                    
                    if (index == (int)(swarm.size() - 1))
                        rightN = 0;
                    
                    topology.setName("Ring Topology");
                    break;
                case 1: //Random Topology
                    leftN = (int)ofRandom(swarm.size());
                    if (leftN == index)
                        leftN = (int)ofRandom(swarm.size());
                    
                    rightN = (int)ofRandom(swarm.size());
                    if (rightN == index || rightN == leftN)
                        rightN = (int)ofRandom(swarm.size());
                    topology.setName("Random Topology");
                    break;
                    
                case 2: //Closest neighbour
                    double minDL = 10E15;
                    for (int i = 0; i < swarm.size(); i++) {
                        if (i == index)
                            continue;
                        
                        double d = fly->getDistance(swarm[i]->getPos());
                        if (d < minDL) {
                            minDL = d;
                            leftN = index;
                        }
                    }
                    
                    double minDR = 10E15;
                    for (int i = 0; i < swarm.size(); i++) {
                        if (i == index)
                            continue;
                        if (i == leftN)
                            continue;
                        
                        double d = fly->getDistance(swarm[i]->getPos());
                        if (d < minDR) {
                            minDR = d;
                            rightN = i;
                        }
                    }
                    topology.setName("Closest Topology");
                    break;
            }
            
            //Get the fitnesses to compare which of the neighbouring flies has the best fitness
            double leftFit = swarm[leftN]->getFitness();
            double rightFit = swarm[rightN]->getFitness();
            
            Fly bestNeighbour = (swarm[leftN]->getFitness() < swarm[rightN]->getFitness()) ? *swarm[leftN] : *swarm[rightN];
            
            //        cout << "L :" << leftN << " "
            //             << "i :" << index << " "
            //             << "R :" << rightN << " " <<
            //        endl;
            
            vector<double> newPositions;
            
            //Disturb the flies to get a better value than before if a random number is below a threshold
            if (ofRandom(1.0) < dt){
                //GetRandom Position in the search space
                newPositions.push_back( ofRandom(searchSpace.getWidth()-1) );
                newPositions.push_back( ofRandom(searchSpace.getHeight()-1) );
            } else {
                //Let the search begin!
                //The next position is based on this formula
                //The best Neighbouring position + a random value from 0 to 1 multiplied by the best fly in the swarm take away the current position
                newPositions.push_back(bestNeighbour.getPos()[0] + ofRandom(1.0) * (swarm[bestIndex]->getPos()[0] - fly->getPos()[0]));
                newPositions.push_back(bestNeighbour.getPos()[1] + ofRandom(1.0) * (swarm[bestIndex]->getPos()[1] - fly->getPos()[1]));
            }
            
            newPositions[0] = ofClamp(newPositions[0], 1, searchSpace.getWidth()-1);
            newPositions[1] = ofClamp(newPositions[1], 1, searchSpace.getHeight()-1);
            
            //Make sure the flies stay within search space
            fly->setPos(newPositions);
            
            index++;
        }
    }
    
    void render(ofPixels &searchSpace){
        ofPushStyle();
        for(int i = 0; i < swarm.size(); i++) {
            
            float mappedPosX = ofMap(swarm[i]->getPos()[0], 1, searchSpace.getWidth()-1, 0, 480);
            float mappedPosY = ofMap(swarm[i]->getPos()[1], 1, searchSpace.getHeight()-1, 0, 360);
            
            if(bestIndex == i){
                ofSetColor(255,0,0);
                ofDrawEllipse(200 + mappedPosX, mappedPosY, 10,10);
                ofSetColor(255,0,0);
                ofNoFill();
                ofDrawRectangle(200 + mappedPosX-15, mappedPosY-15, 30,30);
            } else {
                ofSetColor(255);
                ofFill();
                ofDrawEllipse(200 + mappedPosX, mappedPosY, 5,5);
            }
        }
        ofPopStyle();
    }
    
    double highestVel(ofVec2f _velocity){
        //2 Part Fitness function
        float a = 255 - _velocity.x;
        float b = 255 - _velocity.y;
        //cout << a << " " << b << endl;
        return (a + b)/2;
    }
    
    double output(){
        sound = 0.0;
        
        for (int i = 0 ; i < swarm.size(); i++){
            ofVec2f dist = swarm[i]->getDistance2Vec(swarm[bestIndex]->getPos());
            float bestPosX = swarm[bestIndex]->getPos()[0];
            float bestFitness = swarm[bestIndex]->getFitness();
            
            dist.x = ofMap(dist.x, 0, ofGetWidth(),  10, 400);
            dist.y = ofMap(dist.y, 0, ofGetHeight(), 1, 100);
            
            sound += swarm[i]->output(dist.x, dist.y);
        }
        
        sound /= swarm.size();
        sound = ofClamp(sound, -1, 1);
        
        return sound;
    }
    
    void keypressed(int key){
        switch(key){
                //        case 'r':
                //            swarm.clear();
                //            for (int i = 0 ; i < swarm.size(); i++){
                //                swarm.push_back(Fly());
                //                swarm[i]->setPos( { ofRandom(ofGetWidth()),  ofRandom(ofGetHeight()) } );
                //                swarm[i]->setFitness( bestFitness );
                //            }
                //            break;
        }
    }
    

};
