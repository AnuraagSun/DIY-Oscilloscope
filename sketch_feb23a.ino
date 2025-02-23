// Advanced DIY Oscilloscope on Arduino Uno
// New Features:
// - Adjustable trigger level
// - Multiple trigger modes (Rising, Falling, Auto)
// - Voltage scale selection
// - Timebase adjustment
// - Improved measurements (RMS, Duty Cycle)
// - Serial command interface
// - Freeze display option

#define ADC_PIN A0          // Analog input pin for signal
#define SAMPLES 80          // Number of samples for display
#define BAUD_RATE 115200    // Serial baud rate
#define MAX_TIMEBASE 1000   // Maximum sample delay in microseconds
#define MIN_TIMEBASE 10     // Minimum sample delay in microseconds

// ASCII characters for waveform display
const char WAVEFORM_CHARS[] = " .:-=+*#%@";

// Trigger modes
enum TriggerMode {
  TRIGGER_RISING = 0,
  TRIGGER_FALLING = 1,
  TRIGGER_AUTO = 2
};

// Buffer to store ADC samples
int samples[SAMPLES];

// Configuration variables
int triggerThreshold = 512;    // Default trigger threshold (0-1023)
int sampleDelay = 100;         // Default sample delay in microseconds
TriggerMode triggerMode = TRIGGER_RISING;
float voltageScale = 5.0;      // Default voltage scale (5V)
bool freezeDisplay = false;    // Display freeze flag

// Function prototypes
void captureSamples();
bool findTrigger();
void displayWaveform();
void calculateMeasurements();
float calculateFrequency();
float calculatePeriod();
float calculateAmplitude();
float calculateRMS();
float calculateDutyCycle();
void handleSerialCommands();
void printHelp();

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial);

  // Set ADC prescaler for faster sampling
  ADCSRA = (ADCSRA & 0xF8) | 0x04; // Prescaler 16

  // Print welcome message and help
  Serial.println("Advanced Arduino Uno Oscilloscope");
  Serial.println("Type 'help' for commands");
  Serial.println("----------------------------------------");
}

void loop() {
  // Handle serial commands
  handleSerialCommands();

  if (!freezeDisplay) {
    // Capture samples
    captureSamples();

    // Find trigger point
    bool triggered = findTrigger();
    if (triggered || triggerMode == TRIGGER_AUTO) {
      // Display waveform
      displayWaveform();
      // Calculate and display measurements
      calculateMeasurements();
    } else {
      Serial.println("No trigger found. Adjust signal or threshold.");
    }
  }

  delay(100); // Reduced delay for better responsiveness
}

// Capture samples using ADC
void captureSamples() {
  for (int i = 0; i < SAMPLES; i++) {
    samples[i] = analogRead(ADC_PIN);
    delayMicroseconds(sampleDelay);
  }
}

