//
//  PSO.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#include "ofMain.h"
#include "ofxGui.h"
#include "Particle.hpp"

class PSO {
private:
    vector<Particle *> particles;
    vector<double> globalBest = {0.0, 0.0};
    float sound;
    double X = 0.72984;
    double c1 = X * (2.1/2);
    double c2 = c1;
    
public:
    ofParameterGroup parameters;
    ofParameter<float> dt;
    
    PSO(int population){

        for (int i = 0; i < population; i++){
            particles.push_back(new Particle(2));
            particles[i]->setPos(0, ofRandom(0, 120));
            particles[i]->setPos(1, ofRandom(0, 90));
        }
        
        parameters.setName("PSO Params");
        parameters.add(dt.set("Disturbance", 0.001, 0.0, 0.1));
        //parameters.add(topology.set("Topology", 0, 0, 2));
    }
    
    void update(ofPixels &searchSpace){
        
        for(auto & p : particles){
            
            // Calculate fitness value
            p->setFitness( evaluate( searchSpace.getColor(p->getPos()[0], p->getPos()[1]) ) );
            p->setPBestFitness( evaluate( searchSpace.getColor(p->getPBest()[0], p->getPBest()[1]) ) );
            p->setGBestFitness( evaluate( searchSpace.getColor(globalBest[0], globalBest[1]) ) );
            
            // If the fitness value is better than the best fitness value (pBest) in history
            // set current value as the new pBest
            if (p->getFitness() <= p->getPBestFitness() ){
                p->setPBest(p->getPos());
            }
            
            if (p->getPBestFitness() <= p->getGBestFitness()){
                globalBest = p->getPBest();
            }
        }
        
        for (auto & p : particles){
            //Standard PSO Variation using contriction which will locate one optima and then become static
            for (int d = 0; d < p->getDimensions(); d++){
                p->setVel(d, X * p->getVel()[d] + c1 * ofRandom(1) * (p->getPBest()[d] - p->getPos()[d]) + c2 * ofRandom(1) * (globalBest[d] - p->getPos()[d]));
                p->setPos(d, p->getPos()[d] + p->getVel()[d] );
            }
            
            //Taken from the DFO algorith of using a disturbance threshold to improve diversity
            if (ofRandom(1) < dt){
                p->setPos(0, ofRandom(0, searchSpace.getWidth()) );
                p->setPos(1, ofRandom(0, searchSpace.getHeight()) );
            }
        
            p->setPos(0, ofClamp(p->getPos()[0], 0, searchSpace.getWidth()) );
            p->setPos(1, ofClamp(p->getPos()[1], 0, searchSpace.getHeight()) );
        }
    }
    
    double evaluate(ofColor color){
        ofVec2f velXY = ofVec2f(color.r, color.g);
        //Use only red and green values as these represne the velocity of the search space
        float a = 255 - velXY.x;
        float b = 255 - velXY.y;
        return (a+b)/2;
    }
    
    void draw(ofPixels &searchSpace){
        for(auto & p : particles){
            float mappedPosX = ofMap(p->getPos()[0], 0, searchSpace.getWidth(), 0, 480);
            float mappedPosY = ofMap(p->getPos()[1], 0, searchSpace.getHeight(), 0, 360);
            float mappedPBestPosX = ofMap(p->getPBest()[0], 0, searchSpace.getWidth(), 0, 480);
            float mappedPBestPosY = ofMap(p->getPBest()[1], 0, searchSpace.getHeight(), 0, 360);

            ofPushStyle();
                ofSetColor(255,0,0);
                ofDrawCircle(200 + mappedPosX, mappedPosY,  5);
            
                ofSetColor(0,0,255);
                ofDrawCircle(200 + mappedPBestPosX, mappedPBestPosY, 5);
            ofPopStyle();
        }
        
        float mappedGBestPosX = ofMap(globalBest[0], 0, searchSpace.getWidth(), 0, 480);
        float mappedGBestPosY = ofMap(globalBest[1], 0, searchSpace.getHeight(), 0, 360);
        
        ofPushStyle();
            ofSetColor(0,255,0);
            ofDrawCircle(200 + mappedGBestPosX , mappedGBestPosY, 5);
        ofPopStyle();
    }
    
    double output(){
        sound = 0.0;
        
        for (int i = 0 ; i < particles.size(); i++){
            ofVec2f dist = particles[i]->getDistance2Vec(globalBest);
            //float bestPosX = globalBest.x;
            //float bestFitness = gBestfitness;
            
            dist.x = ofMap(dist.x, 0, ofGetWidth(),  10, 400);
            dist.y = ofMap(dist.y, 0, ofGetHeight(), 1, 100);
            
            sound += particles[i]->output(dist.x, dist.y);
        }
        
        sound /= particles.size();
        sound = ofClamp(sound, -1, 1);
        
        return sound;
    }
    
};
