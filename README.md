# Capstone Project
Teddy, Sasha, Sean, Neil, Amery

Working with four other Northeastern University students, I developed a prototype sensory feedback device for use with virtual reality. I pushed for a familiar method of feedback and came up with the idea of using pneumatics to apply pressure to the hand. I worked on the project from its initial conception through the synthesis of the prototype and demo. To our excitement, the project earned second place at the semesters Capstone design competition. The system precisely distributes pneumatic pressure to a collection of pouches on a glove. The pouches are inflated and deflated based on commands received over a serial connection from a computer. Leveraging the large number of GPIOs on the Arduino Mega, it rapidly measures the pressure of each pouch and actuates specific solenoid valves to maintain a constant experience for the user. We integrated the serial communication in a demo using Unity and the Leap Motion allowing users to feel objects they touch in VR.

![Working Prototype](./Capstone-Angle.jpg)

### Developer Setup Instructions

Install [Processing 3](https://processing.org/download/?processing) preferably for all users

Clone this repository to your processing sketchbook location (File --> Preferences --> Sketchbook Location)

Clone the [Leap Motion Processing Library](https://github.com/nok/leap-motion-processing) to your sketchbook location's libraries folder

Create a symbolic link from "leap-motion-processing" to "LeapMotionForProcessing"

Windows Symbolic Link Command:
```
mklink /j ...\libraries\LeapMotionForProcessing ...\libraries\leap-motion-processing
```

#### Additional Instructions for Sublime Text

Install [Sublime Text 3](http://www.sublimetext.com/3)

Launch Sublime Text 3 and go to (Preferences --> Settings - User) and add the following lines to the hash
```
  "tab_size": 2,
  "translate_tabs_to_spaces": true,
  "spell_check": true
```
Next type ctrl and ` to open the Sublime Console

Enter the Sublime Text 3 [Package Control](https://packagecontrol.io/installation) text into the console

After the installation of the Package Controller is done restart Sublime

Then install the Sublime Text Processing Package (Preferences --> Package Control --> Install Package --> Processing)

Add Processing 3 folder to your System's PATH as shown [here](https://github.com/b-g/processing-sublime)

Restart Sublime one more time

Open the Capstone repository in Sublime and build the project (Tools --> Build)

### Other Resources

Processing Serial Documentation:

https://processing.org/reference/libraries/serial/index.html
