/*
 *  Small program to read a 16-bit, signed, 44.1kHz wave file and play it.
 *  Written by Brian Fraser, heavily based on code found at:
 *  http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_min_8c-example.html
 */

#include <alsa/asoundlib.h>
#include <time.h>
#include <math.h>


//audio.h
// #include "mixer.h"
// #include "audio.h"
// #include "playback.h"
// #include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> 
//accel
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SOURCE_FILE "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define BASE "wave-files/100051__menegass__gui-drum-bd-hard.wav"

#define HI_HATA1 "wave-files/100056__menegass__gui-drum-cyn-hard.wav"
#define SNAREA1 "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define BASEA1 "wave-files/100066__menegass__gui-drum-tom-mid-hard.wav"

#define HI_HATA2 "wave-files/100055__menegass__gui-drum-co.wav"
#define SNAREA2 "wave-files/100058__menegass__gui-drum-snare-hard.wav"
#define BASEA2 "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav"
//#define SOURCE_FILE "wave-files/100053__menegass__gui-drum-cc.wav"

#define SAMPLE_RATE   44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short)) 	// bytes per sample

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;



//audio 

static snd_pcm_t *handle;

#define DEFAULT_VOLUME 80

// #define SAMPLE_RATE 44100
// #define NUM_CHANNELS 1
// #define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample
// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.

static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;


// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30
typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;
static playbackSound_t soundBites[MAX_SOUND_BITES];


static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// Prototypes:
snd_pcm_t *Audio_openDevice();
void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);
void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);


///audio.h

// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AudioMixer_init(void);
void AudioMixer_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioMixer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioMixer_queueSound(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AudioMixer_getVolume(void);
void AudioMixer_setVolume(int newVolume);

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
snd_pcm_t *Audio_openDevice()
{
	snd_pcm_t *handle;

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Play-back open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Play-back configuration error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	return handle;
}

// Read in the file to dynamically allocated memory.
// !! Client code must free memory in wavedata_t !!
void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct)
{
	assert(pWaveStruct);

	// Wave file has 44 bytes of header data. This code assumes file
	// is correct format.
	const int DATA_OFFSET_INTO_WAVE = 44;

	// Open file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - DATA_OFFSET_INTO_WAVE;
	fseek(file, DATA_OFFSET_INTO_WAVE, SEEK_SET);
	pWaveStruct->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Allocate Space
	pWaveStruct->pData = malloc(sizeInBytes);
	if (pWaveStruct->pData == NULL) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read data:
	int samplesRead = fread(pWaveStruct->pData, SAMPLE_SIZE, pWaveStruct->numSamples, file);
	if (samplesRead != pWaveStruct->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pWaveStruct->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

// Play the audio file (blocking)
void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData)
{
	// If anything is waiting to be written to screen, can be delayed unless flushed.
	fflush(stdout);

	// Write data and play sound (blocking)
	snd_pcm_sframes_t frames = snd_pcm_writei(handle, pWaveData->pData, pWaveData->numSamples);

	// Check for errors
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);
	if (frames < 0) {
		fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
		exit(EXIT_FAILURE);
	}
	if (frames > 0 && frames < pWaveData->numSamples)
		printf("Short write (expected %d, wrote %li)\n", pWaveData->numSamples, frames);
}























//mixer.c
// Playback threading
void* playbackThread(void* arg);
static bool stopping = false;
static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;

static int volume = 0;

void AudioMixer_init(void)
{
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	// REVISIT:- Implement this. Hint: set the pSound pointer to NULL for each
	//     sound bite.

	for (int i = 0; i < MAX_SOUND_BITES; i++ ){
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}

	handle = Audio_openDevice();

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}


// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	Audio_readWaveFileIntoMemory(fileName, pSound);
}

