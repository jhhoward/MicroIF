# MicroIF
MicroIF is a simple interactive fiction engine for microcontrollers. It is designed to fit into small memory footprints.

## Scripting language

Here is an example script:
```
# An example script
# Everything on the line after a # is a comment
# All white space is ignored but can be used to make code easier to read

# Variables are single bit flags that can either be true or false
flag doorIsLocked is true

# Items are things that can be collected and stored in the player's inventory to use later
item key
	# The title and description attributes defines how the item is displayed in the inventory
	title "key"
	description "A shiny, copper coloured key."
	
	# This event is called when the item can otherwise not be used
	on use
		say "There is nowhere to use the key here."
		
# Rooms are places where the player can visit. The first room defined is the starting room
room kitchen
	title "The kitchen"
	# Attributes can have conditionals associated with them
	description when doorIsLocked
		"You are in the kitchen. It looks quite messy. The only door out of here is locked!"
	description when not doorIsLocked
		"You are in the kitchen. It looks quite messy. A door to the east is open."
		
	# Options are displayed to the player to choose from
	option "fridge"
		say "Nothing inside the fridge except a broken ketchup bottle."
	
	# Options can have conditionals
	option "east" when doorIsLocked
		say "The door is blocking your path."
	option "east" when not doorIsLocked
		go diningRoom
	
	# Conditionals can be based on item possession
	option "cupboard" when not have key
		say "Looking inside the cupboard you find a key."
		give key
	option "cupboard" when have key
		say "Nothing else in the cupboard worth taking."
		
	# Events for handling item usage
	on use key when doorIsLocked
		say "You unlock the door."
		unset doorIsLocked
	on use key when not doorIsLocked
		say "The door is already unlocked"
		

room diningRoom
	title "The dining room"
	description "A long table with chairs. The kitchen is to the west."
	
	# You can assign events for entering a room
	on enter
		say "You just walked into the dining room!"
	
	option "west"
		go kitchen
```

## VM details

### Variables
There are 256 1-bit flag variables. These are a combination of item status (have item / not have item) and general purpose logic flags.
The first N flags are reserved for however many items that have been defined. e.g. flag 3 == whether item at index 3 is possessed

There a one byte 'current room' variable that is modified by the 'go' command

### Game data layout:
```
[32 byte] Default flag values for the 256 1-bit flag variables
[1 byte] Number of object data blocks (rooms and items)
[1 byte] Number of items
[1 byte] Index for starting room
For each object data block:
	[2 byte] Little endian pointer offset to object data block
For each object data block:
	[N bytes] Collection of VM instructions terminated by a null instruction (0)
```

### Object data blocks
Each item and room has its own object data block. Items always come first in the list of objects, followed by rooms.
Object data blocks are split into 'event' and 'option' handlers

### Events
Event handlers are for specific game events such as entering a room or using an item. They are also used for retrieving [name] and [description] attributes.
The first N event IDs represent using an item. i.e. event id 3 == what to do when using item at index 3

### Options
Option handlers are for the various options available to the player in a given room.

### Conditions
Events and options can have a condition associated with them based on the flag variables. This determines whether events should be handled or options should be visible.

### Execution
The VM can be executed in three modes:
- Enumerate options : builds a list of available options for a given room
- Trigger event [id]
- Choose option [id]

The VM will be placed in a yield state when player input is required (i.e. when displaying a message to the player)

Attributes (i.e. [name] and [description]) are retrieved by triggering the appropriate event on a room / item. 
This means that they can change at run time based on flag states by using a conditional.


## Strings and compression

The string table is just a collection of strings, separated by NUL (\0) characters.
Strings in the VM instructions are pointer offsets from the start of the string table.
Characters in the range [0-127] map directly to ASCII values.
Characters in the range [128-255] map to commonly used phrases / substrings

The first (up to 128) strings in the string table are used to store the commonly used phrases.

Pseudo code to decode a string:

```
print(string):
	for each character:
		if in range[0-127]:
			print character
		else:
			N = character - 128
			print(Nth string in string table)
```

Note that the above code can be recursive when resolving commonly used phrases

Typical compression rate is ~70%
Maybe not the best compression rate but is designed so that decoding strings is as simple as possible


