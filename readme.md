# Swarmotion - A comparison of DFO and PSO

## Definitions
- DFO - Dispersive Fly Optimisation
- PSO - Particle Swarm Optimisation

## How to install
- You will need to use openFrameworks 0.9.4
- Install below addons and compile

## Addons to include:
- ofxFlowTools
- ofxGui
- ofxMaxim

## How to use
- The gui in the top left is for the Optical Flow to capture the 
velocity
- Adjust the sliders threshold and strength to 
get the best possible results of flow.
- I found that also adjusting the blur radius 
gave a better result for the algorithms to fine 
the area of highest velocity.
- Decay will fade out the velocity more quickly 
if set higher

- The Algorthim parameters are minimal in this
- Select and algorithm - DFO or PSO
- Select a topology - How the swarm communicates
- Adjust the disturbance threshold 
