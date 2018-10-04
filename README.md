# Roguelike
## Description
A data driven roguelike where players traverse through various levels, fight monsters, and get loot. More information can be found on my [portfolio](https://www.aahernandez.net/roguelike).

## Controls
'↑'&ensp;&ensp;&ensp;-&ensp;&ensp;Moves the main player north if possible, or attack a character to north if there is one.  
'←'&ensp;&ensp;-&ensp;&ensp;Moves the main player west if possible, or attack a character to west if there is one.  
'↓'&ensp;&ensp;&ensp;-&ensp;&ensp;Moves the main player south if possible, or attack a character to south if there is one.  
'→'&ensp;&ensp;-&ensp;&ensp;Moves the main player east if possible, or attack a character to east if there is one.  

'B'&ensp;&ensp;&ensp;-&ensp;&ensp;This will bring up the aiming assist feature if a ranged weapon is equipped. A glowing red icon will appear on an  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;enemy that is within range.  
&ensp;&ensp;&ensp;&ensp;'→'&ensp;&ensp;-&ensp;&ensp;This will place the glowing red icon on the next visible enemy if there is one, thus changing the ranged weapon's  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;target.  
&ensp;&ensp;&ensp;&ensp;'←'&ensp;&ensp;-&ensp;&ensp;This will place the glowing red icon on the previous visible enemy if there is one., thus changing the ranged  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;weapon's target.  
&ensp;&ensp;&ensp;&ensp;'B'&ensp;&ensp;&ensp;-&ensp;&ensp;This will fire the ranged weapon if you are currently aiming at an enemy.  

'P'&ensp;&ensp;&ensp;-&ensp;&ensp;Starts a test path generation.  
&ensp;&ensp;&ensp;&ensp;'SPACE'&ensp;&ensp;-&ensp;&ensp;Runs a step in path generation.  
&ensp;&ensp;&ensp;&ensp;'ENTER'&ensp;&ensp;-&ensp;&ensp;Instantly completes path generation.  

In Stepped Map Generation from Main Menu.  
&ensp;&ensp;&ensp;&ensp;'SPACE'&ensp;&ensp;-&ensp;&ensp;Runs one generator to completion.  
&ensp;&ensp;&ensp;&ensp;'ENTER'&ensp;&ensp;-&ensp;&ensp;Instantly runs and completes any remaining map generators.  

'Tab'&ensp;-&ensp;&ensp;Holding this will allow you to view your inventory.  
'E'&ensp;&ensp;&ensp;-&ensp;&ensp;This will bring up a list of usable items. Each item will have a letter in front of it. Press that corresponding letter in  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;order to use the item. Some items can be equipped into equip slots. Other items, like potions, can apply one time  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;effects.  
'U'&ensp;&ensp;&ensp;-&ensp;&ensp;This will allow you to unequip any of your equipped items. After pressing this key, a letter will be displayed  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;in front of every equipped item. Pressing the corresponding will unequip that item.  
'D'&ensp;&ensp;&ensp;-&ensp;&ensp;This will bring up a list of all your items (equipped and unequipped). Each item will have a letter in front of it.  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;Pressing the corresponding letter will drop that item. If an equipped item is dropped, it will first be unequipped, and  
 &ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;then dropped.  

'Esc'&ensp;&ensp;&ensp;-&ensp;&ensp;This will exit the game from the main menu, or return to the main menu from the game.  

## Run
An executable is provided in the [Run_Win32](Roguelike/Run_Win32) folder.

## Build
The project was built using Visual Studio 17. If you would like to build it yourself, you can open the visual studio [solution](Roguelike/Roguelike.sln) to do so. You must also have a copy of my [engine](https://github.com/aahernandez/Engine) to build it, and the engine and roguelike project must both have the same parent folder like so:

|—parent folder  
&ensp;&ensp;&ensp;&ensp;|—Roguelike  
&ensp;&ensp;&ensp;&ensp;|—Engine  