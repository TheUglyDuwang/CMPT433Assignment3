
#ifndef BEATBOX_H
#define BEATBOX_H

// Function to initialize audio playback
void initAudio();

// Function to generate different drum beats
void dumBeat1();
void drumBeat2();
void drumBeat3();

// Function to control tempo (BPM)
void setTempo(int bpm);

// Function to control volume
void setVolume(int volume);

// Function to play additional drum sounds
void playDrumSound(int soundIndex);
