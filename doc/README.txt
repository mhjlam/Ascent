*******************************************************************************
						ASCENT README (15 April 2011)
*******************************************************************************

*******************************************************************************
*    ABOUT    *
***************

Ascent is a first-person shooter in which the player takes control of a space-
ship. The player is able to move in six degrees of freedom and has several 
different firing capabilities. 

The goal is to reach the exit of each level, and getting a higher score by 
defeating enemies.



*******************************************************************************
*    AUTHORS    *
*****************

- Jannes Beurskens	(3649598)
- Marco van Laar	(3652246)
- Maurits Lam		(3644383)

This game was made as an assignment for the course Game Engine Programming 
(2010/2011), part of the master Game & Media Technology at Utrecht University.



*******************************************************************************
*    TASK DIVISION    *
***********************

Jannes Beurskens: 	NPCs, serialization
Marco van Laar:		collision detection, map creation, map overview
Maurits Lam: 		movement, shooting, collision detection, map overview



*******************************************************************************
*    CONTROLS    *
******************

								   Keyboard
===============================================================================
W			move forward
S 			move backward
A			strafe left
D			strafe right
Q			roll left
E			roll right
SPACE		move up
LEFT CTRL	move down

TAB			switch overview mode on/off
ENTER		next level (if it exists)
BACKSPACE	restart level
F1			save game
F2			load game
ESCAPE		exit the game


									Mouse
===============================================================================
Move the mouse to look around (yaw & pitch)

LMB			click to shoot (hold to keep firing)
RMB			click to change turret mode

			turret modes: single shot, alternating, dual shot and triple shot



*******************************************************************************
*    DEPENDENCIES    *
**********************

- The source code is dependent on boost_1_42, the serialization module in
particular. The headers and lib files have been provided in the archive.



*******************************************************************************
*    INTEGRATION    *
*********************

- (OGRE_HOME) should be set as an environment variable and pointing to the 
OgreSDK_vc10_v1-7-2 install directory, as it is used in the project settings.

- The boost_1_42 directory (included in the archive) should be in the same 
directory as Ascent in order to be able to compile/build the source code.

- All materials are present in the local media directory.



*******************************************************************************
*    ADDITIONAL NOTES    *
**************************

- The game may be quite dark when viewing the screen in bright areas, but this 
is by design.

- The Ogre configuration file is created only once at first startup. After 
running the game once, you can edit the ogre.cfg file manually or delete it to 
show the Ogre configuration tool again.

- We seem to be getting the best results while using Direct3D as rendering 
system as the fog effects may show glitches when using OpenGL.

- On fast computers, we recommend switching vsync on in order to get a 
consistent frame rate.
