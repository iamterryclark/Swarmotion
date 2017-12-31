//
//  PSO.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#include "ofMain.h"
#include "Particle.hpp"
#include "SIAlgo.h"

class PSO : public SIAlgo {
private:
    vector<Particle *> particles;
    vector<double> globalBest = {0.0, 0.0};
    int bestIndex = 0;
    float sound;
    double X = 0.72984;
    double c1 = X * (2.1/2);
    double c2 = c1;
    
public:
    PSO(int population){
        for (int i = 0; i < population; i++){
            particles.push_back(new Particle(2));
            particles[i]->setPos(0, ofRandom(0, 120));
            particles[i]->setPos(1, ofRandom(0, 90));
        }
    }
    
    void find(ofPixels &searchSpace, int topology, float dt, int isElitest){
        
        int index = 0;
        for(auto & p : particles){
        
            // Calculate fitness value
            p->setFitness( highestVel( getVelocity(searchSpace.getColor(p->getPos()[0], p->getPos()[1])) ) );
            p->setPBestFitness( highestVel( getVelocity(searchSpace.getColor(p->getPBest()[0], p->getPBest()[1])) ) );
            p->setGBestFitness( highestVel( getVelocity(searchSpace.getColor(globalBest[0], globalBest[1])) ) );
            
            // If the fitness value is better than the best fitness value (pBest) in history
            // set current value as the new pBest
            if (p->getFitness() <= p->getPBestFitness() ){
                p->setPBest(p->getPos());
            }
            
            if (p->getPBestFitness() <= p->getGBestFitness() ){
                globalBest = p->getPBest();
                bestIndex = index;
            }
            index++;
        }
            
        index = 0;
        //Find Neighbours to each current fly
        int leftN = 0;
        int rightN = 0;
        
        for (auto & p : particles){
            switch(topology){
                case 0://Ring Topology
                    
                    //Create a ring of flies so that the last in the array is neighboured to the first fly
                    
                    if (index == 0) {
                        leftN = (int)(particles.size() - 1);
                        rightN = index + 1;
                    }
                    else if (index == (int)(particles.size() - 1)){
                        leftN = index - 1;
                        rightN = 0;
                    } else {
                        rightN = index + 1;
                        leftN = index - 1;
                    }
                    break;
                case 1: //Random Topology
                    leftN = (int)ofRandom(particles.size());
                    if (leftN == index)
                        leftN = (int)ofRandom(particles.size());
                    
                    rightN = (int)ofRandom(particles.size());
                    if (rightN == index || rightN == leftN)
                        rightN = (int)ofRandom(particles.size());
                    break;
                    
                case 2: //Closest neighbour
                    double minDL = 10E15;
                    for (int i = 0; i < particles.size(); i++) {
                        if (i == index)
                            continue;
                        
                        double d = p->getDistance(particles[i]->getPos());
                        if (d < minDL) {
                            minDL = d;
                            leftN = index;
                        }
                    }
                    
                    double minDR = 10E15;
                    for (int i = 0; i < particles.size(); i++) {
                        if (i == index)
                            continue;
                        if (i == leftN)
                            continue;
                        
                        double d = p->getDistance(particles[i]->getPos());
                        if (d < minDR) {
                            minDR = d;
                            rightN = i;
                        }
                    }
                    break;
            }
            
            //Get the fitnesses to compare which of the neighbouring flies has the best fitness
            double leftFit = particles[leftN]->getFitness();
            double rightFit = particles[rightN]->getFitness();
            
            Particle bestNeighbour = (particles[leftN]->getFitness() < particles[rightN]->getFitness()) ? *particles[leftN] : *particles[rightN];
        
            //Taken from the DFO algorithm of using a disturbance threshold to improve diversity of algorithm
            if (ofRandom(0, 1) < dt){
                p->setPos(0, ofRandom(0, searchSpace.getWidth()) );
                p->setPos(1, ofRandom(0, searchSpace.getHeight()) );
            } else {
                if (isElitest){
                    //Standard PSO Variation using contriction which will locate one optima and then become static
                    for (int d = 0; d < p->getDimensions(); d++){
                        p->setVel(d, X * p->getVel()[d] + c1 * ofRandom(1) * (p->getPBest()[d] - p->getPos()[d]) + c2 * ofRandom(1) * (globalBest[d] - p->getPos()[d]));
                        p->setPos(d, p->getPos()[d] + p->getVel()[d] );
                    }
                } else if (!isElitest){
                    //Standard PSO Variation using contriction which will locate one optima and then become static
                    for (int d = 0; d < p->getDimensions(); d++){
                        p->setVel(d, X * p->getVel()[d] + c1 * ofRandom(1) * (p->getPBest()[d] - p->getPos()[d]) + c2 * ofRandom(1) * (bestNeighbour.getPos()[d] - p->getPos()[d]));
                        p->setPos(d, p->getPos()[d] + p->getVel()[d] );
                    }
                }
            }
        
            p->setPos(0, ofClamp(p->getPos()[0], 1, searchSpace.getWidth()-1) );
            p->setPos(1, ofClamp(p->getPos()[1], 1, searchSpace.getHeight()-1) );
        }
    }

    void render(ofPixels &searchSpace){
        for(auto & p : particles){
            float mappedPosX = ofMap(p->getPos()[0], 1, searchSpace.getWidth()-1, 0, 480);
            float mappedPosY = ofMap(p->getPos()[1], 1, searchSpace.getHeight()-1, 0, 360);
            float mappedPBestPosX = ofMap(p->getPBest()[0], 1, searchSpace.getWidth()-1, 0, 480);
            float mappedPBestPosY = ofMap(p->getPBest()[1], 1, searchSpace.getHeight()-1, 0, 360);

            ofPushStyle();
                ofSetColor(255,0,0);
                ofDrawCircle(200 + mappedPosX, mappedPosY,  5);
            
                ofSetColor(0,0,255);
                ofDrawCircle(200 + mappedPBestPosX, mappedPBestPosY, 5);
            ofPopStyle();
        }
        
        float mappedGBestPosX = ofMap(globalBest[0], 1, searchSpace.getWidth()-1, 0, 480);
        float mappedGBestPosY = ofMap(globalBest[1], 1, searchSpace.getHeight()-1, 0, 360);
        
        ofPushStyle();
            ofSetColor(0,255,0);
            ofDrawCircle(200 + mappedGBestPosX , mappedGBestPosY, 5);
        ofPopStyle();
    }
    
    double output(){
        sound = 0.0;
        
        for (int i = 0 ; i < particles.size(); i++){
            ofVec2f dist = particles[i]->getDistance2Vec(globalBest);
            double bestFitness = particles[i]->getGBestFitness();
            
            dist.x = ofMap(dist.x, 0, ofGetWidth(),  100, 400);
            dist.y = ofMap(dist.y, 0, ofGetHeight(), 10, 100);
            bestFitness = ofMap(bestFitness, 0, 255, 0,100);
            
            sound += particles[i]->output(dist.x, dist.y, bestFitness);
        }
        
        sound /= particles.size();
        sound = ofClamp(sound, -1, 1);
        
        return sound;
    }
    
};
