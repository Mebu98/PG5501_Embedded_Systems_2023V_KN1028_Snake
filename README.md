# PG5501_Embedded_Systems_2023V_KN1028_Snake
Exam in PG5501 Embedded Systems spring 2023, final grade B.

Starting the exam I thought I'd create a relativly simple snake game and thus didn't (forgot to) create a git repo, which later ended up biting me at least once... 
I have thus gained newfound respect for VCS, and learned it's importance, even for projects that are expected to be small of scale.

Task I ended up choosing is (translated to english):
# Task B - Game Console
Use the screen and SD card to build your own little game console! The game does not need to be based on an original idea, but all code must be written by you. 
The playability of the game will be an important part of the evaluation, so make sure the game works well. 
The requirements are:

* You MUST use a TFT screen and buttons
* Feel free to use an accelerometer, joystick, or other equipment in the kit. (You can only use components that came with the component set).
* Libraries you find online are of course allowed to be used, but the game itself cannot use code from others.
* Feel free to use sound and light to enhance the experience.
* You can choose whether to use Arduino UNO or ESP32.
* Remember to create and post a video, documentation, and wiring diagram (e.g. from Fritzing).
* If you make a copy of a very simple game - feel free to add some elements that differentiate your game from the original.


# Parts list
* Arduiono Uno R3 by Kuongshun
* TFT screen: Adafruit 240x135 color TFT model ST7789 1.14"
* Joystick: HW-504
* IR sensor: HW-477 v0.2
* Remote: I don't know, but it's from the UNO R3 project kit....

# Fritzing wiring diagram
![fritzing_bb](https://github.com/Mebu98/PG5501_Embedded_Systems_2023V_KN1028_Snake/blob/main/fritzing/fritzing_bb.png)

Not certain why I chose the wire colours I did, but it is what it is...
In the diagram a 160x80 TFT screen is used, but they both have the same pins if I remember correctly, so it doesn't "really" matter (Also not certain if someone has made a Fritzing part for the 240x135).

# Videos

https://github.com/Mebu98/PG5501_Embedded_Systems_2023V_KN1028_Snake/assets/89260657/66e1fb14-d026-409d-ad6a-74b5dbcc7ca4

https://github.com/Mebu98/PG5501_Embedded_Systems_2023V_KN1028_Snake/assets/89260657/6a32a7cd-23ff-48f0-aa28-9c02b2dddaad

# Feedback
Norwegian: 
"Jeg har sett mange snake-spill opp gjennom årene, men dette er av de bedre. 
God spillbarhet, god framerate og pent/smart tegnet brett gjør at denne er over snittet. 
IR-fjernkontroll funker brukbart men er mye dårligere enn joystick. Greit tillegg uansett? 
Jeg liker måten spillet er løst på (omtrent som jeg ville gjort det selv) og det fungerer godt. Bra jobba!"

English translation:
"I have seen many snake games over the years, but this is one of the better ones. 
Good playability, good framerate, and a pretty/smart drawn board make this game above average. 
The IR remote control works decently but is much worse than a joystick. A decent addition anyway? 
I like the way the game is solved (almost like I would have done it myself) and it works well. Well done!"

# Retrospective
Looking back I would obviously have made it a git project from the start, but what's done is done.

I'd have reset the high scores in the videos to showcase that it actually increases as it should, and not just a static number, as well as maybe trying to get some better video quality.

I also think I might have gotten a better grade if I found some way to implement an SD card, by for example saving the high scores to it or some sort of "game over" animation, 
but I was afraid that it'd take a lot of precious time so I went for the easier IR remote instead.

Ultimatly I'm happy with the grade I ended up getting. :)
