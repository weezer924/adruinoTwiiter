#include <WioLTEforArduino.h>
#include <stdio.h>

#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"

#define WEBHOOK_EVENTNAME "atuber-hungry"
#define WEBHOOK_KEY       "bTyrqBzgy3ex8csO7FjG5H"
#define WEBHOOK_URL       "https://maker.ifttt.com/trigger/"WEBHOOK_EVENTNAME"/with/key/"WEBHOOK_KEY

#define INTERVAL          (1000)
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

void send_to_ifttt() {
  Count++;
  if(Count == 6) {
    Count = 0;
  }

  char* twiiterArray[]={
  "ねぇねぇ、お腹空いた、エサちょうだい〜",
  "お腹空いた、エサくださいよ〜",
  "お〜〜〜〜い、お腹すいたよ〜",
  "誰かエサくれないか〜",
  "お〜い！エサくれくれ",
  "誰か〜〜、お腹空いたやき"
  };

  char twiiterStr[100];
  sprintf(twiiterStr, twiiterArray[Count]);
  SerialUSB.print("twiiterStr:");
  SerialUSB.print(twiiterStr);

  SerialUSB.print("Count:");
  SerialUSB.print(Count);
  
  char data[100];
  sprintf(data, "{\"value1\":\"%s uptime %lu\"}", twiiterStr, millis() / 1000);

  
  int status;
  
  SerialUSB.println("### Post.");
  
  SerialUSB.print("Post:");
  SerialUSB.print(data);
  SerialUSB.println("");
  SerialUSB.println("### EXE #57");
  
  if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
  SerialUSB.println("### EXE #63");
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