void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

	// Insert the sound by searching for an empty sound bite spot
	/*
	 * REVISIT: Implement this:
	 * 1. Since this may be called by other threads, and there is a thread
	 *    processing the soundBites[] array, we must ensure access is threadsafe.
	 * 2. Search through the soundBites[] array looking for a free slot.
	 * 3. If a free slot is found, place the new sound file into that slot.
	 *    Note: You are only copying a pointer, not the entire data of the wave file!
	 * 4. After searching through all slots, if no free slot is found then print
	 *    an error message to the console (and likely just return vs asserting/exiting
	 *    because the application most likely doesn't want to crash just for
	 *    not being able to play another wave file.
	 */

	pthread_mutex_lock(&audioMutex);
	bool slotFound = false;

	for(int i = 0 ; i < MAX_SOUND_BITES ; i++ ){
		
		if( soundBites[i].pSound == NULL){

			soundBites[i].pSound = pSound;
			soundBites[i].location = 0;
			slotFound = true;
			break;
		}
	}

	if(slotFound == false){
		printf("No empty slots\n");
	}

	pthread_mutex_unlock(&audioMutex);




}

void AudioMixer_cleanup(void)
{
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	printf("Done stopping audio...\n");
	fflush(stdout);
}


int AudioMixer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > 100) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *volHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}


// Fill the `buff` array with new PCM values to output.
//    `buff`: buffer to fill with new PCM data from sound bites.
//    `size`: the number of values to store into playbackBuffer
static void fillPlaybackBuffer(short *buff, int size)
{
	/*
	 * REVISIT: Implement this
	 * 1. Wipe the playbackBuffer to all 0's to clear any previous PCM data.
	 *    Hint: use memset()
	 * 2. Since this is called from a background thread, and soundBites[] array
	 *    may be used by any other thread, must synchronize this.
	 * 3. Loop through each slot in soundBites[], which are sounds that are either
	 *    waiting to be played, or partially already played:
	 *    - If the sound bite slot is unused, do nothing for this slot.
	 *    - Otherwise "add" this sound bite's data to the play-back buffer
	 *      (other sound bites needing to be played back will also add to the same data).
	 *      * Record that this portion of the sound bite has been played back by incrementing
	 *        the location inside the data where play-back currently is.
	 *      * If you have now played back the entire sample, free the slot in the
	 *        soundBites[] array.
	 *
	 * Notes on "adding" PCM samples:
	 * - PCM is stored as signed shorts (between SHRT_MIN and SHRT_MAX).
	 * - When adding values, ensure there is not an overflow. Any values which would
	 *   greater than SHRT_MAX should be clipped to SHRT_MAX; likewise for underflow.
	 * - Don't overflow any arrays!
	 * - Efficiency matters here! The compiler may do quite a bit for you, but it doesn't
	 *   hurt to keep it in mind. Here are some tips for efficiency and readability:
	 *   * If, for each pass of the loop which "adds" you need to change a value inside
	 *     a struct inside an array, it may be faster to first load the value into a local
	 *      variable, increment this variable as needed throughout the loop, and then write it
	 *     back into the struct inside the array after. For example:
	 *           int offset = myArray[someIdx].value;
	 *           for (int i =...; i < ...; i++) {
	 *               offset ++;
	 *           }
	 *           myArray[someIdx].value = offset;
	 *   * If you need a value in a number of places, try loading it into a local variable
	 *          int someNum = myArray[someIdx].value;
	 *          if (someNum < X || someNum > Y || someNum != Z) {
	 *              someNum = 42;
	 *          }
	 *          ... use someNum vs myArray[someIdx].value;
	 *
	 */
	
	memset(buff, 0, size * sizeof(short));

	pthread_mutex_lock(&audioMutex);

	wavedata_t *audio;
	int loc;
	int index = 0;

	for(int i = 0; i < MAX_SOUND_BITES; i++){
		if(soundBites[i].pSound != NULL){
			audio = soundBites[i].pSound;
			loc = soundBites[i].location;
			index = 0;
			while(index < size && loc + index < audio->numSamples){

				int sound = buff[index] + audio->pData[loc];

				if(sound<SHRT_MIN){
					sound = SHRT_MIN;
				}
				if(sound>SHRT_MAX){
					sound = SHRT_MAX;
				}
				
				buff[index] = sound;

				loc++;
				index++;
			}
			soundBites[i].location = loc;

			if(audio->numSamples <= loc){
				soundBites[i].pSound = NULL;
				soundBites[i].location = 0;
			}
		}
	}

	pthread_mutex_unlock(&audioMutex);
}


