# Weapon of Mads. (Madisyn's) Destruction
A project designed to stop Madisyn from going into the Laundry Room.

## Introduction
Meet Madisyn, a 3 year old domestic short-hair/russian blue mix with an abundance of energy.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/madisyn.jpg?raw=true" width="200" />

Now meet, Mr. Darcy; an 18 year old domestic short hair who enjoys long naps in the sun and begging for food.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/darcy.jpg?raw=true" width="200" />

Madisyn desperately wants to play with Darcy, but he wants nothing to do with her. He'll regularly hiss and swap her away,
but does that stop Ms. Madisyn? Absolutely not! She will be his friend no matter what! Recently she's gotten into a habit of
checking on Mr. Darcy when he uses the bathroom, and Mr Darcy does not appreciate this. So much so, that I began to fear he may
stop using his litter box all together. I tried adding more litter boxes, and while she'll use any of them, he has used this one
for the last 10 years and he's not about to change now!

What I need is a way to keep Madisyn out of the Laundry Room, without preventing Darcy from entering. Browsing Amazon one night,
I stumbled upon the [PetSafe SSSCAT Spray Deterrent](https://www.amazon.com/dp/B0721735K9). It's a motion activated air spray can
meant to startle a pet and keep them out of places you don't want them to go. 

There are two problems with this product however. First, the sprayer is battery powered and I don't want to have to stay on top of
changing the batteries. Since it took 4 AAA, I figured I may be able to convert it to micro-USB easily enough. The second and
much bigger issue: this sprayer triggers on motion - any motion. This means it'll spray not only Madisyn but Darcy and anyone
else that walks into my laundry room! However I thought I had a solution to this problem too, so I bought it.

## The _Simple_ Solution
My idea seemed simple enough, I was going to use a camera and a [custom trained Yolov5](https://github.com/ultralytics/yolov5) model
to detect which cat had entered the laundry room. If it was Madisyn, I was going to activate a Smart Plug which would power the sprayer
and allow its motion sensor to trigger. If it was a person, or Darcy, the Smart Plug (and thus the sprayer) would remain powered off. 

When the sprayer got here I immediately noticed a problem. When powered on, it took 10-30 seconds before the motion sensor would initially
trigger. This was not acceptable, as it's possible Madisyn would already be causing trouble by the time the sprayer was ready. Upon disassembly,
I noted that the sprayer's micro-controller, while getting 5v from the batteries, was powered by 3.3v; the same power requirements
for an ESP8266 board. So I decided to do a full board swap, and instead of relying on motion I would issue an MQTT command when the camera
detected Madisyn, to trigger the sprayer.

## The Model
Out of the box, the Yolov5 models can already alert me if it detects a cat; but I needed more. I needed the model to differentiate between
each cat, not just tell me "there's a cat". I started by having Blue Iris (my NVR of choice), take a photo every 2 seconds when motion was
detected. I let this run for 4 days and was able to collect over 500 pictures of Darcy, but only 100 pictures of Madisyn. Ideally I
would have liked more pictures of Madisyn to train on, but I decided to [train](https://www.codeproject.com/Articles/5347827/How-to-Train-a-Custom-YOLOv5-Model-to-Detect-Objec)
on ~100 images of each cat as a first attempt and I am happy to say I was pleasantly surprised by results:
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/detect.png?raw=true" />

### Version 2
After running version 1 for a few days, I collected around 500 additional pictures of each cat and re-trained the model on around ~848
pictures total. I also increased the epochs to 300 as opposed to the 50 laid out in the earlier steps of that linked tutorial. The results
was a much more accurate model which now was able to correctly identifies each cat with 95-97% accuracy. 

### Version 3
I added another ~1000 pictures (500 of each) from a different camera; just to see if I could detect when my cats were in a different
part of the house. The model seems to be equally as accurate when running in my laundry room, and much more accurate when detecting
the cats on this additional camera. Next I'm going to look into continuous training, taking pictures from these cameras and using previous
versions of the models to generate test data for any future training runs.

## The Sprayer
For the sprayer I started by removing the stock micro-controller board and ended up disconnecting the PIR sensor entirely. Initially, I
wanted to pass the PIR data into MQTT as additional sensor data but since the stock micro-controller board had all the relevant circuitry to
boost the PIR signal embedded in it, and since I didn't _really_ need it, I just disconnected it.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-orig.jpg?raw=true" width="200" />

As a replacement, I chose an ESP-10 ESP8266 controller board running custom firmware that would connect to my MQTT server and simply listen
for a a command to fire the sprayer.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-esp10.jpg?raw=true" width="200" />

Wiring was pretty straight forward, however I did have to add a buck converter connected directly to the usb plug to power the esp-10
instead of using the sprayer's native 5v -> 3.3v converter. Apparently it did not put off enough amps to power the esp-10 reliably.

## The Results
I could not be more pleased with the results. After the first encounter with the spray, Madisyn didn't return to the laundry room for
almost a week. When she did, she was again correctly identified by the model and sprayed again. I have also had no false positives,
and Darcy doesn't even know this device exists.

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/result.gif?raw=true"/>

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/result2.gif?raw=true"/>
