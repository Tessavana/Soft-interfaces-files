import processing.serial.*;

Serial myPort;
int[] sensor = new int[16];

final int RADIUS  = 40;
final int GAP     = 10;
final int MARGIN  = 10;
final int WIDTH   = 16 * (RADIUS + GAP) + MARGIN;
final int HEIGHT  = 120;

void settings() {
  size(WIDTH, HEIGHT);
}

void setup() {
  String portName = Serial.list()[0];  // Change index if needed
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');

  surface.setTitle("16-channel touch visualizer (A7..A0, A8..A15)");
  textAlign(CENTER, CENTER);
  textSize(10);
}

void draw() {
  background(255);

  for (int i = 0; i < 16; i++) {
    // Determine the actual sensor index to show at this visual position
    int sensorIndex;
    String label;

    if (i < 8) {
      // Left half (A7 to A0)
      sensorIndex = 7 - i;
      label = "A" + (7 - i);
    } else {
      // Right half (A8 to A15)
      sensorIndex = i;
      label = "A" + i;
    }

    int shade = int(map(sensor[sensorIndex], 0, 400, 255, 0));
    fill(shade);
    stroke(0);

    float x = MARGIN + RADIUS / 2 + i * (RADIUS + GAP);
    float y = height / 2;
    ellipse(x, y, RADIUS, RADIUS);

    fill(0);
    text(label, x, y + RADIUS * 0.75);
  }
}

void serialEvent(Serial p) {
  String line = trim(p.readStringUntil('\n'));
  if (line == null) return;

  String[] pieces = split(line, ',');
  if (pieces.length == 16) {
    for (int i = 0; i < 16; i++) {
      sensor[i] = int(constrain(float(pieces[i]), 0, 1023));
    }
  }
}
