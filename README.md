# DIY-Oscilloscope
// Oscilloscope on Arduino Uno with Serial Terminal Display
// Features: High-speed ADC, basic triggering, ASCII waveform display, basic measurements

#define ADC_PIN A0          // Analog input pin for signal
#define SAMPLES 80          // Number of samples for display (80 columns)
#define TRIGGER_THRESHOLD 512 // Trigger threshold (midpoint of 10-bit ADC)
#define BAUD_RATE 115200    // Serial baud rate
#define SAMPLE_DELAY 100    // Delay between samples in microseconds (adjust for sampling rate)

// ASCII characters for waveform display
const char WAVEFORM_CHARS[] = " .:-=+*#%@";

// Buffer to store ADC samples
int samples[SAMPLES];

// Function prototypes
void captureSamples();
bool findTrigger(int mode);
void displayWaveform();
void calculateMeasurements();
float calculateFrequency();
float calculateAmplitude();
float calculatePeriod();

void setup() {
  // Initialize serial communication
  Serial.begin(BAUD_RATE);
  while (!Serial); // Wait for serial connection

  // Set ADC prescaler for faster sampling (optional, adjust for accuracy)
  ADCSRA = (ADCSRA & 0xF8) | 0x04; // Set prescaler to 16 (faster ADC)

  // Print welcome message
  Serial.println("Arduino Uno Oscilloscope");
  Serial.println("Trigger Mode: Edge (Rising)");
  Serial.println("Sampling Rate: ~10 kSa/s");
  Serial.println("----------------------------------------");
}

void loop() {
  // Capture samples
  captureSamples();

  // Find trigger point (edge trigger, rising)
  if (findTrigger(0)) { // 0 = Rising edge trigger
    // Display waveform
    displayWaveform();

    // Calculate and display measurements
    calculateMeasurements();
  } else {
    Serial.println("No trigger found. Adjust signal or threshold.");
  }

  // Small delay to prevent overwhelming Serial Terminal
  delay(500);
}

// Capture samples using ADC
void captureSamples() {
  for (int i = 0; i < SAMPLES; i++) {
    samples[i] = analogRead(ADC_PIN);
    delayMicroseconds(SAMPLE_DELAY); // Control sampling rate
  }
}

// Find trigger point (basic edge trigger)
bool findTrigger(int mode) {
  if (mode == 0) { // Rising edge trigger
    for (int i = 1; i < SAMPLES; i++) {
      if (samples[i - 1] < TRIGGER_THRESHOLD && samples[i] >= TRIGGER_THRESHOLD) {
        // Shift samples to align with trigger point
        int shift = i;
        for (int j = 0; j < SAMPLES - shift; j++) {
          samples[j] = samples[j + shift];
        }
        return true;
      }
    }
  }
  return false;
}

// Display waveform using ASCII characters
void displayWaveform() {
  // 24 rows for waveform display
  for (int row = 23; row >= 0; row--) {
    for (int col = 0; col < SAMPLES; col++) {
      // Map ADC value (0-1023) to row (0-23)
      int mappedValue = map(samples[col], 0, 1023, 0, 23);
      if (mappedValue == row) {
        Serial.print(WAVEFORM_CHARS[map(samples[col], 0, 1023, 0, 9)]); // ASCII char
      } else if (row == 12 && col == 0) {
        Serial.print("|"); // Trigger marker
      } else {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  Serial.println("----------------------------------------");
}

// Calculate and display measurements
void calculateMeasurements() {
  float frequency = calculateFrequency();
  float period = calculatePeriod();
  float amplitude = calculateAmplitude();

  Serial.print("Frequency: ");
  Serial.print(frequency, 2);
  Serial.println(" Hz");

  Serial.print("Period: ");
  Serial.print(period, 2);
  Serial.println(" ms");

  Serial.print("Amplitude: ");
  Serial.print(amplitude, 2);
  Serial.println(" V");
  Serial.println("----------------------------------------");
}

// Calculate frequency (based on zero crossings)
float calculateFrequency() {
  int crossings = 0;
  for (int i = 1; i < SAMPLES; i++) {
    if ((samples[i - 1] < TRIGGER_THRESHOLD && samples[i] >= TRIGGER_THRESHOLD) ||
        (samples[i - 1] >= TRIGGER_THRESHOLD && samples[i] < TRIGGER_THRESHOLD)) {
      crossings++;
    }
  }
  // Estimate frequency based on crossings and sampling rate
  float samplingRate = 1000000.0 / SAMPLE_DELAY; // Samples per second
  return (crossings / 2.0) * (samplingRate / SAMPLES);
}

// Calculate period (inverse of frequency)
float calculatePeriod() {
  float frequency = calculateFrequency();
  if (frequency == 0) return 0;
  return 1000.0 / frequency; // Period in milliseconds
}

// Calculate amplitude (peak-to-peak voltage)
float calculateAmplitude() {
  int minVal = 1023;
  int maxVal = 0;
  for (int i = 0; i < SAMPLES; i++) {
    if (samples[i] < minVal) minVal = samples[i];
    if (samples[i] > maxVal) maxVal = samples[i];
  }
  // Convert ADC values to voltage (assuming 5V reference)
  return (maxVal - minVal) * (5.0 / 1023.0);
}
