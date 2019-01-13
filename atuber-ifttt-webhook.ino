#include <WioLTEforArduino.h>
#include <stdio.h>

#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"

#define WEBHOOK_EVENTNAME "atuber-hungry"
#define WEBHOOK_KEY       "dtiYHqmhyJk3nKx5iNRV3w"
#define WEBHOOK_URL       "https://maker.ifttt.com/trigger/"WEBHOOK_EVENTNAME"/with/key/"WEBHOOK_KEY

#define INTERVAL          (60000)
#define BUTTON_PIN  (WIOLTE_D38)

WioLTE Wio;

volatile bool StateChanged = false;
volatile bool State = false;
volatile int Count = 0;

void change_state()
{
  State = !State;
  StateChanged = true;
  delay(150);
}

char getTwiiterStr(int count) {
  char data[5] = [
    'ねぇねぇ,お腹空いた,エサちょうだい〜',
    'お腹空いた,エサくださいよ〜',
    'お〜〜〜〜い,お腹すいたよ〜',
    '誰かエサくれないか〜',
    'お〜い！エサくれくれ',
  ];

  return data[count];
}

void send_to_ifttt() {
  char time[100];
  
  sprintf(time, "{\"value1\":\"uptime %lu\"}", millis() / 1000);
  char data = getTwiiterStr(Count);
  
  Count++;
  if(Count == 5) {
    Count = 0;
  }
  
  int status;

  
  SerialUSB.println("### Post.");
  
  SerialUSB.print("Post:");
  SerialUSB.print(data);
  SerialUSB.println("");
  if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
  SerialUSB.print("Status:");
  SerialUSB.println(status);

err:
  SerialUSB.println("### Wait.");
  delay(INTERVAL);
}

void setup() {
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(500);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, change_state, CHANGE);

  SerialUSB.println("### Setup completed.");
}

void loop() {
  if (StateChanged) {
    if (State) {
      send_to_ifttt();
    }
    StateChanged = false;
  } 
}
