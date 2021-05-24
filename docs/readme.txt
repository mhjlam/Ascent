*******************************************************************************
						ASCENT README (15 April 2011)
*******************************************************************************

*******************************************************************************
*    ABOUT    *
***************

Ascent is a first-person shooter in which the player takes control of a space
ship. The player is able to move in six degrees of freedom and has several 
different firing capabilities. 

The goal is to reach the exit of each level, and getting a higher score by 
defeating enemies.



*******************************************************************************
*    AUTHORS    *
*****************

- Jannes Beurskens
- Marco van Laar
- Maurits Lam

This game was made as an assignment for the course Game Engine Programming 
(2010/2011), part of the master Game & Media Technology at Utrecht University.



*******************************************************************************
*    TASK DIVISION    *
***********************

Jannes Beurskens: 	enemies, saving/loading
Marco van Laar:		map creation, collision detection, map overview
Maurits Lam: 		movement/shooting, collision detection, map overview



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

TAB			toggle map overview
ENTER		next level (if available)
BACKSPACE	restart level
F1			save game
F2			load game
ESCAPE		exit the game


									Mouse
===============================================================================
Move the mouse to look around (yaw & pitch)

LMB			fire turret (hold to keep firing)
RMB			change turret type

available turret types: single shot, alternating shots, dual shot, triple shot



*******************************************************************************
*    DEPENDENCIES    *
**********************

- The source code, and in particular the serialization module, is dependent on 
boost_1_42. The headers and lib files have been provided with the source code.



*******************************************************************************
*    INTEGRATION    *
*********************

- (OGRE_HOME) should be set as an environment variable and pointing to the 
OgreSDK_vc10_v1-7-2 install directory, as it is used in the project settings.

- The boost_1_42 directory should be in the same directory as Ascent in order 
to be able to compile/build the source code.

- All necessary materials are included in the local media directory.



*******************************************************************************
*    ADDITIONAL NOTES    *
**************************

- The game may be quite dark when viewing on a monitor in a bright environment,
but this is by design.

- The Ogre configuration file is created only once at first startup. After 
running the game once, you can edit the ogre.cfg file manually or delete it to 
show the Ogre configuration tool again.

- For the best results use Direct3D as rendering system. Fog effects may 
exhibit glitches when rendering with OpenGL.

- It is recommended to turn vsync on, especially on fast computers, as this 
will limit the frame rate to 60.
