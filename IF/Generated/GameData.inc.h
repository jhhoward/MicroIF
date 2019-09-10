// Default flag values
0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x14,				// num objects
0x04,				// num items
0x0c,				// starting room : intro
// cartridge [0]
0x4b,0x00,			// data offset
// gadget [1]
0x5b,0x00,			// data offset
// keycard [2]
0x87,0x00,			// data offset
// magnet [3]
0x97,0x00,			// data offset
// centralControl [4]
0xa7,0x00,			// data offset
// closet [5]
0xfd,0x00,			// data offset
// computer [6]
0x0d,0x01,			// data offset
// corridorEnd [7]
0x3c,0x01,			// data offset
// dead [8]
0x67,0x01,			// data offset
// demoFinished [9]
0x81,0x01,			// data offset
// escapePod [10]
0x8f,0x01,			// data offset
// flightPrep [11]
0xf4,0x01,			// data offset
// intro [12]
0x46,0x02,			// data offset
// lab [13]
0x79,0x02,			// data offset
// library [14]
0xb3,0x02,			// data offset
// lowerElevators [15]
0x10,0x03,			// data offset
// outsideFlightPrep [16]
0x2a,0x03,			// data offset
// upperDeck [17]
0x6a,0x03,			// data offset
// upperElevators [18]
0x8a,0x03,			// data offset
// vehicleBay [19]
0xa4,0x03,			// data offset
// cartridge data
0x0b,0xfe,			// title
0x0c,0x24,0x11,		// "cartridge"
0x0b,0xff,			// description
0x0c,0xf2,0x0b,		// "This is a data storage cartridge.  On it are the words 'Astral Body Formation: The Untold Story.'"
0x0b,0x00,			// use cartridge
0x02,0x75,0x0e,		// say "You can't use that here!"
0x00,				// end
// gadget data
0x0b,0xfe,			// title
0x0c,0x54,0x11,		// "gadget"
0x0b,0xff,			// description
0x04,				// when
0x06,				// not
0x05,0x09,			// eval gadgetOn
0x0c,0x91,0x0d,		// "Written on the gadget are the words 'dialect translator'. It is currently off."
0x0b,0xff,			// description
0x04,				// when
0x05,0x09,			// eval gadgetOn
0x0c,0xb6,0x0d,		// "Written on the gadget are the words 'dialect translator'. It is currently on."
0x0b,0x01,			// use gadget
0x04,				// when
0x06,				// not
0x05,0x09,			// eval gadgetOn
0x02,0x58,0x10,		// say "You turn a dial on the gadget. A light begins to glow."
0x09,0x09,			// set gadgetOn
0x0b,0x01,			// use gadget
0x04,				// when
0x05,0x09,			// eval gadgetOn
0x02,0x77,0x10,		// say "You turn a dial on the gadget. The lights go dim."
0x0a,0x09,			// unset gadgetOn
0x00,				// end
// keycard data
0x0b,0xfe,			// title
0x0c,0x5b,0x11,		// "keycard"
0x0b,0xff,			// description
0x0c,0x88,0x02,		// "A flat rectangular piece of synthetic material, magnetically keyed for passing check stations."
0x0b,0x02,			// use keycard
0x02,0x75,0x0e,		// say "You can't use that here!"
0x00,				// end
// magnet data
0x0b,0xfe,			// title
0x0c,0x5d,0x11,		// "magnet"
0x0b,0xff,			// description
0x0c,0xcc,0x02,		// "A horse shoe shaped magnet that you found in the laboratory on the Arcada."
0x0b,0x03,			// use magnet
0x02,0x75,0x0e,		// say "You can't use that here!"
0x00,				// end
// centralControl data
0x0b,0xfe,			// title
0x0c,0x0d,0x05,		// "Central control"
0x0b,0xff,			// description
0x0c,0xda,0x0d,		// "You are in the central control area of the Arcada. The reactor domes pulsate irregularly, as if they had been tampered with."
0x0b,0xfc,			// on enter
0x04,				// when
0x06,				// not
0x05,0x05,			// eval avoidedGuard
0x02,0x4c,0x10,		// say "You think you hear footsteps."
0x0b,0xfd,			// on exit
0x04,				// when
0x06,				// not
0x05,0x05,			// eval avoidedGuard
0x02,0x81,0x07,		// say "Suddenly an alien intruder appears brandishing a large pulse rifle."
0x02,0xf0,0x04,		// say "BANG! In one shot you hit the ground lifeless."
0x02,0x6f,0x09,		// say "The alien observes his handiwork briefly, then leaves."
0x01,0x08,			// go dead
0x03,0x4a,0x11,		// option "east"
0x01,0x10,			// go outsideFlightPrep
0x03,0xa7,0x11,		// option "west"
0x01,0x0f,			// go lowerElevators
0x03,0x56,0x11,		// option "hide"
0x04,				// when
0x06,				// not
0x05,0x05,			// eval avoidedGuard
0x02,0xb2,0x0e,		// say "You hide behind one of the reactor domes, out of view."
0x02,0x97,0x0e,		// say "You hear the footsteps of one of the alien intruders getting louder!"
0x02,0xff,0x0c,		// say "Unable to see anyone here, the alien leaves to search elsewhere."
0x09,0x05,			// set avoidedGuard
0x03,0x56,0x11,		// option "hide"
0x04,				// when
0x05,0x05,			// eval avoidedGuard
0x02,0xb2,0x0e,		// say "You hide behind one of the reactor domes, out of view."
0x02,0x00,0x07,		// say "Nobody comes! You come back out of hiding."
0x03,0x34,0x11,		// option "corpse"
0x02,0x70,0x0f,		// say "You search the body of your crewmate and find..."
0x02,0x6c,0x11,		// say "nothing."
0x00,				// end
// closet data
0x0b,0xfe,			// title
0x0c,0x97,0x06,		// "Janitor's closet"
0x0b,0xff,			// description
0x0c,0x8d,0x03,		// "A small, dark and very cluttered utility closet"
0x03,0x9c,0x11,		// option "south"
0x01,0x11,			// go upperDeck
0x00,				// end
// computer data
0x0b,0xfe,			// title
0x0c,0xa7,0x06,		// "Library computer"
0x0b,0xff,			// description
0x0c,0xd4,0x09,		// "The console consists of a screen, keyboard and cartridge slot. Atop it rests a Model DX Cartridge Retrieval Unit"
0x03,0x4f,0x11,		// option "exit"
0x01,0x0e,			// go library
0x03,0x16,0x11,		// option "astral body"
0x04,				// when
0x05,0x0b,			// eval spokenToScientist
0x07,				// and
0x06,				// not
0x05,0x00,			// have cartridge
0x02,0x58,0x03,		// say "A robot arm springs to action and retrieves a cartridge from one of the many shelves. You take the cartridge."
0x09,0x00,			// give cartridge
0x0b,0x00,			// use cartridge
0x02,0xce,0x0e,		// say "You insert the cartridge into the library computer."
0x02,0x81,0x07,		// say "Suddenly an alien intruder appears brandishing a large pulse rifle."
0x02,0xf0,0x04,		// say "BANG! In one shot you hit the ground lifeless."
0x02,0x6f,0x09,		// say "The alien observes his handiwork briefly, then leaves."
0x01,0x08,			// go dead
0x00,				// end
// corridorEnd data
0x0b,0xfe,			// title
0x0c,0x83,0x05,		// "Dead end"
0x0b,0xff,			// description
0x0c,0xfd,0x0a,		// "The hallway ends here. The corpse of one of your former crewmates lies here. He is nonfunctional."
0x03,0x4a,0x11,		// option "east"
0x01,0x12,			// go upperElevators
0x03,0x34,0x11,		// option "corpse"
0x04,				// when
0x06,				// not
0x05,0x02,			// have keycard
0x02,0x70,0x0f,		// say "You search the body of your crewmate and find..."
0x02,0x11,0x11,		// say "a keycard."
0x09,0x02,			// give keycard
0x03,0x34,0x11,		// option "corpse"
0x04,				// when
0x05,0x02,			// have keycard
0x02,0x70,0x0f,		// say "You search the body of your crewmate and find..."
0x02,0x6c,0x11,		// say "nothing."
0x00,				// end
// dead data
0x0b,0xfe,			// title
0x0c,0x9d,0x05,		// "Game over!"
0x0b,0xfc,			// on enter
0x02,0xa8,0x07,		// say "Thank you for playing Space Quest. Too bad you've failed miserably and doomed all your people to a horrible death at the hands of the Sariens."
0x02,0x27,0x06,		// say "If you continue playing as skillfully as this, we'll never have a chance for a sequel. Better luck next time."
0x03,0x7e,0x11,		// option "restart"
0x02,0x16,0x07,		// say "Not yet implemented!"
0x03,0x82,0x11,		// option "restore"
0x02,0x16,0x07,		// say "Not yet implemented!"
0x00,				// end
// demoFinished data
0x0b,0xfe,			// title
0x0c,0x89,0x05,		// "Demo complete"
0x0b,0xfc,			// on enter
0x02,0x19,0x05,		// say "Congratulations! You have narrowly escaped an explosive death! Don't start patting yourself on the back just yet, though. You are now traveling aimlessly through the cosmos."
0x02,0xd4,0x0c,		// say "This is the end of the demo."
0x00,				// end
// escapePod data
0x0b,0xfe,			// title
0x0c,0x8f,0x05,		// "Escape pod"
0x0b,0xff,			// description
0x0c,0x4e,0x07,		// "Sitting in the pilot seat you see a console with all sorts of buttons and switches."
0x03,0x4f,0x11,		// option "exit"
0x01,0x13,			// go vehicleBay
0x03,0x7a,0x11,		// option "power"
0x04,				// when
0x06,				// not
0x05,0x07,			// eval escapePodSwitchedOn
0x02,0x2c,0x0f,		// say "You press the power button, lights turn on and you hear a beep."
0x09,0x07,			// set escapePodSwitchedOn
0x03,0x7a,0x11,		// option "power"
0x04,				// when
0x05,0x07,			// eval escapePodSwitchedOn
0x02,0x91,0x10,		// say "Your effort to power-down the pod was wisely overridden by the on-board computer."
0x03,0xa0,0x11,		// option "throttle"
0x04,				// when
0x06,				// not
0x05,0x07,			// eval escapePodSwitchedOn
0x02,0x4a,0x0f,		// say "You pull at the throttle but nothing happens."
0x03,0xa0,0x11,		// option "throttle"
0x04,				// when
0x05,0x07,			// eval escapePodSwitchedOn
0x07,				// and
0x06,				// not
0x05,0x06,			// eval bayDoorsOpen
0x02,0x5c,0x0f,		// say "You pull the throttle and the escape pod rumbles to life."
0x02,0xc5,0x0a,		// say "The escape pod smashes into the bay doors and explodes in a spectacular fashion. You die instantly."
0x01,0x08,			// go dead
0x03,0xa0,0x11,		// option "throttle"
0x04,				// when
0x05,0x07,			// eval escapePodSwitchedOn
0x07,				// and
0x05,0x06,			// eval bayDoorsOpen
0x02,0x5c,0x0f,		// say "You pull the throttle and the escape pod rumbles to life."
0x02,0xab,0x0a,		// say "The escape pod moves slowly out of the vehicle bay and into space."
0x01,0x09,			// go demoFinished
0x03,0x62,0x11,		// option "nav"
0x04,				// when
0x06,				// not
0x05,0x07,			// eval escapePodSwitchedOn
0x02,0x17,0x0f,		// say "You press the nav button but nothing happens."
0x03,0x62,0x11,		// option "nav"
0x04,				// when
0x05,0x07,			// eval escapePodSwitchedOn
0x02,0x92,0x09,		// say "The computer beeps and displays a message: Error: cannot initiate auto navigation whilst docked."
0x00,				// end
// flightPrep data
0x0b,0xfe,			// title
0x0c,0x97,0x05,		// "Flight prep"
0x0b,0xff,			// description
0x0c,0x9f,0x0b,		// "There is a sign on the back wall above a closet and drawer. An airlock door is to the west"
0x03,0xa4,0x11,		// option "up"
0x01,0x10,			// go outsideFlightPrep
0x03,0xa7,0x11,		// option "west"
0x01,0x13,			// go vehicleBay
0x03,0x91,0x11,		// option "sign"
0x04,				// when
0x06,				// not
0x05,0x06,			// eval bayDoorsOpen
0x02,0x6a,0x0b,		// say "The sign on the wall suggests that the bay doors are not currently open."
0x03,0x91,0x11,		// option "sign"
0x04,				// when
0x05,0x06,			// eval bayDoorsOpen
0x02,0x85,0x0b,		// say "The sign on the wall warns that the bay doors are currently open."
0x03,0x26,0x11,		// option "closet"
0x04,				// when
0x06,				// not
0x05,0x0d,			// eval wearingFlightSuit
0x02,0x86,0x0f,		// say "You see a flight suit hanging in the closet. You take it and get suited up."
0x09,0x0d,			// set wearingFlightSuit
0x03,0x26,0x11,		// option "closet"
0x04,				// when
0x05,0x0d,			// eval wearingFlightSuit
0x02,0xd1,0x0b,		// say "There is nothing else in the closet."
0x03,0x45,0x11,		// option "drawer"
0x04,				// when
0x06,				// not
0x05,0x01,			// have gadget
0x02,0x81,0x0e,		// say "You find some sort of electrical gadget."
0x09,0x01,			// give gadget
0x03,0x45,0x11,		// option "drawer"
0x04,				// when
0x05,0x01,			// have gadget
0x02,0x70,0x0a,		// say "The drawer is empty."
0x00,				// end
// intro data
0x0b,0xfe,			// title
0x0c,0x72,0x07,		// "Space Quest Chapter 1"
0x0b,0xff,			// description
0x0c,0x89,0x08,		// "The Sarien Encounter  Demake by James Howard Original game designed by Mark Crowe and Scott Murphy."
0x03,0x73,0x11,		// option "play"
0x02,0xad,0x06,		// say "Light-years from Earth's solar system the people of the galaxy Earnon have been struggling to maintain the precious balance of life."
0x02,0x15,0x09,		// say "The Sun of Earnon is slowly dying. The planet grows cold. Food is no longer plentiful. Life will soon become impossible to sustain."
0x02,0x32,0x0b,		// say "The scientific community of Xenon devised a plan to convert one of Earnon's lifeless planets into a new Sun."
0x02,0x7f,0x0a,		// say "The effort was centered around the development of a device called the Star Generator."
0x02,0xd9,0x08,		// say "The Star Generator would be capable of igniting an otherwise useless planet into a raging ball of flame."
0x02,0x7e,0x04,		// say "An expedition set out aboard the starship Arcada to successfully complete development and testing of the Star Generator."
0x02,0x63,0x08,		// say "The Arcada is now returning triumphantly to Xenon with the fully operational Star Generator."
0x02,0xaf,0x0f,		// say "You serve as a member of the crew of the Arcada as a janitor."
0x02,0x04,0x08,		// say "That's right, a janitor. And not a very good one. You would probably have been sacked and replaced were the Arcada not millions of kilometers from Xenon."
0x02,0xac,0x04,		// say "As we join our story, you have just completed one of your famous on-shift naps in one of the janitorial storage closets."
0x01,0x05,			// go closet
0x03,0x93,0x11,		// option "skip intro"
0x01,0x05,			// go closet
0x00,				// end
// lab data
0x0b,0xfe,			// title
0x0c,0xa1,0x06,		// "Laboratory"
0x0b,0xff,			// description
0x0c,0x8b,0x0c,		// "This is the Star Generator development laboratory.  Due to your incredibly low security clearance you have never been allowed access to this room."
0x03,0x9c,0x11,		// option "south"
0x01,0x0f,			// go lowerElevators
0x03,0x34,0x11,		// option "corpse"
0x02,0x70,0x0f,		// say "You search the body of your crewmate and find..."
0x02,0x6c,0x11,		// say "nothing."
0x03,0x2d,0x11,		// option "computer"
0x02,0xc0,0x10,		// say "Your mind is boggled in attempting to understand the function of the various buttons, switches, lights, and displays. It appears the board is not operating."
0x03,0x8c,0x11,		// option "screen"
0x02,0x37,0x0d,		// say "WARNING! Self-Destruct Mechanism Has Been Armed! Evacuate Ship Immediately! THIS IS NOT A TEST!"
0x03,0x3b,0x11,		// option "desk"
0x04,				// when
0x06,				// not
0x05,0x03,			// have magnet
0x02,0x29,0x07,		// say "On the desk you find a magnet."
0x09,0x03,			// give magnet
0x03,0x3b,0x11,		// option "desk"
0x04,				// when
0x05,0x03,			// have magnet
0x02,0xdd,0x0b,		// say "There is nothing else on the desk worth taking."
0x00,				// end
// library data
0x0b,0xfe,			// title
0x0c,0xa4,0x06,		// "Library"
0x0b,0xff,			// description
0x04,				// when
0x06,				// not
0x05,0x0a,			// eval scientistEntered
0x0c,0x2b,0x0c,		// "This is the Arcada's data archive. Many volumes of information are stored on data cartridges."
0x0b,0xff,			// description
0x04,				// when
0x05,0x0a,			// eval scientistEntered
0x0c,0x54,0x0c,		// "This is the Arcada's data archive. Many volumes of information are stored on data cartridges. A scientist lies on the floor."
0x0b,0xfc,			// on enter
0x04,				// when
0x06,				// not
0x05,0x0c,			// eval visitedLibrary
0x09,0x0c,			// set visitedLibrary
0x0b,0xfc,			// on enter
0x04,				// when
0x05,0x0c,			// eval visitedLibrary
0x07,				// and
0x06,				// not
0x05,0x0a,			// eval scientistEntered
0x09,0x0a,			// set scientistEntered
0x02,0x1f,0x0a,		// say "The door opens and a man you recognise as one of the lab scientists enters. He appears to be injured! After only a few steps he slumps to the floor."
0x03,0x4a,0x11,		// option "east"
0x01,0x11,			// go upperDeck
0x03,0xa7,0x11,		// option "west"
0x01,0x12,			// go upperElevators
0x03,0x2d,0x11,		// option "computer"
0x01,0x06,			// go computer
0x03,0x88,0x11,		// option "scientist"
0x04,				// when
0x05,0x0a,			// eval scientistEntered
0x07,				// and
0x06,				// not
0x05,0x0b,			// eval spokenToScientist
0x02,0xf0,0x02,		// say "A large laser hole has been burned in his uniform through which you can see previously unexposed tissue. Struggling painfully he raises up on one elbow."
0x02,0xe8,0x05,		// say "He tells you the Arcada is under attack and the Star Generator is in danger. You had better leave if you value your life."
0x02,0xa6,0x05,		// say "He looks over toward the shelves full of cartridges and utters the words 'astral body'. He then settles to the floor, lifeless."
0x09,0x0b,			// set spokenToScientist
0x03,0x88,0x11,		// option "scientist"
0x04,				// when
0x05,0x0a,			// eval scientistEntered
0x07,				// and
0x05,0x0b,			// eval spokenToScientist
0x02,0x77,0x06,		// say "It's a grisly sight. There is nothing you can do for him now."
0x00,				// end
// lowerElevators data
0x0b,0xfe,			// title
0x0c,0xf9,0x06,		// "Lower deck elevators"
0x0b,0xff,			// description
0x0c,0xee,0x0c,		// "Two elevators connect to the upper deck"
0x03,0xa4,0x11,		// option "up"
0x01,0x12,			// go upperElevators
0x03,0x66,0x11,		// option "north"
0x01,0x0d,			// go lab
0x03,0x4a,0x11,		// option "east"
0x01,0x04,			// go centralControl
0x00,				// end
// outsideFlightPrep data
0x0b,0xfe,			// title
0x0c,0xaf,0x03,		// "Above flight prep"
0x0b,0xff,			// description
0x0c,0xf0,0x03,		// "An elevator here connects to the flight prep room below. There is a console with a large button and a keycard unit."
0x03,0xa7,0x11,		// option "west"
0x01,0x04,			// go centralControl
0x03,0x40,0x11,		// option "down"
0x04,				// when
0x05,0x08,			// eval flightPrepElevatorLocked
0x02,0xb9,0x03,		// say "Alas, the door does not open. You have adorned the door with a small circle of nose grease."
0x03,0x40,0x11,		// option "down"
0x04,				// when
0x06,				// not
0x05,0x08,			// eval flightPrepElevatorLocked
0x01,0x0b,			// go flightPrep
0x03,0x1d,0x11,		// option "button"
0x04,				// when
0x06,				// not
0x05,0x06,			// eval bayDoorsOpen
0x02,0xf1,0x0e,		// say "You press the button. Through a window you see the doors to the vehicle bay slide open."
0x09,0x06,			// set bayDoorsOpen
0x03,0x1d,0x11,		// option "button"
0x04,				// when
0x05,0x06,			// eval bayDoorsOpen
0x02,0xe1,0x0e,		// say "You press the button and the vehicle bay doors close."
0x0a,0x06,			// unset bayDoorsOpen
0x0b,0x02,			// use keycard
0x02,0xcc,0x0f,		// say "You slide the keycard into its slot. You hear an audible click. You take the keycard back."
0x0a,0x08,			// unset flightPrepElevatorLocked
0x00,				// end
// upperDeck data
0x0b,0xfe,			// title
0x0c,0x21,0x0d,		// "Upper deck"
0x0b,0xff,			// description
0x0c,0x3a,0x07,		// "One of the many hallways of the Arcada."
0x0b,0xfc,			// on enter
0x04,				// when
0x06,				// not
0x05,0x04,			// eval alarmSounded
0x02,0x1b,0x0e,		// say "You are startled by the sound of an alarm. It is followed by an urgent voice which warns that the Arcada has been boarded by unknown intruders. It ends abruptly."
0x09,0x04,			// set alarmSounded
0x03,0x66,0x11,		// option "north"
0x01,0x05,			// go closet
0x03,0xa7,0x11,		// option "west"
0x01,0x0e,			// go library
0x00,				// end
// upperElevators data
0x0b,0xfe,			// title
0x0c,0x26,0x0d,		// "Upper deck elevators"
0x0b,0xff,			// description
0x0c,0xdf,0x0c,		// "Two elevators connect to the lower deck"
0x03,0x4a,0x11,		// option "east"
0x01,0x0e,			// go library
0x03,0xa7,0x11,		// option "west"
0x01,0x07,			// go corridorEnd
0x03,0x40,0x11,		// option "down"
0x01,0x0f,			// go lowerElevators
0x00,				// end
// vehicleBay data
0x0b,0xfe,			// title
0x0c,0x2d,0x0d,		// "Vehicle bay"
0x0b,0xff,			// description
0x04,				// when
0x05,0x06,			// eval bayDoorsOpen
0x0c,0x49,0x04,		// "An escape pod sits on a raised platform. Through the open bay doors lies the serenity of deep space."
0x0b,0xff,			// description
0x04,				// when
0x06,				// not
0x05,0x06,			// eval bayDoorsOpen
0x0c,0x21,0x04,		// "An escape pod sits on a raised platform. The massive bay doors are closed."
0x0b,0xfc,			// on enter
0x04,				// when
0x06,				// not
0x05,0x0d,			// eval wearingFlightSuit
0x07,				// and
0x05,0x06,			// eval bayDoorsOpen
0x02,0xfc,0x0f,		// say "You take a few steps into the vehicle bay. Without any form of protection against the vacuum of space, you explode into meaty chunks."
0x01,0x08,			// go dead
0x03,0x4a,0x11,		// option "east"
0x01,0x0b,			// go flightPrep
0x03,0x76,0x11,		// option "pod"
0x01,0x0a,			// go escapePod
0x00,				// end