// Find trigger point
bool findTrigger() {
  // Auto trigger always returns true
  if (triggerMode == TRIGGER_AUTO) return true;

  for (int i = 1; i < SAMPLES; i++) {
    if (triggerMode == TRIGGER_RISING) {
      if (samples[i - 1] < triggerThreshold && samples[i] >= triggerThreshold) {
        // Shift samples to align with trigger point
        int shift = i;
        for (int j = 0; j < SAMPLES - shift; j++) {
          samples[j] = samples[j + shift];
        }
        return true;
      }
    } else if (triggerMode == TRIGGER_FALLING) {
      if (samples[i - 1] >= triggerThreshold && samples[i] < triggerThreshold) {
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
      // Map ADC value to row based on voltage scale
      int mappedValue = map(samples[col], 0, 1023, 0, 23);
      if (mappedValue == row) {
        Serial.print(WAVEFORM_CHARS[map(samples[col], 0, 1023, 0, 9)]);
      } else if (row == map(triggerThreshold, 0, 1023, 0, 23)) {
        Serial.print("-"); // Trigger level marker
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
  float rms = calculateRMS();
  float dutyCycle = calculateDutyCycle();

  Serial.print("Frequency: ");
  Serial.print(frequency, 2);
  Serial.println(" Hz");

  Serial.print("Period: ");
  Serial.print(period, 2);
  Serial.println(" ms");

  Serial.print("Amplitude: ");
  Serial.print(amplitude, 2);
  Serial.println(" V");

  Serial.print("RMS: ");
  Serial.print(rms, 2);
  Serial.println(" V");

  Serial.print("Duty Cycle: ");
  Serial.print(dutyCycle, 1);
  Serial.println(" %");

  Serial.print("Timebase: ");
  Serial.print(sampleDelay);
  Serial.println(" us/sample");

  Serial.print("Trigger: ");
  Serial.print(triggerThreshold * (voltageScale / 1023.0), 2);
  Serial.println(" V");
  Serial.println("----------------------------------------");
}

// Calculate frequency (based on zero crossings)
float calculateFrequency() {
  int crossings = 0;
  for (int i = 1; i < SAMPLES; i++) {
    if ((samples[i - 1] < triggerThreshold && samples[i] >= triggerThreshold) ||
        (samples[i - 1] >= triggerThreshold && samples[i] < triggerThreshold)) {
      crossings++;
    }
  }
  float samplingRate = 1000000.0 / sampleDelay;
  return (crossings / 2.0) * (samplingRate / SAMPLES);
}

// Calculate period
float calculatePeriod() {
  float frequency = calculateFrequency();
  if (frequency == 0) return 0;
  return 1000.0 / frequency;
}

// Calculate amplitude
float calculateAmplitude() {
  int minVal = 1023;
  int maxVal = 0;
  for (int i = 0; i < SAMPLES; i++) {
    if (samples[i] < minVal) minVal = samples[i];
    if (samples[i] > maxVal) maxVal = samples[i];
  }
  return (maxVal - minVal) * (voltageScale / 1023.0);
}

// Calculate RMS voltage
float calculateRMS() {
  long sum = 0;
  int avg = 0;
  // Calculate average for DC offset
  for (int i = 0; i < SAMPLES; i++) {
    avg += samples[i];
  }
  avg /= SAMPLES;

  // Calculate RMS
  for (int i = 0; i < SAMPLES; i++) {
    int dev = samples[i] - avg;
    sum += dev * dev;
  }
  return sqrt(sum / SAMPLES) * (voltageScale / 1023.0);
}

// Calculate duty cycle
float calculateDutyCycle() {
  int aboveThreshold = 0;
  for (int i = 0; i < SAMPLES; i++) {
    if (samples[i] >= triggerThreshold) {
      aboveThreshold++;
    }
  }
  return (aboveThreshold * 100.0) / SAMPLES;
}

// Handle serial commands
void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "help") {
      printHelp();
    }
    else if (command.startsWith("trigger ")) {
      int value = command.substring(8).toInt();
      triggerThreshold = constrain(value, 0, 1023);
    }
    else if (command == "mode rising") {
      triggerMode = TRIGGER_RISING;
    }
    else if (command == "mode falling") {
      triggerMode = TRIGGER_FALLING;
    }
    else if (command == "mode auto") {
      triggerMode = TRIGGER_AUTO;
    }
    else if (command.startsWith("timebase ")) {
      int value = command.substring(9).toInt();
      sampleDelay = constrain(value, MIN_TIMEBASE, MAX_TIMEBASE);
    }
    else if (command.startsWith("scale ")) {
      float value = command.substring(6).toFloat();
      voltageScale = constrain(value, 1.0, 10.0);
    }
    else if (command == "freeze") {
      freezeDisplay = !freezeDisplay;
      Serial.println(freezeDisplay ? "Display frozen" : "Display resumed");
    }
  }
}

// Print help message
void printHelp() {
  Serial.println("Available commands:");
  Serial.println("trigger <value> - Set trigger threshold (0-1023)");
  Serial.println("mode rising - Set rising edge trigger");
  Serial.println("mode falling - Set falling edge trigger");
  Serial.println("mode auto - Set auto trigger");
  Serial.println("timebase <us> - Set timebase (10-1000 us)");
  Serial.println("scale <volts> - Set voltage scale (1-10 V)");
  Serial.println("freeze - Toggle display freeze");
  Serial.println("----------------------------------------");
}