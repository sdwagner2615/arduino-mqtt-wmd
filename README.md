# Weapon of Mads. (Madisyn's) Destruction
A project designed to stop Madisyn from going into the Laundry Room.

## Introduction
Meet Madisyn, a 3 year old domestic short-hair/russian blue mix with an abundance of energy.
![Ms. Madisyn](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/madisyn.jpg?raw=true)

Now meet, Mr. Darcy; an 18 year old domestic short hair who enjoys long naps in the sun and begging for food.
![Mr. Darcy](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/darcy.jpg?raw=true)

Madisyn desperately wants to play with Darcy, but he wants nothing to do with her. He'll regularlly hiss and swap her away,
but does that stop Ms. Madisyn? Absolustely not! She will be his friend no matter what! Recently she's gotten into a habit of
checking on Mr. Darcy when he uses the bathroom, Mr Darcy does not appreciate this. So much so, that I began to fear he may
stop using his litter box all together. I tried adding more litter boxes, and while she'll use any of them, he's used this one
for the last 10 years and he's not about to change now!

What I need is a way to keep Madisyn out of the Laundry Room, without preventing Darcy from entering. Enter the
[PetSafe SSSCAT Spray Deterrent](https://www.amazon.com/dp/B0721735K9), it's a motion activated air spray can meant to startle
a cat and keep them out of places you don't want them to go. Their are two problems with this however. First, the sprayer is battery
powered and I dont want to have to stay on top of changing the batteries. Since it took 4 AAA however, I figured I may be able to
convert it to USB easily enough. The second and much bigger issue, this sprayer only triggers on moition - any motion. This means
it'll spray not only Madisy but Darcy and myself! However I thought I had a solution to this problem as well, so I bought it.

## The Solution
My idea seemed simple enough, I was going to use a camera and a [custom trained Yolov5](https://github.com/ultralytics/yolov5) model
to detect which cat had entered the laundry room. If it was Madisyn, I was going to activate a Smart Plug which would power the sprayer
and allow its motion sensor to trigger. If it was a person, or Darcy, the sprayer would remain off. 

When the sprayer got here I immediately noticed a problem. When powered on, it takes 10-30 seconds before the motion sensor will trigger.
This was not acceptable, as it's possible Madisyn would already be causing trouble by the time the sprayer was ready. Upon dissasembly,
I noted that the sprayer while it was getting 5v from the batteries, the microcontroller was powered by 3.3v; the same power requirements
for an ESP8266 board. So I decided to do a full board swap, and instead of relying on motion I would issue an MQTT command when the camera
detected Madisyn, to fire the sprayer.

## The Model
Out of the box, the Yolov5 models can already alert me if a cat is detected; but I needed the model to differentiate between each
cat not just tell me "there's a cat". I started by having Blue Iris (my NVR of choice), take a photo every 2 seconds when motion was
detected. I let this run for 4 days and was able to collect over 500 pictures of Darcy, but only 100 pictures of Madisyn. Ideally I
would have liked more pictures of Madisyn to train on, I decided to [train](https://www.codeproject.com/Articles/5347827/How-to-Train-a-Custom-YOLOv5-Model-to-Detect-Objec)
on ~100 images of each cat as a first attempt and I can say I was pleasantly surprised by results:
![Version 1](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/detect.png?raw=true).

### Version 2
After running version 1 for a few days, I collected around 500 pictures of each cat and re-trained the model on around ~848 pictures total
and with 300 epochs as opposed to the 50 laid out in the earlier steps of that linked tutorial. The results was a much more accurate
model which now correctly identifies each cat with 95-97% accuracy. 

### Version 3
I added another ~1000 pictures (500 of each) from a different angle on a different camera; just to see if I could detect when my
cats were in a different part of the house. Next I'm going to look into continuous training, taking pictures from the cameras and using
previous versions of the models to generate test data for any future version.


## The Sprayer
For the sprayer I started by remove the stock microcontroller board and disconnecting the PIR sensor entirely.
![The original board](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-orig.png?raw=true)

As a replacement, I chose an ESP-10 ESP8266 controller board running some custom firmware that would connect to my MQTT server and
listen to a command to fire the sprayer.
![The new board](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-esp10.png?raw=true)

Wiring was pretty straight forward, however I did have to add a buck converter connected directly to the usb plug to power the esp-10
instead of using the native 5v -> 3.3v converter. Apparently it did not put off enough amperage to power the esp-10 reliably.

## The Results
I could not be more pleased with the results. After the first encounter with the spray, Madisyn didn't return to the laundry room for
almost a week. When she did, she was again correctly identified by the model and sprayed again. I have also had no false positives,
and Darcy remains completely oblivious to the presecense of the device.

![result](https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/result.gif?raw=true)
