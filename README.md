# Weapon of Mads. (Madisyn's) Destruction
A project designed to stop Madisyn from going into the Laundry Room.

## Introduction
Meet Madisyn, a 3 year old domestic short-hair/russian blue mix with an abundance of energy.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/madisyn.jpg?raw=true" width="600" />

Now meet, Mr. Darcy; an 18 year old domestic short hair who enjoys long naps in the sun and begging for food.
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/darcy.jpg?raw=true" width="600" />

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
<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/detect.png?raw=true" width="600" />

### Version 2
After running version 1 for a few days, I collected around 500 additional pictures of each cat and re-trained the model on around ~848
pictures total. I also increased the epochs to 300 as opposed to the 50 laid out in the earlier steps of that linked tutorial. The results
was a much more accurate model which now was able to correctly identifies each cat with 95-97% accuracy. 
> Looking back after training 5 additional versions of this model I'm realizing that this model is probably the perfect version to solve
> _this_ problem. I believe this model is over-fit to the Laundry Room camera's perspective and lighting but that also makes it the most
> effective version to solve this problem. I plan to fallback to this version for that camera if version 7 proves to be inefficient.

### Version 3
I added another ~1000 pictures (500 of each) from a different camera; just to see if I could detect when my cats were in a different
part of the house. The model seems to be equally as accurate when running in my laundry room, and much more accurate when detecting
the cats on this additional camera. Next I'm going to look into continuous training, taking pictures from these cameras and using previous
versions of the models to generate test data for any future training runs.
> After running this for a few days I discovered that starting with version 3 the model started to confuse background objects for Madisyn.
> Oddly enough, this seems to only be a problem with her and my assumption is given that she's a dark colored cat that any dark colored
> object or shadow was confusing enough for the model to just tag it as Madisyn.

### Version 4 and 5
I had CodeProject.AI feed predictions into a local instance of LabelStudio using some manually edited code. Version 4 and 5 of the models
were retrained using version 3 and 4 respectively as the base weights and on only these auto-annotated images as training data. However,
these versions while becoming more accurate on other cameras have started to become less accurate on the laundry room camera. I'm going
to combine all images from all model training runs (going all the way back to the original 100 images uses for version 1) and try training
a new version on this dataset to see if I get the improvement on other cameras without sacrificing the core use-case in the laundry room.
> These versions also still confused background images (dark objects and spaces) for Madisyn on occasion.

### Version 6-1/6-2
For version 6 I combined all images I have so far into one massive training set totaling around 6k images and used it to train two variants
of the model. Version 6-1, was trained using yolov5s as the base weight, while version 6-2 was trained using version 5 of the model
as the base. Version 6-1 trained on 300 epochs, while version 6-2 finished after 265 epochs with a message stating "no improvement" had been
seen for the last 100 epochs. When I did some spot checks on these two models I actually found 6-1 to be more accurate and rolled that out to
all my cameras (including the laundry room). This was the first version of the model since version 2 I felt comfortable enough in the laundry
room as it didn't perform dramatically worse than version 2 (but it did have lower confidence scores than version 2).
> In this version the confusion matrix showed a significant improvement in the models ability to differentiate background images from Madisyn
> but annoyingly version 6 started confusing background images for Darcy too (albeit not as much as Madisyn though: 0.45 vs 0.55).

### Version 7
After I rolled version 6-1 out, I let label studio collect another 3-4k images before re-training. At this point I had almost 10k images total
and decided that this would be the final version of the model. I achieved the task I set out to do, and at this point all 3 of the cameras
running these models were doing a good-enough job at differentiating the cats. With this last training, I was hoping to keep the model from
confusing background images for Madisyn but the training results still showed that to be a problem. Confusing background images for Darcy
improved (down to 0.22) but Madisyn's increased when compared to version 6 (0.78). I have rolled this model out to all cameras, and plan to
review the predictions coming in to label studio over the coming days. The plan is to revert the Laundry Room back to version 2 if I see any
issues but leave version 7 on all other cameras for the forseeable future.


## The Sprayer
For the sprayer I started by removing the stock micro-controller board and ended up disconnecting the PIR sensor entirely. Initially, I
wanted to pass the PIR data into MQTT as additional sensor data but since the stock micro-controller board had all the relevant circuitry to
boost the PIR signal embedded in it, and since I didn't _really_ need it, I just disconnected it.

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-orig.jpg?raw=true" width="600" />

As a replacement, I chose an ESP-10 ESP8266 controller board running custom firmware that would connect to my MQTT server and simply listen
for a a command to fire the sprayer.

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/sprayer-esp10.jpg?raw=true" width="600" />

Wiring was pretty straight forward, however I did have to add a buck converter connected directly to the usb plug to power the esp-10
instead of using the sprayer's native 5v -> 3.3v converter. Apparently it did not put off enough amps to power the esp-10 reliably.

## The Results
I could not be more pleased with the results. After the first encounter with the spray, Madisyn didn't return to the laundry room for
almost a week. When she did, she was again correctly identified by the model and sprayed again. I have also had no false positives,
and Darcy doesn't even know this device exists.

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/result.gif?raw=true"/>

<img src="https://github.com/sdwagner2615/arduino-mqtt-wmd/blob/main/images/result2.gif?raw=true"/>