void* playbackThread(void* arg)
{

	while (!stopping) {
		// Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);


		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,
				playbackBuffer, playbackBufferSize);

		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize) {
			printf("Short write (expected %li, wrote %li)\n",
					playbackBufferSize, frames);
		}
	}

	return NULL;
}

















//joystick.c


enum JoystickDirection  {up, right, down, left, pushed, none};

int readFromFileToScreen(char *path);

static enum JoystickDirection getDirection(void);


#define JSUP "/sys/class/gpio/gpio26/value" //filepath for press value of joystick
#define JSRT "/sys/class/gpio/gpio47/value" ////1 is not pressed 0 is pressed
#define JSDN "/sys/class/gpio/gpio46/value"
#define JSLFT "/sys/class/gpio/gpio65/value"
#define JSPB "/sys/class/gpio/gpio27/value"


int readFromFileToScreen(char *path)
{
    FILE *pFile = fopen(path, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", path);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    return atoi(buff);
}

static enum JoystickDirection getDirection(void)
{
    if(readFromFileToScreen(JSUP) == 0){
        return up;
    } else if (readFromFileToScreen(JSRT) == 0){
        return right;
    }   else if (readFromFileToScreen(JSDN) == 0){
        return down;
    }   else if (readFromFileToScreen(JSLFT) == 0){
        return left;
    } else if (readFromFileToScreen(JSPB) == 0){
        return pushed;
    } else{
        return none;
    } 
}









//accel

#define BUS1 "/dev/i2c-1"
#define I2C_ADDRESS 0x1C
#define WHO_AM_I 0x2A
#define XMSB 0x01
#define XLSB 0x02
#define YMSB 0x03
#define YLSB 0x04
#define ZMSB 0x05
#define ZLSB 0x06

static int i2cFile;

void AccelStart();
void AccelStop();
int *getAccel();

// static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,
// unsigned char value)
// {
// unsigned char buff[2];
// buff[0] = regAddr;
// buff[1] = value;
// int res = write(i2cFileDesc, buff, 2);
// if (res != 2) {
// perror("I2C: Unable to write i2c register.");
// exit(1);
// }
// }

// static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
// {
// // To read a register, must first write the address
// int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
// if (res != sizeof(regAddr)) {
// perror("I2C: Unable to write to i2c register.");
// exit(1);
// }
// // Now read the value and return it
// char value = 0;
// res = read(i2cFileDesc, &value, sizeof(value));
// if (res != sizeof(value)) {
// perror("I2C: Unable to read from i2c register");
// exit(1);
// }
// return value;
// }

static int initI2cBus(char* bus, int address)
{
int i2cFileDesc = open(bus, O_RDWR);
int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
if (result < 0) {
perror("I2C: Unable to set I2C device to slave address.");
exit(1);
}
return i2cFileDesc;
}




void AccelStart() {
    i2cFile = initI2cBus( BUS1, I2C_ADDRESS);
	char ar[2];
	ar[0] = WHO_AM_I;
	ar[1] = 0x01;
	write(i2cFile, ar, 2);
}

void AccelStop() {
   	char ar[2];
	ar[0] = 0x2A;
	ar[1] = 0x00;
	write(i2cFile, ar, 2);
    close(i2cFile);
}

int *getAccel() {
    char ar[7];
	static int array[3];

	write(i2cFile, 0x0, 1);
	if(read(i2cFile, ar, 7) != 7){
		printf("Error getting accel\n");
	}else{
		int16_t x = (ar[1] << 8) | ar[2];
		int16_t y = (ar[3] << 8) | ar[4];
		int16_t z = (ar[5] << 8) | ar[6];

		array[0] = x / 10000;
		array[1] = y / 10000;
		array[2] = z / 10000;
	}
    return array;
}



























static int bpm = 120;
static int pattern = 3;

static bool pattern0 = false;
static bool pattern1 = false;
static bool pattern2 = false;
static bool play = false;



int getBPM(){
	return bpm;
}
int setBPM(int newBPM){
	return bpm = newBPM;
}

void changeBPM(int change){
	int newBPM = getBPM() + change;

	if(newBPM>300){
		newBPM = 300;
	}
	if(newBPM < 40){
		newBPM = 40;
	}
	setBPM(newBPM);
	
}

static void sleepBPM(){
	long long delay = floor(60000/getBPM());
	sleepForMs(delay);
}


int getPattern(){
	return pattern;
}

int setPattern(int n){
	return pattern = n;
}

void *playPattern(void *arg){
	while(play){
		while(pattern0){

		}


		while(pattern1){
			wavedata_t hiHat;
			wavedata_t base;
			wavedata_t snare;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
			AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
			AudioMixer_readWaveFileIntoMemory(BASE, &base);
			int beat = 0;
			while (pattern1){
				if(beat == 0){
					beat = 1;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				} else if(beat == 1){
					beat = 2;
					AudioMixer_queueSound(&hiHat);
					
					
				}else if(beat == 2){
					beat = 3;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&snare);
				} else{
					beat = 0;
					AudioMixer_queueSound(&hiHat);

				}
				sleepBPM();
			}

			AudioMixer_freeWaveFileData(&hiHat);
			AudioMixer_freeWaveFileData(&base);
			AudioMixer_freeWaveFileData(&snare);
		}



		while(pattern2){
			wavedata_t hiHat;
			wavedata_t base;
			wavedata_t snare;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
			AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
			AudioMixer_readWaveFileIntoMemory(BASE, &base);
			int beat = 0;
			while (pattern2){
				if(beat == 0){
					beat = 1;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				} else if(beat == 1){
					beat = 2;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				}else if(beat == 2){
					beat = 3;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&snare);
				} else{
					beat = 0;
					AudioMixer_queueSound(&hiHat);

				}
				sleepBPM();
			}

			AudioMixer_freeWaveFileData(&hiHat);
			AudioMixer_freeWaveFileData(&base);
			AudioMixer_freeWaveFileData(&snare);
		}


	}
	return NULL;
}

