//
//  DFO.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Fly.hpp"
#include "SIAlgo.h"

class DFO : public SIAlgo {
private:
public:
    vector<Fly *> swarm;
    int bestIndex = 0;
    double bestFitness = std::numeric_limits<float>::max();
    double sound;
    vector<Fly> storedNeighbour;
    int isElite;

    DFO(int population){
        for (int i = 0 ; i < population; i++){
            swarm.push_back(new Fly(2));
            storedNeighbour.push_back(Fly(2));
            swarm[i]->setPos(0, ofRandom(0, 120));
            swarm[i]->setPos(1, ofRandom(0, 90));
        }
    }
    
    void find(ofPixels &searchSpace, int topology, float dt, int isElitist){
        
        for (auto & fly : swarm){
            //Set the fitness of each fly based on image colour
          
            ofVec2f velXY = getVelocity( searchSpace.getColor( fly->getPos()[0], fly->getPos()[1]) );
            
            fly->setFitness( highestVel( velXY ) );
            
            //cout << fly.getFitness() << endl;
        }
        
        bestFitness = swarm[0]->getFitness();
        
        int index = 0;
        for (auto & fly : swarm){
            //Find the best fly in the swarm
            if (fly->getFitness() < bestFitness){
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
                   
                    if (index == 0) {
                        leftN = (int)(swarm.size() - 1);
                        rightN = index + 1;
                    }
                    else if (index == (int)(swarm.size() - 1)){
                        leftN = index - 1;
                        rightN = 0;
                    } else {
                        rightN = index + 1;
                        leftN = index - 1;
                    }
                    break;
                case 1: //Random Topology
                    leftN = (int)ofRandom(swarm.size());
                    if (leftN == index)
                        leftN = (int)ofRandom(swarm.size());
                    
                    rightN = (int)ofRandom(swarm.size());
                    if (rightN == index || rightN == leftN)
                        rightN = (int)ofRandom(swarm.size());
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
                    break;
            }
            
            //Get the fitnesses to compare which of the neighbouring flies has the best fitness
            double leftFit = swarm[leftN]->getFitness();
            double rightFit = swarm[rightN]->getFitness();
            
            Fly bestNeighbour = (swarm[leftN]->getFitness() < swarm[rightN]->getFitness()) ? *swarm[leftN] : *swarm[rightN];
            
            storedNeighbour[index] = bestNeighbour;
            
            //        cout << "L :" << leftN << " "
            //             << "i :" << index << " "
            //             << "R :" << rightN << " " <<
            //        endl;
            
            vector<double> newPositions;
            //cout << dt << endl;
            //Disturb the flies to get a better value than before if a random number is below a threshold
            if (ofRandom(0, 1) < dt){
                //GetRandom Position in the search space
                newPositions.push_back( ofRandom(0, searchSpace.getWidth()) );
                newPositions.push_back( ofRandom(0, searchSpace.getHeight()) );
            } else {
                if (isElitist){
                    //Let the search begin!
                    //The next position is based on this formula
                    //The best Neighbouring position + a random value from 0 to 1 multiplied by the best fly in the swarm take away the current position
                    newPositions.push_back(bestNeighbour.getPos()[0] + ofRandom(1.0) * (swarm[bestIndex]->getPos()[0] - fly->getPos()[0]));
                    newPositions.push_back(bestNeighbour.getPos()[1] + ofRandom(1.0) * (swarm[bestIndex]->getPos()[1] - fly->getPos()[1]));
                } else if (!isElitist){
                    //Let the search begin!
                    //The next position is based on this formula
                    //The best Neighbouring position + a random value from 0 to 1 multiplied by the best fly in the swarm take away the current position
                    newPositions.push_back(fly->getPos()[0] + ofRandom(1.0) * (bestNeighbour.getPos()[0] - fly->getPos()[0]));
                    newPositions.push_back(fly->getPos()[1] + ofRandom(1.0) * (bestNeighbour.getPos()[1] - fly->getPos()[1]));
                }
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
            float mappedPosX = ofMap(swarm[i]->getPos()[0], 0, searchSpace.getWidth(), 0, 480);
            float mappedPosY = ofMap(swarm[i]->getPos()[1], 0, searchSpace.getHeight(), 0, 360);
            
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
    
  
    double output(){
        sound = 0.0;
        
        for (int i = 0 ; i < swarm.size(); i++){
            ofVec2f dist;
            double bestFitness;
            
            if (isElite){
                swarm[i]->getDistance2Vec(swarm[bestIndex]->getPos());
                bestFitness = swarm[bestIndex]->getFitness();
            } else {
                swarm[i]->getDistance2Vec(storedNeighbour[i].getPos());
                bestFitness = storedNeighbour[i].getFitness();
            }
            
            dist.x = ofMap(dist.x, 0, ofGetWidth(),  100, 300);
            dist.y = ofMap(dist.y, 0, ofGetHeight(), 10, 100);
            bestFitness = ofMap(bestFitness, 0, 255, 10, 0);
            
            sound += swarm[i]->output(dist.x, dist.y, bestFitness);
        }
        
        sound /= swarm.size();        
        return sound;
    }
};
