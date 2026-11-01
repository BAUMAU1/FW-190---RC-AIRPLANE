This is the repository for a DIY 3D-printable airplane with the looks of the german WW2 fighter FW190.
On here you can find everything necessary to build one yourself, aswell as a detailed description of the planes functions.



How the plane works:
An ESP32 S3 CAM hosts a controller, reads its data and then sends the wanted controllerdata to another ESP32 S3 CAM per radio.
The receiving ESP32 sits on a pcb on an electronics tray inside the plane, it processes the incoming radio signals and outputs them to the wanted servo in the plane.
The airplane itself has retractable gears, two aileroins with one servo each, elevator flaps and a motor.
The aileroins can be used as an airbrake.
There is no controllable rudder, becuase it is not necessary for flight, and there was too little space on the axis of the rudder to place any functioning component.
The electronics tray on which the pcb is mounted is fully removable and also holds the battery and ESC-controller.
The tray is tucked in place behind the nose of the plane.
The planes nose is screwed on the rest of the body.

For anyone reviewing this on stardance:
The back steering mechanism is not fully finished, which is why there is no actual footage of the plane flying yet.
The reason for that, is because the potentiometer used in that mechanism was too rigid, so no gears would turn or the motor would not stop spinning because of a lack of feedback. (When testing with other potentiometer the mechanism worked)