void setBoolPattern(){
	if(getPattern() == 0){
		play = true;
		pattern0 = true;
		pattern1 = false;
		pattern2 = false;
	}
	if(getPattern() == 1){
		play = true;
		pattern0 = false;
		pattern1 = true;
		pattern2 = false;
	}
	if(getPattern() == 2){
		play = true;
		pattern0 = false;
		pattern1 = false;
		pattern2 = true;
	}
	if(getPattern() == 3){
		play = false;
		pattern0 = false;
		pattern1 = false;
		pattern2 = false;
	}
}


void setVolume(int volume){
    volume = AudioMixer_getVolume() + volume;
    if(volume > 100){
        volume = 100;
    }
    
    if(volume<0){
        volume = 0;
    }

    AudioMixer_setVolume(volume);
    return;
}

void* playAccel(void* arg){

	while(play){
		if(getPattern() == 0){

		}
		else if (getPattern() == 1){
			wavedata_t hiHatA1;
			wavedata_t baseA1;
			wavedata_t snareA1;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HATA1, &hiHatA1);
			AudioMixer_readWaveFileIntoMemory(SNAREA1, &snareA1);
			AudioMixer_readWaveFileIntoMemory(BASEA1, &baseA1);

			while(getPattern() == 1){
				int *a = getAccel();

				if(a[0] == -1 || a[0] ==1){

					AudioMixer_queueSound(&hiHatA1);
					sleepBPM();
				}
				else if(a[1]== -1 || a[1] ==1){

					AudioMixer_queueSound(&snareA1);
					sleepBPM();
				}else if(a[2] == -1 || a[2] ==0){

					AudioMixer_queueSound(&baseA1);
					sleepBPM();
				}
			}
			AudioMixer_freeWaveFileData(&hiHatA1);
			AudioMixer_freeWaveFileData(&baseA1);
			AudioMixer_freeWaveFileData(&snareA1);
		}
		else if(getPattern() == 2){
			wavedata_t hiHatA2;
			wavedata_t baseA2;
			wavedata_t snareA2;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HATA2, &hiHatA2);
			AudioMixer_readWaveFileIntoMemory(SNAREA2, &snareA2);
			AudioMixer_readWaveFileIntoMemory(BASEA2, &baseA2);
			while(getPattern() == 2){
				int *a = getAccel();

				if(a[0] == -1 || a[0] ==1){

					AudioMixer_queueSound(&hiHatA2);
					sleepBPM();
				}
				else if(a[1]== -1 || a[1] ==1){


					AudioMixer_queueSound(&snareA2);
					sleepBPM();
				}else if(a[2] == -1 || a[2] ==0){
					AudioMixer_queueSound(&baseA2);
					sleepBPM();
				}
			}
			AudioMixer_freeWaveFileData(&hiHatA2);
			AudioMixer_freeWaveFileData(&baseA2);
			AudioMixer_freeWaveFileData(&snareA2);
		}
	}
	return NULL;
}




