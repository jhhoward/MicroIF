// Default flag values
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,				// num items
0x0c,				// num rooms

// cartridge
0x74,0x03,			// "Cartridge"
0x71,0x06,			// "This is a data storage cartridge.  On it are the words 'Astral Body Formation: The Untold Story.'"
// keycard
0x72,0x04,			// "Keycard"
0x49,0x02,			// "A flat rectangular piece of synthetic material, magnetically keyed for passing check stations."
// magnet
0x88,0x04,			// "Magnet"
0xe7,0x02,			// "A magnet"
// intro [0]
0xe0,0x04,			// "Space Quest Chapter 1"
0xff,0xff,			// ""
0x76,0x00,			// data offset
// dead [1]
0x83,0x03,			// "Game over!"
0xff,0xff,			// ""
0x7e,0x00,			// data offset
// closet [2]
0x69,0x04,			// "Janitor's closet"
0x1e,0x03,			// "A small, dark and very cluttered utility closet"
0x8d,0x00,			// data offset
// upperDeck [3]
0x64,0x07,			// "Upper deck"
0xc8,0x04,			// "One of the many hallways of the Arcada."
0x93,0x00,			// data offset
// library [4]
0x7c,0x04,			// "Library"
0xad,0x06,			// "This is the Arcada's data archive. Many volumes of information are stored on data cartridges."
0xa9,0x00,			// data offset
// computer [5]
0x7e,0x04,			// "Library computer"
0xa5,0x05,			// "The console consists of a screen, keyboard and cartridge slot. Atop it rests a Model DX Cartridge Retrieval Unit"
0xf0,0x00,			// data offset
// corridorEnd [6]
0x7c,0x03,			// "Dead end"
0x38,0x06,			// "The hallway ends here. The corpse of one of your former crewmates lies here. He is nonfunctional"
0x05,0x01,			// data offset
// upperElevators [7]
0x69,0x07,			// "Upper deck elevators"
0x2a,0x07,			// "Two elevators connect to the lower deck"
0x26,0x01,			// data offset
// lowerElevators [8]
0x82,0x04,			// "Lower deck elevators"
0x37,0x07,			// "Two elevators connect to the upper deck"
0x36,0x01,			// data offset
// lab [9]
0x78,0x04,			// "Laboratory"
0xda,0x06,			// "This is the Star Generator development laboratory.  Due to your incredibly low security clearance you have never been allowed access to this room."
0x46,0x01,			// data offset
// centralControl [10]
0x77,0x03,			// "Central control"
0xc3,0x07,			// "You are in the central control area of the Arcada. The reactor domes pulsate irregularly, as if they had been tampered with."
0x76,0x01,			// data offset
// outsideFlightPrep [11]
0xd9,0x04,			// "Outside flight prep"
0xd7,0x04,			// "Outside"
0xdb,0x01,			// data offset
// intro data
0x0c,0x00,			// on enter
0x02,0x3c,0x05,		// say "The Sarien Encounter  Demake by James Howard Original game designed by Mark Crowe and Scott Murphy."
0x01,0x02,			// go closet
0x00,				// end
// dead data
0x0c,0x00,			// on enter
0x02,0xeb,0x04,		// say "Thank you for playing Space Quest. Too bad you've failed miserably and doomed all your people to a horrible death at the hands of the Sariens."
0x02,0xfa,0x03,		// say "If you continue playing as skillfully as this, we'll never have a chance for a sequel. Better luck next time."
0x03,0x2b,0x09,		// option "restart"
0x02,0xa7,0x04,		// say "Not yet implemented!"
0x00,				// end
// closet data
0x03,0x33,0x09,		// option "south"
0x01,0x03,			// go upperDeck
0x00,				// end
// upperDeck data
0x0c,0x00,			// on enter
0x05,				// when
0x07,				// not
0x06,0x03,			// eval alarmSounded
0x02,0xfc,0x07,		// say "You are startled by the sound of an alarm. It is followed by an urgent voice which warns that the Arcada has been boarded by unknown intruders. It ends abruptly."
0x0a,0x03,			// set alarmSounded
0x03,0x20,0x09,		// option "north"
0x01,0x02,			// go closet
0x03,0x3a,0x09,		// option "west"
0x01,0x04,			// go library
0x00,				// end
// library data
0x0c,0x00,			// on enter
0x05,				// when
0x07,				// not
0x06,0x06,			// eval visitedLibrary
0x0a,0x06,			// set visitedLibrary
0x0c,0x00,			// on enter
0x05,				// when
0x06,0x06,			// eval visitedLibrary
0x08,				// and
0x07,				// not
0x06,0x04,			// eval scientistEntered
0x0a,0x04,			// set scientistEntered
0x02,0xed,0x05,		// say "The door opens and a man you recognise as one of the lab scientists enters. He appears to be injured! After only a few steps he slumps to the floor."
0x03,0x13,0x09,		// option "east"
0x01,0x03,			// go upperDeck
0x03,0x3a,0x09,		// option "west"
0x01,0x07,			// go upperElevators
0x03,0x01,0x09,		// option "computer"
0x01,0x05,			// go computer
0x03,0x2f,0x09,		// option "scientist"
0x05,				// when
0x06,0x04,			// eval scientistEntered
0x08,				// and
0x07,				// not
0x06,0x05,			// eval spokenToScientist
0x02,0x8b,0x02,		// say "A large laser hole has been burned in his uniform through which you can see previously unexposed tissue. Struggling painfully he raises up on one elbow."
0x02,0xc0,0x03,		// say "He tells you the Arcada is under attack and the Star Generator is in danger. You had better leave if you value your life."
0x02,0x8c,0x03,		// say "He looks over toward the shelves full of cartridges and utters the words 'astral body'. He then settles to the floor, lifeless."
0x0a,0x05,			// set spokenToScientist
0x03,0x2f,0x09,		// option "scientist"
0x05,				// when
0x06,0x04,			// eval scientistEntered
0x08,				// and
0x06,0x05,			// eval spokenToScientist
0x02,0x45,0x04,		// say "It's a grisly sight. There is nothing you can do for him now."
0x00,				// end
// computer data
0x03,0x16,0x09,		// option "exit"
0x01,0x04,			// go library
0x03,0xfd,0x08,		// option "astral body"
0x05,				// when
0x06,0x05,			// eval spokenToScientist
0x08,				// and
0x07,				// not
0x06,0x00,			// have cartridge
0x02,0xeb,0x02,		// say "A robot arm springs to action and retrieves a cartridge from one of the many shelves. You take the cartridge."
0x0a,0x00,			// give cartridge
0x00,				// end
// corridorEnd data
0x03,0x13,0x09,		// option "east"
0x01,0x07,			// go upperElevators
0x03,0x05,0x09,		// option "corpse"
0x05,				// when
0x07,				// not
0x06,0x01,			// have keycard
0x02,0x88,0x08,		// say "You search the body of your crewmate and find..."
0x02,0xf7,0x08,		// say "a keycard."
0x0a,0x01,			// give keycard
0x03,0x05,0x09,		// option "corpse"
0x05,				// when
0x06,0x01,			// have keycard
0x02,0x88,0x08,		// say "You search the body of your crewmate and find..."
0x02,0x26,0x09,		// say "nothing."
0x00,				// end
// upperElevators data
0x03,0x13,0x09,		// option "east"
0x01,0x04,			// go library
0x03,0x3a,0x09,		// option "west"
0x01,0x06,			// go corridorEnd
0x03,0x0e,0x09,		// option "down"
0x01,0x08,			// go lowerElevators
0x00,				// end
// lowerElevators data
0x03,0x37,0x09,		// option "up"
0x01,0x07,			// go upperElevators
0x03,0x20,0x09,		// option "north"
0x01,0x09,			// go lab
0x03,0x13,0x09,		// option "east"
0x01,0x0a,			// go centralControl
0x00,				// end
// lab data
0x03,0x33,0x09,		// option "south"
0x01,0x08,			// go lowerElevators
0x03,0x05,0x09,		// option "corpse"
0x02,0x88,0x08,		// say "You search the body of your crewmate and find..."
0x02,0x26,0x09,		// say "nothing."
0x03,0x01,0x09,		// option "computer"
0x02,0xa4,0x08,		// say "Your mind is boggled in attempting to understand the function of the various buttons, switches, lights, and displays. It appears the board is not operating."
0x03,0x31,0x09,		// option "screen"
0x02,0x6f,0x07,		// say "WARNING! Self-Destruct Mechanism Has Been Armed! Evacuate Ship Immediately! THIS IS NOT A TEST!"
0x03,0x09,0x09,		// option "desk"
0x05,				// when
0x07,				// not
0x06,0x02,			// have magnet
0x02,0xba,0x04,		// say "On the desk you find a magnet."
0x0a,0x02,			// give magnet
0x03,0x09,0x09,		// option "desk"
0x05,				// when
0x06,0x02,			// have magnet
0x02,0x5d,0x06,		// say "There is nothing else on the desk worth taking."
0x00,				// end
// centralControl data
0x0c,0x00,			// on enter
0x05,				// when
0x07,				// not
0x06,0x07,			// eval avoidedGuard
0x02,0x98,0x08,		// say "You think you hear footsteps."
0x03,0x13,0x09,		// option "east"
0x05,				// when
0x07,				// not
0x06,0x07,			// eval avoidedGuard
0x02,0x3d,0x03,		// say "An alien intruder appears brandishing a large pulse rifle."
0x02,0x59,0x03,		// say "BANG! In one shot you hit the ground lifeless."
0x02,0x85,0x05,		// say "The alien observes his handiwork briefly, then leaves."
0x01,0x01,			// go dead
0x03,0x13,0x09,		// option "east"
0x05,				// when
0x06,0x07,			// eval avoidedGuard
0x01,0x0b,			// go outsideFlightPrep
0x03,0x3a,0x09,		// option "west"
0x05,				// when
0x07,				// not
0x06,0x07,			// eval avoidedGuard
0x02,0x3d,0x03,		// say "An alien intruder appears brandishing a large pulse rifle."
0x02,0x59,0x03,		// say "BANG! In one shot you hit the ground lifeless."
0x02,0x85,0x05,		// say "The alien observes his handiwork briefly, then leaves."
0x01,0x01,			// go dead
0x03,0x3a,0x09,		// option "west"
0x05,				// when
0x06,0x07,			// eval avoidedGuard
0x01,0x08,			// go lowerElevators
0x03,0x1b,0x09,		// option "hide"
0x05,				// when
0x07,				// not
0x06,0x07,			// eval avoidedGuard
0x02,0x6a,0x08,		// say "You quickly duck behind a desk out of view."
0x02,0x51,0x08,		// say "You hear the footsteps of one of the alien intruders getting louder!"
0x02,0x44,0x07,		// say "Unable to see anyone here, the alien leaves to search elsewhere."
0x0a,0x07,			// set avoidedGuard
0x03,0x1b,0x09,		// option "hide"
0x05,				// when
0x06,0x07,			// eval avoidedGuard
0x02,0x6a,0x08,		// say "You quickly duck behind a desk out of view."
0x02,0x8c,0x04,		// say "Nobody comes! You come back out of hiding."
0x03,0x05,0x09,		// option "corpse"
0x02,0x88,0x08,		// say "You search the body of your crewmate and find..."
0x02,0x26,0x09,		// say "nothing."
0x00,				// end
// outsideFlightPrep data
0x03,0x3a,0x09,		// option "west"
0x01,0x0a,			// go centralControl
0x00,				// end
