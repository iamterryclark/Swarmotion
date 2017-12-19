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
    ofVec2f globalBest;
    float sound;
    double X = 0.72984;
    double c1 = X * (2.1/2);
    double c2 = X * (2.1/2);
    
public:
    ofParameterGroup parameters;
    ofParameter<float> dt;
    
    PSO(int population){

        for (int i = 0; i < population; i++){
            particles.push_back(new Particle());
            particles[i]->pos.x = ofRandom(0, 120);
            particles[i]->pos.y = ofRandom(0, 90);
        }
        
        parameters.setName("PSO Params");
        parameters.add(dt.set("Disturbance", 0.001, 0.0, 0.1));
        //parameters.add(topology.set("Topology", 0, 0, 2));
    }
    
    void update(ofPixels &searchSpace){
        
        for(auto & p : particles){
            
            // Calculate fitness value
            p->fitness      = evaluate( searchSpace.getColor(p->pos.x, p->pos.y) );
            p->pBestFitness = evaluate( searchSpace.getColor(p->personalBest.x, p->personalBest.y) );
            p->gBestFitness = evaluate( searchSpace.getColor(globalBest.x, globalBest.y) );
            
            // If the fitness value is better than the best fitness value (pBest) in history
            // set current value as the new pBest
            if (p->fitness <= p->pBestFitness ){
                p->personalBest = p->pos;
            }
            
            if (p->pBestFitness <= p->gBestFitness){
                globalBest = p->personalBest;
            }
        }
        
        for (auto & p : particles){
            //Standard PSO Variation using contriction which will locate one optima and then become static
            p->vel.x = X * p->vel.x + c1 * ofRandom(0, 1) * (p->personalBest.x - p->pos.x) + c2 * ofRandom(0,1) * (globalBest.x - p->pos.x);
            p->vel.y = X * p->vel.y + c1 * ofRandom(0, 1) * (p->personalBest.y - p->pos.y) + c2 * ofRandom(0,1) * (globalBest.y - p->pos.y);

            p->pos.x += p->vel.x;
            p->pos.y += p->vel.y;
            
            //Taken from the DFO algorith of using a disturbance threshold to improve diversity
            if (ofRandom(1) < dt){
                p->pos.x = ofRandom(0, searchSpace.getWidth());
                p->pos.y = ofRandom(0, searchSpace.getHeight());
            }
        
            p->pos.x = ofClamp(p->pos.x, 0, searchSpace.getWidth());
            p->pos.y = ofClamp(p->pos.y, 0, searchSpace.getHeight());
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
            float mappedPosX = ofMap(p->pos.x, 0, searchSpace.getWidth(), 0, 480);
            float mappedPosY = ofMap(p->pos.y, 0, searchSpace.getHeight(), 0, 360);
            float mappedPBestPosX = ofMap(p->personalBest.x, 0, searchSpace.getWidth(), 0, 480);
            float mappedPBestPosY = ofMap(p->personalBest.y, 0, searchSpace.getHeight(), 0, 360);

            ofPushStyle();
                ofSetColor(255,0,0);
                ofDrawCircle(200 + mappedPosX, mappedPosY,  5);
            
                ofSetColor(0,0,255);
                ofDrawCircle(200 + mappedPBestPosX, mappedPBestPosY, 5);
            ofPopStyle();
        }
        
        float mappedGBestPosX = ofMap(globalBest.x, 0, searchSpace.getWidth(), 0, 480);
        float mappedGBestPosY = ofMap(globalBest.y, 0, searchSpace.getHeight(), 0, 360);
        
        ofPushStyle();
            ofSetColor(0,255,0);
            ofDrawCircle(200 + mappedGBestPosX , mappedGBestPosY, 5);
        ofPopStyle();
    }
    
    double output(){
        sound = 0.0;
        
        for (int i = 0 ; i < particles.size(); i++){
            ofVec2f dist = particles[i]->getDistance2vec(globalBest);
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