void *terminalOutput(void* args){

	while(play){
		printf("M%d %dbpm vol:%d \n",getPattern(), getBPM(), AudioMixer_getVolume() );
		sleepForMs(1000);
	}

	return NULL;
}
//main

int main(void){
	setVolume(80);
	setBPM(120);





	AudioMixer_init();
	AccelStart();

	//sleepForMs(1000);

	setPattern(1);
	setBoolPattern();

    pthread_t playPatternThread;

	pthread_t accelThread;

	pthread_t terminalThread;

    if (pthread_create(&playPatternThread, NULL, playPattern, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }
	
	if (pthread_create(&accelThread, NULL, playAccel, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }

	if (pthread_create(&terminalThread, NULL, terminalOutput, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }
	

	bool terminate = false;

	while(!terminate){
		if(terminate){
			setPattern(3);
			sleepForMs(400);
			setBoolPattern();
			break;
		}
		else if(getDirection() == up){
			setVolume(5);
			printf("Volume: %d\n", AudioMixer_getVolume());
			sleepForMs(400);
		} 
		else if(getDirection() == down){
			setVolume(-5);
			printf("Volume: %d\n", AudioMixer_getVolume());
			sleepForMs(400);
		}
		else if(getDirection() == left){
			changeBPM(-5);
			printf("BPM: %d\n", getBPM());
			sleepForMs(400);
		}
		else if(getDirection() == right){
			changeBPM(5);
			printf("BPM: %d\n", getBPM());
			sleepForMs(400);
		}
		else if(getDirection() == pushed){
			if(getPattern() == 0){
				setPattern(1);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d : rock\n", getPattern());
				sleepForMs(400);
			}
			else if(getPattern() == 1){
				setPattern(2);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d : custom\n", getPattern());
				
			}
			else if(getPattern() == 2){
				setPattern(0);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d none\n", getPattern());
			}
		}
	}

    if (pthread_join(playPatternThread, NULL) != 0) {
        fprintf(stderr, "Error joining playPattern thread\n");
        return 1; 
    }

	if (pthread_join(accelThread, NULL) != 0) {
        fprintf(stderr, "Error joining accelThread thread\n");
        return 1; 
    }

	if (pthread_join(terminalThread, NULL) != 0) {
        fprintf(stderr, "Error joining terminalThread thread\n");
        return 1; 
    }

	AccelStop();
	AudioMixer_cleanup();
	

	return 0;
}