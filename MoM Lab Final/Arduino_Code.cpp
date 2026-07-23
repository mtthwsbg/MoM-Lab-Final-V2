
// Reads 3 quarter-bridge strain gauge amplifiers and prints tared microstrain
// to Serial for the Serial Plotter, using the EML 3301C Lab 1 quarter-bridge
// equation. No CLI args -- everything you'd ever change is one of the
// variables below.
//
//   eps = 4*dVamp / (gain*Vs*Gf)      <- Lab 1 quarter-bridge equation
//   ue  = eps * 1e6                    <- microstrain
//
// Setup:
//   1. Wire amp outputs to SG1/SG2/SG3.
//   2. Leave the specimen unloaded and power up / reset the board -- it
//      waits 3 s, then reads and locks in the tare automatically and
//      prints "tare" once that's done.
//   3. Load the specimen; Serial Plotter now shows tared microstrain only.
//      Set SHOW_RAW = true if you ever want raw ADC counts printed too.

const int SG1 = A0;
const int SG2 = A1;
const int SG3 = A2;

// ---- voltage window ----
// The Uno's ADC always spreads 1024 codes (0-1023) across whatever range you
// tell it to represent, centered on 0 here:
//   WINDOW = 5  ->  codes 0..1023 map to -5 V .. +5 V
// RESOLUTION (volts/bit) is derived from WINDOW automatically -- a bigger
// window stretches the same 1024 codes over more volts, so it auto-updates
// to a coarser value. Match WINDOW to what your amp output actually swings
// (e.g. a single-ended 0-5V output centered at 2.5V unloaded -> WINDOW = 2.5).
const float WINDOW = 0.05;
const float RESOLUTION = (2.0 * WINDOW) / 1024.0;   // auto-updates with WINDOW

// ---- Lab 1 quarter-bridge equation constants ----
const float GAIN = 900.0;     // amplifier gain, fixed on this board
const float VS = 2.5;         // bridge excitation voltage
const float GF = 2.00;       // gauge factor

// ---- print raw ADC counts too? only turn on while checking wiring ----
const bool SHOW_RAW = false;

// ---- tare, raw ADC counts -- set automatically in setup(), leave alone ----
float tare1, tare2, tare3;

void setup() {
  Serial.begin(9600);

  delay(3000);   // let the amps settle with the specimen unloaded

  tare1 = analogRead(SG1);
  tare2 = analogRead(SG2);
  tare3 = analogRead(SG3);

  Serial.println("tare");
}

void loop() {
  int raw1 = analogRead(SG1);
  int raw2 = analogRead(SG2);
  int raw3 = analogRead(SG3);

  float v1 = raw1 * RESOLUTION - WINDOW;
  float v2 = raw2 * RESOLUTION - WINDOW;
  float v3 = raw3 * RESOLUTION - WINDOW;

  float v0_1 = tare1 * RESOLUTION - WINDOW;
  float v0_2 = tare2 * RESOLUTION - WINDOW;
  float v0_3 = tare3 * RESOLUTION - WINDOW;

  float dVamp1 = v1 - v0_1;
  float dVamp2 = v2 - v0_2;
  float dVamp3 = v3 - v0_3;

  float ue1 = 4.0 * dVamp1 / (GAIN * VS * GF) * 1e6;
  float ue2 = 4.0 * dVamp2 / (GAIN * VS * GF) * 1e6;
  float ue3 = 4.0 * dVamp3 / (GAIN * VS * GF) * 1e6;

  if (SHOW_RAW) {
    Serial.print("raw1:");
    Serial.print(abs(raw1));
    Serial.print(" raw2:");
    Serial.print(abs(raw2));
    Serial.print(" raw3:");
    Serial.print(abs(raw3));
    Serial.print(" ");
  }


  //microstrain
  Serial.print("SG1:");
  Serial.print(abs(ue1));
  Serial.print(" SG2:");
  Serial.print(abs(ue2));
  Serial.print(" SG3:");
  Serial.println(abs(ue3));

  delay(200);
}
